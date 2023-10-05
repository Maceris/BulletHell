#include "MeshConverter.h"

#include "FileUtils.h"
#include "RawMeshData.h"

#include "Animation.h"
#include "Material.h"
#include "MeshData.h"
#include "Node.h"

#include <iostream>
#include <vector>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "gtc/matrix_transform.hpp"

#pragma region Constants
const unsigned int DEFAULT_POST_PROCESS_FLAGS = 
    /*
      Generates smooth normals for all vertices in the mesh unless the normals
	  are already there at the time this flag is evaluated.
    */
      aiProcess_GenSmoothNormals
    /*
      Reduces number of vertices by reusing identical vertices between faces.
    */
    | aiProcess_JoinIdenticalVertices
	/*
	  Splits all the faces into triangles in case it uses quads or other
	  geometry.
	*/
	| aiProcess_Triangulate
	/*
	   Tries to fix normals that point inwards by reversing them.
	 */
	| aiProcess_FixInfacingNormals
	/*
	   Used for lighting, calculates tangents and bitangents from normals.
	 */
	| aiProcess_CalcTangentSpace
	/*
	   Limits the number of bone weights that affect any particular vertex. =
	   The default bone weight limit is 4.
	 */
	| aiProcess_LimitBoneWeights
	/*
	   Finds and removes redundant/unreferenced materials.
	*/
	| aiProcess_RemoveRedundantMaterials;

const std::string OUTPUT_EXTENSION = ".model";

#pragma endregion

#pragma region Structs
struct Bone
{
	int bone_ID;
	std::string bone_name;
	glm::mat4 offset_matrix;
};

struct VertexWeight
{
	int bone_ID;
	int vertex_ID;
	float weight;
};

/// <summary>
/// All of the things that we have loaded from the model file.
/// </summary>
struct ModelResults
{
	std::vector<std::shared_ptr<Animation>> animations;
	std::vector<std::shared_ptr<RawMeshData>> mesh_data;
	std::vector<std::shared_ptr<Material>> materials;

};

#pragma endregion

#pragma region Function definitions

/// <summary>
/// Append a an assimp vector array to a vector of floats, in x, y, z order.
/// </summary>
/// <param name="target">The array of vectors to append.</param>
/// <param name="length">The length of the vector array.</param>
/// <param name="vector">The destination float vector.</param>
inline void append_vector_array(const aiVector3D* vector_array,
	const int length, std::vector<float>& destination);

/// <summary>
/// Import a model, process it, and return results that we will output to a 
/// file.
/// </summary>
/// <param name="source">The source file.</param>
ModelResults* import_model(const fs::directory_entry& source);

/// <summary>
/// Output the results from loading a model to a file.
/// </summary>
/// <param name="results">The results from importing.</param>
/// <param name="expected_path">The path to the output model file.</param>
void output_results(ModelResults* results,
	fs::path expected_path);
void build_frame_matrices();
Node* build_node_tree(aiNode* raw_node, Node* parent_node);
void build_node_transform_matrix();
void calc_animation_max_frames();
std::vector<std::shared_ptr<Animation>> process_animations(
	const aiScene* scene, std::vector<Bone>& bones,
	Node* root_node, glm::mat4 global_inverse_transform);
void process_bones(const aiMesh* mesh, std::vector<Bone>& bones,
	std::shared_ptr<RawMeshData> mesh_data);

/// <summary>
/// Process all of the indices for a mesh and store them in the mesh data.
/// </summary>
/// <param name="mesh">The mesh to process.</param>
/// <param name="mesh_data">The storage for mesh data.</param>
void process_indices(const aiMesh* mesh,
	std::shared_ptr<RawMeshData> mesh_data);

/// <summary>
/// Process a material into something easier to export. Any colors not
/// specified are set to the default color.
/// </summary>
/// <param name="raw_material">The assimp material.</param>
/// <returns>A material reference.</returns>
std::shared_ptr<Material> process_material(const aiMaterial* raw_material);

/// <summary>
/// Process an assimp mesh into raw mesh data and return it.
/// </summary>
/// <param name="mesh">The assimp mesh.</param>
/// <param name="bones">A list of bone information to populate, for later
/// use in calculating animations.</param>
/// <returns>The resulting mesh data information.</returns>
std::shared_ptr<RawMeshData> process_mesh(const aiMesh* mesh, 
	std::vector<Bone>& bones);

/// <summary>
/// Process texture coordinates for the first texture for a mesh, converting it
/// to a buffer of x and then (1-y) values for each coordinate, and storing
/// it in the mesh data's texture coordinate storage.
/// </summary>
/// <param name="mesh">The mesh to process.</param>
/// <param name="mesh_data">The place to store the coordinate data.</param>
void process_texture_coordinates(const aiMesh* mesh,
	std::shared_ptr<RawMeshData> mesh_data);

/// <summary>
/// Convert an assimp 4x4 matrix to a glm one.
/// </summary>
/// <param name="matrix">The matrix from assimp.</param>
/// <returns>The glm version.</returns>
constexpr glm::mat4 to_matrix(const aiMatrix4x4& matrix);

/// <summary>
/// Convert a color4d to a glm vector.
/// </summary>
/// <param name="color">The color.</param>
/// <returns>A 4d vector with the same rgba values.</returns>
constexpr glm::vec4 to_vector(const aiColor4D& color);

#pragma endregion

inline void append_vector_array(const aiVector3D* vector_array,
	const int length, std::vector<float>& destination)
{
	if (vector_array == nullptr || length <= 0)
	{
		return;
	}
	for (int i = 0; i < length; ++i)
	{
		const aiVector3D vector = vector_array[i];
		destination.push_back(vector.x);
		destination.push_back(vector.y);
		destination.push_back(vector.z);
	}
}

void MeshConverter::convert_model(const fs::directory_entry& source)
{
    std::string target_file_name = source.path().string();
    target_file_name = target_file_name.erase(0, ASSET_FOLDER.size());
  
    // Swap extensions
    auto dot = target_file_name.find_last_of("\\.");
    const int extension_size = target_file_name.size() - dot;
    target_file_name = target_file_name.erase(dot, extension_size);
    target_file_name = target_file_name + OUTPUT_EXTENSION;
    
    target_file_name = TEMP_FOLDER + target_file_name;

    fs::path target_path(target_file_name);
	ModelResults* results = import_model(source);
	if (results != nullptr)
	{
		output_results(results, target_path);
		delete results;
		results = nullptr;
	}
}

ModelResults* import_model(const fs::directory_entry& source)
{
	const std::string source_path = source.path().string();
	const aiScene* scene = aiImportFile(source_path.c_str(),
		DEFAULT_POST_PROCESS_FLAGS);
	if (scene == nullptr)
	{
		std::cout << "Error loading " << source_path << std::endl;
		return nullptr;
	}
	
	const bool animated = scene->mNumAnimations > 0;
	if (!animated)
	{
		/*
		  This flag interferes with / removes animations, but helps fix static
		  meshes for use in OpenGL.
		*/
		const aiScene* result  = aiApplyPostProcessing(scene, 
			aiProcess_PreTransformVertices);

		if (result == nullptr)
		{
			std::cout << "Error transforming " << source_path << std::endl;
			return nullptr;
		}
		// scene was modified in place
	}
	ModelResults* results = new ModelResults();

	const int material_count = scene->mNumMaterials;
	for (int i = 0; i < material_count; ++i)
	{
		const aiMaterial* raw_material = scene->mMaterials[i];
		std::shared_ptr<Material> material = process_material(raw_material);
		results->materials.push_back(std::move(material));
	}

	std::vector<Bone> bones;
	const int mesh_count = scene->mNumMeshes;
	for (int i = 0; i < mesh_count; ++i)
	{
		const aiMesh* raw_mesh = scene->mMeshes[i];
		std::shared_ptr<RawMeshData> mesh_data = process_mesh(raw_mesh, bones);
		results->mesh_data.push_back(std::move(mesh_data));
	}

	
	if (animated)
	{
		Node* root_node = build_node_tree(scene->mRootNode, nullptr);
		const glm::mat4 global_inverse_transform = glm::inverse(to_matrix(
			scene->mRootNode->mTransformation));
		results->animations = process_animations(scene, bones, root_node,
			global_inverse_transform);
	}

	aiReleaseImport(scene);
	return results;
}

void output_results(ModelResults* results, fs::path expected_path)
{
	//TODO(ches) complete this
}

void build_frame_matrices()
{
	//TODO(ches) complete this
}
Node* build_node_tree(aiNode* raw_node, Node* parent_node)
{
	//TODO(ches) complete this
	return nullptr;
}

void build_node_transform_matrix()
{
	//TODO(ches) complete this
}

void calc_animation_max_frames()
{
	//TODO(ches) complete this
}

std::vector<std::shared_ptr<Animation>> process_animations(
	const aiScene* scene, std::vector<Bone>& bones,
	Node* root_node, glm::mat4 global_inverse_transform)
{
	//TODO(ches) complete this
	std::vector<std::shared_ptr<Animation>> animations;
	return animations;
}

void process_bones(const aiMesh* mesh, std::vector<Bone>& bones,
	std::shared_ptr<RawMeshData> mesh_data)
{
	//TODO(ches) complete this
}

void process_indices(const aiMesh* mesh, 
	std::shared_ptr<RawMeshData> mesh_data)
{
	const int face_count = mesh->mNumFaces;
	const aiFace* faces = mesh->mFaces;
	
	for (int face_index = 0; face_index < face_count; ++face_index)
	{
		const aiFace face = faces[face_index];
		const unsigned int indices_count = face.mNumIndices;
		for (int index = 0; index < indices_count; ++index)
		{
			mesh_data->indices.push_back(face.mIndices[index]);
		}
	}
}

std::shared_ptr<Material> process_material(const aiMaterial* assimp_material)
{
	std::shared_ptr<Material> material = std::make_shared<Material>();

	aiColor4D color;

	aiReturn result = aiGetMaterialColor(assimp_material,
		AI_MATKEY_COLOR_AMBIENT, &color);
	if (result == aiReturn_SUCCESS)
	{
		material->ambient_color = to_vector(color);
	}

	result = aiGetMaterialColor(assimp_material, AI_MATKEY_COLOR_DIFFUSE,
		&color);
	if (result == aiReturn_SUCCESS)
	{
		material->diffuse_color = to_vector(color);
	}

	result = aiGetMaterialColor(assimp_material, AI_MATKEY_COLOR_SPECULAR,
		&color);
	if (result == aiReturn_SUCCESS)
	{
		material->specular_color = to_vector(color);
	}

	ai_real shininess = 0.0f ;
	result = aiGetMaterialFloat(assimp_material, AI_MATKEY_SHININESS_STRENGTH,
		&shininess);
	if (result == aiReturn_SUCCESS)
	{
		material->reflectance = shininess;
	}

	aiString aiDiffusePath;
	aiGetMaterialTexture(assimp_material, aiTextureType_DIFFUSE, 0,
		&aiDiffusePath);
	std::string diffusePath(aiDiffusePath.C_Str());

	if (!diffusePath.empty())
	{
		material->texture_name = diffusePath;
	}

	aiString aiNormalPath;
	aiGetMaterialTexture(assimp_material, aiTextureType_DIFFUSE, 0,
		&aiNormalPath);
	std::string normalPath(aiNormalPath.C_Str());

	if (!normalPath.empty())
	{
		material->normal_map_name = normalPath;
	}

	return material;
}

std::shared_ptr<RawMeshData> process_mesh(const aiMesh* mesh,
	std::vector<Bone>& bones)
{
	std::shared_ptr<RawMeshData> result = std::make_shared<RawMeshData>();

	append_vector_array(mesh->mVertices, mesh->mNumVertices,
		result->positions);
	append_vector_array(mesh->mNormals, mesh->mNumVertices,
		result->normals);
	append_vector_array(mesh->mTangents, mesh->mNumVertices,
		result->tangents);
	append_vector_array(mesh->mBitangents, mesh->mNumVertices,
		result->bitangents);

	process_texture_coordinates(mesh, result);
	process_indices(mesh, result);
	process_bones(mesh, bones, result);

	result->material_id = mesh->mMaterialIndex;

	return result;
}

void process_texture_coordinates(const aiMesh* mesh,
	std::shared_ptr<RawMeshData> mesh_data)
{
	const aiVector3D* const *coordinates_array = mesh->mTextureCoords;
	const unsigned int length = mesh->mNumVertices;

	if (coordinates_array == nullptr || length <= 0)
	{
		return;
	}
	for (int i = 0; i < length; ++i)
	{
		const aiVector3D vector = *coordinates_array[i];
		mesh_data->texture_coordinates.push_back(vector.x);
		mesh_data->texture_coordinates.push_back(1 - vector.y);
	}
}

constexpr glm::mat4 to_matrix(const aiMatrix4x4& matrix)
{
	return glm::mat4(
		matrix.a1, matrix.a2, matrix.a3, matrix.a4,
		matrix.b1, matrix.b2, matrix.b3, matrix.b4,
		matrix.c1, matrix.c2, matrix.c3, matrix.c4,
		matrix.d1, matrix.d2, matrix.d3, matrix.d4
	);
}

constexpr glm::vec4 to_vector(const aiColor4D& color)
{
	return glm::vec4(color.r, color.g, color.b, color.a);
}