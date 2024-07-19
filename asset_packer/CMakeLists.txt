# Asset Packer

CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

SET(HEADER_PATH ${ASSET_PACKER_DIR}/include)
SET(SOURCE_PATH ${ASSET_PACKER_DIR}/src)

ADD_DEFINITIONS(-DGLFW_INCLUDE_NONE)

SET(GLM_HEADERS
  ${GLM_DIR}/glm/common.hpp
  ${GLM_DIR}/glm/exponential.hpp
  ${GLM_DIR}/glm/ext.hpp
  ${GLM_DIR}/glm/fwd.hpp
  ${GLM_DIR}/glm/geometric.hpp
  ${GLM_DIR}/glm/glm.hpp
  ${GLM_DIR}/glm/integer.hpp
  ${GLM_DIR}/glm/mat2x2.hpp
  ${GLM_DIR}/glm/mat2x3.hpp
  ${GLM_DIR}/glm/mat2x4.hpp
  ${GLM_DIR}/glm/mat3x2.hpp
  ${GLM_DIR}/glm/mat3x3.hpp
  ${GLM_DIR}/glm/mat3x4.hpp
  ${GLM_DIR}/glm/mat4x2.hpp
  ${GLM_DIR}/glm/mat4x3.hpp
  ${GLM_DIR}/glm/mat4x4.hpp
  ${GLM_DIR}/glm/matrix.hpp
  ${GLM_DIR}/glm/packing.hpp
  ${GLM_DIR}/glm/trigonometric.hpp
  ${GLM_DIR}/glm/vec2.hpp
  ${GLM_DIR}/glm/vec3.hpp
  ${GLM_DIR}/glm/vec4.hpp
  ${GLM_DIR}/glm/vector_relational.hpp
)

SET(THIRD_PARTY_HEADERS
  ${GLM_HEADERS}
)

SET(HEADER_FILES
  ${HEADER_PATH}/file_utils.h
  ${HEADER_PATH}/packer_constants.h
  ${HEADER_PATH}/images/image_converter.h
  ${HEADER_PATH}/models/mesh_converter.h
  ${HEADER_PATH}/models/node.h
  ${HEADER_PATH}/models/raw_mesh_data.h
)

SET(SOURCE_FILES
  ${SOURCE_PATH}/file_utils.cpp
  ${SOURCE_PATH}/main.cpp
  ${SOURCE_PATH}/packer_constants.cpp
  ${SOURCE_PATH}/images/image_converter.cpp
  ${SOURCE_PATH}/models/mesh_converter.cpp
  ${SOURCE_PATH}/models/node.cpp
)

INCLUDE_DIRECTORIES(BEFORE
  ${HEADER_PATH}/
  ${GLM_DIR}/
  ${COMMON_INCLUDE_DIR}/
  ${THIRD_PARTY_DIR}/assimp-5.4.2/include
  ${THIRD_PARTY_DIR}/glad/include/glad
  ${THIRD_PARTY_DIR}/stb
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib
)

SOURCE_GROUP(TREE ${COMMON_INCLUDE_DIR} PREFIX "common-include" FILES ${COMMON_HEADERS})
SOURCE_GROUP(TREE ${COMMON_SOURCE_DIR} PREFIX "common-src" FILES ${COMMON_SOURCES})
SOURCE_GROUP(TREE ${HEADER_PATH} PREFIX "include" FILES ${HEADER_FILES})
SOURCE_GROUP(TREE ${SOURCE_PATH} PREFIX "src" FILES ${SOURCE_FILES})

ADD_EXECUTABLE(AssetPacker
  ${HEADER_FILES}
  ${COMMON_HEADERS}
  ${THIRD_PARTY_HEADERS}
  
  ${SOURCE_FILES}
  ${COMMON_SOURCES}
)

TARGET_USE_COMMON_OUTPUT_DIRECTORY(AssetPacker)

TARGET_LINK_LIBRARIES(AssetPacker assimp ThirdParty ws2_32)
