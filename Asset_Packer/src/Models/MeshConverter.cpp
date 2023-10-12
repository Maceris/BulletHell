#include "MeshConverter.h"


#include "FileUtils.h"
#include "Logger.h"
#include "RawMeshData.h"

#include "Animation.h"
#include "Material.h"
#include "MeshData.h"
#include "Node.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#define NOMINMAX
#include <winsock2.h>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/quaternion.hpp"

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

const std::string ANIMATION_OUTPUT_EXTENSION = ".animation";
const std::string MATERIAL_OUTPUT_EXTENSION = ".material";
const std::string MODEL_OUTPUT_EXTENSION = ".model";

#pragma endregion

#pragma region Structs
struct Bone
{
	int bone_ID;
	std::string bone_name;
	glm::mat4 offset_matrix;
	Bone() = default;
	Bone(const int id, const std::string bone_name, 
		const glm::mat4& offset_matrix)
		: bone_ID(id)
		, bone_name(bone_name)
		, offset_matrix(offset_matrix)
	{}
	Bone(const int id, const std::string bone_name, glm::mat4&& offset_matrix)
		: bone_ID(id)
		, bone_name(bone_name)
		, offset_matrix(std::move(offset_matrix))
	{}
	Bone(const Bone&) = default;
	Bone& operator=(const Bone&) = default;
	~Bone() = default;
};

struct VertexWeight
{
	int bone_ID;
	int vertex_ID;
	float weight;
	VertexWeight() = default;
	VertexWeight(const int bone_ID, const int vertex_ID, const float weight)
		: bone_ID(bone_ID)
		, vertex_ID(vertex_ID)
		, weight(weight)
	{}
	VertexWeight(const VertexWeight&) = default;
	VertexWeight& operator=(const VertexWeight&) = default;
	~VertexWeight() = default;
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
/// <param name="model_path">The folder that results will be placed in.</param>
/// <param name="model_name">The name of the model, without path or extension.
/// </param>
void output_results(ModelResults* results, const fs::path& model_path, 
	const std::string& model_name);

void output_mesh(std::vector<std::shared_ptr<RawMeshData>>& data, 
	const fs::path& model_path, const std::string& model_name);

void output_animation(std::shared_ptr<Animation> animation,
	const fs::path& model_path, const std::string& model_name);

void output_material(std::shared_ptr<Material> material,
	const fs::path& model_path, const std::string& model_name);

/// <summary>
/// For a given frame, go through and build transformation matrices from the
/// root node down recursively.
/// </summary>
/// <param name="animation">The animation data.</param>
/// <param name="bones">The list of bone information.</param>
/// <param name="frame">The frame data.</param>
/// <param name="frame_number">The frame number we are processing.</param>
/// <param name="node">The current node we are processing.</param>
/// <param name="parent_transform">The transformation matrix of the parent
/// node.</param>
/// <param name="global_inverse_transform">The inverse of the root node
/// transformation matrix.</param>
void build_frame_matrices(const aiAnimation* animation,
	const std::vector<Bone>& bones, AnimatedFrame& frame, 
	const int frame_number, Node* node, const glm::mat4& parent_transform,
	const glm::mat4& global_inverse_transform);

/// <summary>
/// Builds a tree of named nodes for later processing.
/// </summary>
/// <param name="raw_node">The node from assimp to process.</param>
/// <param name="parent_node">The parent node, which may be null for the root
/// node.</param>
/// <returns>The converted node.</returns>
Node* build_node_tree(const aiNode* raw_node, Node* parent_node);

/// <summary>
/// Build a ndoe transformation matrix for the specified frame of an 
/// animation.
/// </summary>
/// <param name="node_animation">The animation we are working on.</param>
/// <param name="frame">The frame we want.</param>
/// <returns>The node transformation matrix corresponding to 
/// the specifed frame of the animation.</returns>
glm::mat4 build_node_transform_matrix(const aiNodeAnim* node_animation, 
	const unsigned int frame);

/// <summary>
/// Calculate the maximum number of frames in an animation. Since each node
/// might have a different number of keys, and there are different keys for
/// position, scaling, and rotation, we look for the max overall.
/// </summary>
/// <param name="animation">The animation.</param>
/// <returns>The maximum number of frames for the animation.</returns>
unsigned int calc_animation_max_frames(const aiAnimation* animation);

/// <summary>
/// Process a scene and extract a list of animations.
/// </summary>
/// <param name="scene">The scene we are processing.</param>
/// <param name="bones">The bone data.</param>
/// <param name="root_node">The scene's root node.</param>
/// <param name="global_inverse_transform">The inverse of the root node's
/// transformation relative to the parent.</param>
/// <returns>A list of animations.</returns>
std::vector<std::shared_ptr<Animation>> process_animations(
	const aiScene* scene, std::vector<Bone>& bones,
	Node* root_node, glm::mat4 global_inverse_transform);

/// <summary>
/// Process the mesh and populate the bone list with bone data for use in
/// processing animations, as well as updating the mesh data structure for
/// with bone and weight data for the vertices.
/// </summary>
/// <param name="mesh">The mesh we are processing.</param>
/// <param name="bones">The bone list to populate.</param>
/// <param name="mesh_data">Mesh data to store bone weights in.</param>
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

/// <summary>
/// Write a value to an output stream as a 16 bit integer, in network byte 
/// order (big endian).
/// </summary>
/// <typeparam name="T">The type we are converting from.</typeparam>
/// <param name="value">The value to write.</param>
/// <param name="target">The stream to write to.</param>
template <typename T>
void constexpr write_uint16(const T& value, std::ofstream& target)
{
	static_assert(sizeof(T) <= sizeof(uint16_t), 
		"Provided value does not fit in 16 bits.");
	uint16_t local = htons(std::bit_cast<uint16_t>(value));
	target.write((char*)&local, sizeof(local));
}

/// <summary>
/// Write a value to an output stream as a 32 bit integer, in network byte 
/// order (big endian).
/// </summary>
/// <typeparam name="T">The type we are converting from.</typeparam>
/// <param name="value">The value to write.</param>
/// <param name="target">The stream to write to.</param>
template <typename T>
void constexpr write_uint32(const T& value, std::ofstream& target)
{
	static_assert(sizeof(T) <= sizeof(uint32_t), 
		"Provided value does not fit in 32 bits.");
	uint32_t local = htonl(std::bit_cast<uint32_t>(value));
	target.write((char*)&local, sizeof(local));
}

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
	LOG_TAGGED("MODEL", "Converting model " + source.path().string());
    std::string target_file_name = source.path().string();
	target_file_name.erase(0, ASSET_FOLDER.size());
  
	std::string model_path_string = TEMP_FOLDER + target_file_name;
	auto last_slash = model_path_string.find_last_of(
		fs::path::preferred_separator);
	model_path_string.erase(last_slash, model_path_string.size() - last_slash);

	fs::path model_path(model_path_string);

	target_file_name = source.path().filename().string();
	auto dot = target_file_name.find_last_of("\\.");
	const int extension_size = target_file_name.size() - dot;
	std::string model_name = target_file_name.erase(dot, extension_size);

    fs::path target_path(target_file_name);
	ModelResults* results = import_model(source);
	if (results != nullptr)
	{
		output_results(results, model_path, model_name);
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
		LOG_FATAL("Error loading " + source_path);
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
			LOG_FATAL("Error transforming " + source_path);
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

		delete root_node;
	}
	aiReleaseImport(scene);
	return results;
}

void output_results(ModelResults* results, const fs::path& model_path,
	const std::string& model_name)
{
	LOG_ASSERT(results != nullptr);
	assert(results != nullptr);

	LOG_TAGGED("MODEL", "Meshes: "
		+ std::to_string(results->mesh_data.size()));
	output_mesh(results->mesh_data, model_path, model_name);

	LOG_TAGGED("MODEL", "Materials: "
		+ std::to_string(results->materials.size()));
	for (auto& material : results->materials)
	{
		output_material(material, model_path, model_name);
	}

	LOG_TAGGED("MODEL", "Animations: "
		+ std::to_string(results->animations.size()));
	for (auto& animation : results->animations)
	{
		output_animation(animation, model_path, model_name);
	}
}

void output_mesh(std::vector<std::shared_ptr<RawMeshData>>& data,
	const fs::path& model_path, const std::string& model_name)
{
	uint16_t _tmp16;
	uint32_t _tmp32;

	//TODO(ches) complete this
	const std::string file_name = model_path.string() + '/' + model_name
		+ MODEL_OUTPUT_EXTENSION;
	std::ofstream out(file_name, std::ios::out | std::ios::binary 
		| std::ios::app);
	out.imbue(std::locale::classic());

	const uint32_t file_id = 0xC0DE0001;
	write_uint32(file_id, out);

	const uint16_t version = 1;
	write_uint16(version, out);

	const uint32_t mesh_count = data.size();
	write_uint32(mesh_count, out);
	
	for (auto& mesh : data)
	{
		const uint32_t vertex_count = mesh->positions.size() / 3;
		write_uint32(vertex_count, out);

		LOG_ASSERT(mesh->positions.size() == mesh->normals.size());
		LOG_ASSERT(mesh->positions.size() == mesh->tangents.size());
		LOG_ASSERT(mesh->positions.size() == mesh->bitangents.size());
		LOG_ASSERT(mesh->positions.size() / 3 
			== mesh->texture_coordinates.size() / 2);

		for (int row = 0; row < vertex_count; ++row)
		{
			const int start_pos = row * 3;
			const int start_text_coord = row * 2;
			write_uint32(mesh->positions[start_pos], out);
			write_uint32(mesh->positions[start_pos + 1], out);
			write_uint32(mesh->positions[start_pos + 2], out);
			write_uint32(mesh->normals[start_pos], out);
			write_uint32(mesh->normals[start_pos + 1], out);
			write_uint32(mesh->normals[start_pos + 2], out);
			write_uint32(mesh->tangents[start_pos], out);
			write_uint32(mesh->tangents[start_pos + 1], out);
			write_uint32(mesh->tangents[start_pos + 2], out);
			write_uint32(mesh->bitangents[start_pos], out);
			write_uint32(mesh->bitangents[start_pos + 1], out);
			write_uint32(mesh->bitangents[start_pos + 2], out);
			write_uint32(mesh->texture_coordinates[start_text_coord], out);
			write_uint32(mesh->texture_coordinates[start_text_coord + 1], out);
		}
		const uint32_t index_count = mesh->indices.size();
		write_uint32(index_count, out);
		for (int i = 0; i < index_count; ++i)
		{
			write_uint32(mesh->indices[i], out);
		}
	}

	out.close();
}

void output_animation(std::shared_ptr<Animation> animation,
	const fs::path& model_path, const std::string& model_name)
{
	//TODO(ches) complete this
}

void output_material(std::shared_ptr<Material> material,
	const fs::path& model_path, const std::string& model_name)
{
	//TODO(ches) complete this
}

void build_frame_matrices(const aiAnimation* animation, 
	const std::vector<Bone>& bones, AnimatedFrame& frame,
	const int frame_number, Node* node, const glm::mat4& parent_transform,
	const glm::mat4& global_inverse_transform)
{
	aiNodeAnim* node_animation = nullptr;
	const std::string node_name = node->name;
	const unsigned int channel_count = animation->mNumChannels;
	for (int i = 0; i < channel_count; ++i)
	{
		aiNodeAnim* test_node = animation->mChannels[i];

		if (strcmp(test_node->mNodeName.C_Str(), node_name.c_str()) == 0)
		{
			node_animation = test_node;
		}
	}

	glm::mat4 node_transform;
	if (node_animation != nullptr)
	{
		node_transform = build_node_transform_matrix(node_animation, 
			frame_number);
	}
	else {
		node_transform = node->node_transformation;
	}

	glm::mat4 node_global_transform = glm::mat4(parent_transform)
		* node_transform;

	std::vector<Bone> affected_bones;
	for (auto& bone : bones)
	{
		if (bone.bone_name == node_name)
		{
			affected_bones.push_back(bone);
		}
	}

	for (auto& bone : affected_bones)
	{
		glm::mat4 bone_tranform = glm::mat4(global_inverse_transform)
			* node_global_transform * bone.offset_matrix;
		int bone_ID = bone.bone_ID;
		if (bone_ID < MAX_BONES)
		{
			frame.bone_matrices[bone.bone_ID] = bone_tranform;
		}
		else {
			LOG_WARNING("Ignoring a bone ID of " + std::to_string(bone_ID)
				+ " as it is above the limit of " + std::to_string(MAX_BONES));
		}
	}

	for (Node* child_node : node->children)
	{
		build_frame_matrices(animation, bones, frame, frame_number, child_node,
			node_global_transform, global_inverse_transform);
	}
}
Node* build_node_tree(const aiNode* raw_node, Node* parent_node)
{
	// For fallback IDs
	static int node_ID;

	std::string name;
	if (raw_node->mName.length == 0)
	{
		name = "node" + node_ID;
		++node_ID;
	}
	else
	{
		name = std::string(raw_node->mName.C_Str());
	}

	Node* node = 
		new Node(name, parent_node, to_matrix(raw_node->mTransformation));

	const unsigned int child_count = raw_node->mNumChildren;
	aiNode** const children = raw_node->mChildren;
	for (int i = 0; i < child_count; ++i)
	{
		const aiNode* ai_child = children[i];
		Node* child = build_node_tree(ai_child, node);
		node->add_child(child);
	}

	return node;
}

glm::mat4 build_node_transform_matrix(const aiNodeAnim* node_animation,
	const unsigned int frame)
{
	aiVectorKey* position_keys = node_animation->mPositionKeys;
	aiVectorKey* scaling_keys = node_animation->mScalingKeys;
	aiQuatKey* rotation_keys = node_animation->mRotationKeys;
	
	const unsigned int position_count = node_animation->mNumPositionKeys;
	const unsigned int scaling_count = node_animation->mNumScalingKeys;
	const unsigned int rotation_count = node_animation->mNumRotationKeys;

	// Start with identity, then transform, rotate, and scale it in that order
	glm::mat4 node_transform = glm::mat4(1);
	if (position_count > 0)
	{
		aiVectorKey key = position_keys[std::min(position_count - 1, frame)];
		aiVector3D vector = key.mValue;
		glm::mat4 translation = glm::mat4(
			1, 0, 0, vector.x,
			0, 1, 0, vector.y,
			0, 0, 1, vector.z,
			0, 0, 0, 1
		);
		node_transform *= translation;
	}

	if (rotation_count > 0)
	{
		aiQuatKey key = rotation_keys[std::min(rotation_count - 1, frame)];
		glm::quat quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
		node_transform *= glm::toMat4(quat);
	}
	
	if (scaling_count)
	{
		aiVectorKey key = scaling_keys[std::min(scaling_count - 1, frame)];
		aiVector3D vector = key.mValue;
		glm::mat4 scale = glm::mat4(
			vector.x, 0,        0,        0,
			0,        vector.y, 0,        0,
			0,        0,        vector.z, 0,
			0,        0,        0,        0
		);
		node_transform *= scale;
	}

	return node_transform;
}

unsigned int calc_animation_max_frames(const aiAnimation* animation)
{
	unsigned int max_frames = 0;
	const int channel_counts = animation->mNumChannels;
	aiNodeAnim** const channels = animation->mChannels;
	for (int i = 0; i < channel_counts; ++i)
	{
		const aiNodeAnim* node_animation = channels[i];
		const unsigned int num_frames = std::max(
			std::max(node_animation->mNumPositionKeys, 
			node_animation->mNumScalingKeys),
			node_animation->mNumRotationKeys);
		max_frames = std::max(max_frames, num_frames);
	}
	return max_frames;
}

std::vector<std::shared_ptr<Animation>> process_animations(
	const aiScene* scene, std::vector<Bone>& bones,
	Node* root_node, glm::mat4 global_inverse_transform)
{
	std::vector<std::shared_ptr<Animation>> animations;

	// bones.size() should not be ridiculously large, so down-casting is fine
	const int bone_count = std::min(MAX_BONES, (int) bones.size());

	const unsigned int animation_count = scene->mNumAnimations;
	aiAnimation** const aiAnimations = scene->mAnimations;
	for (int i = 0; i < animation_count; ++i)
	{
		const aiAnimation* aiAnimation = aiAnimations[i];
		const unsigned int max_frames = calc_animation_max_frames(aiAnimation);
		std::vector<AnimatedFrame> frames;
		std::string name;
		
		if (aiAnimation->mName.length <= 0)
		{
			name = "animation" + i;
		} 
		else
		{
			name = std::string(aiAnimation->mName.C_Str());
		}

		for (int j = 0; j < max_frames; ++j)
		{
			AnimatedFrame animatedFrame(bone_count);

			build_frame_matrices(aiAnimation, bones, animatedFrame, j,
				root_node, root_node->node_transformation, 
				global_inverse_transform);
			frames.push_back(animatedFrame);
		}

		std::shared_ptr<Animation> animation = 
			std::make_shared<Animation>(name, aiAnimation->mDuration, frames);
		animations.push_back(animation);
	}

	return animations;
}

void process_bones(const aiMesh* mesh, std::vector<Bone>& bones,
	std::shared_ptr<RawMeshData> mesh_data)
{
	const unsigned int bone_count = mesh->mNumBones;
	aiBone** const ai_bones = mesh->mBones;

	std::map<int, std::vector<VertexWeight>> weight_map;

	for (int i = 0; i < bone_count; ++i)
	{
		const aiBone* ai_bone = ai_bones[i];
		const int id = bones.size();
		std::string bone_name;
		if (ai_bone->mName.length == 0)
		{
			bone_name = "bone" + id;
		}
		else
		{
			bone_name = std::string(ai_bone->mName.C_Str());
		}

		Bone bone(id, bone_name, to_matrix(ai_bone->mOffsetMatrix));
		bones.push_back(bone);

		const unsigned int weight_count = ai_bone->mNumWeights;
		const aiVertexWeight* weights = ai_bone->mWeights;
		for (int j = 0; j < weight_count; ++j)
		{
			aiVertexWeight ai_weight = weights[j];
			VertexWeight weight(bone.bone_ID, ai_weight.mVertexId,
				ai_weight.mWeight);

			if (weight_map.find(weight.vertex_ID) == weight_map.end())
			{
				weight_map.insert(std::make_pair(weight.vertex_ID,
					std::vector<VertexWeight>()));
			}
			std::vector<VertexWeight>& weights = 
				weight_map.find(weight.vertex_ID)->second;
			weights.push_back(weight);
		}
	}

	const unsigned int vertex_count = mesh->mNumVertices;
	for (int i = 0; i < vertex_count; ++i)
	{
		
		unsigned int size = 0;
		auto iter = weight_map.find(i);
		std::vector<VertexWeight>* weights = nullptr;
		if (iter != weight_map.end())
		{
			weights = &iter->second;
			size = weights->size();
		}
		BoneWeights bone_weights = BoneWeights();
		for (int j = 0; j < MAX_WEIGHTS_PER_BONE; ++j)
		{
			if (j < size)
			{
				// Only reachable if weights was not null
				VertexWeight& vw = weights->at(j);
				bone_weights.weights[j] = vw.weight;
				bone_weights.indices[j] = vw.bone_ID;
			}
			else
			{
				bone_weights.weights[j] = 0.0f;
				bone_weights.indices[j] = 0;
			}
		}
		mesh_data->bone_indices.push_back(bone_weights);
	}
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
	const aiVector3D* coordinates_array = mesh->mTextureCoords[0];
	const unsigned int length = mesh->mNumVertices;

	if (coordinates_array == nullptr || length <= 0)
	{
		return;
	}
	for (int i = 0; i < length; ++i)
	{
		const aiVector3D vector = coordinates_array[i];
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