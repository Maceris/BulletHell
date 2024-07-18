# Third Party Libraries

CMAKE_MINIMUM_REQUIRED(VERSION 3.14)

SET(BUILD_SHARED_LIBS ON)
SET(ASSIMP_BUILD_TESTS OFF)
SET(ASSIMP_INSTALL OFF)

SET(GLFW_BUILD_EXAMPLES OFF)
SET(GLFW_BUILD_TESTS OFF)
SET(GLFW_BUILD_DOCS OFF)

ADD_SUBDIRECTORY(assimp-5.4.2)
ADD_SUBDIRECTORY(glfw-3.3.8)

SET(IMGUI_DIR ${THIRD_PARTY_DIR}/imgui-1.89.9)
SET(GLFW_DIR ${THIRD_PARTY_DIR}/glfw-3.3.8)

SET(IMGUI_HEADERS
  ${IMGUI_DIR}/imconfig.h
  ${IMGUI_DIR}/imgui.h
  ${IMGUI_DIR}/imgui_internal.h
  ${IMGUI_DIR}/imstb_rectpack.h
  ${IMGUI_DIR}/imstb_textedit.h
  ${IMGUI_DIR}/imstb_truetype.h
  ${IMGUI_DIR}/backends/imgui_impl_glfw.h
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.h
  ${IMGUI_DIR}/backends/imgui_impl_win32.h
  ${IMGUI_DIR}/misc/cpp/imgui_stdlib.h
)

INCLUDE_DIRECTORIES(BEFORE
  ${IMGUI_DIR}/
  ${IMGUI_DIR}/backends
  ${IMGUI_DIR}/misc/cpp
  ${GLFW_DIR}/include
)

SET(IMGUI_SOURCES
  ${THIRD_PARTY_DIR}/imgui-1.89.9/imgui.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/imgui_demo.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/imgui_draw.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/imgui_tables.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/imgui_widgets.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/backends/imgui_impl_glfw.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/backends/imgui_impl_opengl3.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/backends/imgui_impl_win32.cpp
  ${THIRD_PARTY_DIR}/imgui-1.89.9/misc/cpp/imgui_stdlib.cpp
)

SET(ZIPLIB_HEADERS
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/bzip2/bzip2_decoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/bzip2/bzip2_decoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/bzip2/bzip2_encoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/bzip2/bzip2_encoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/compression_interface.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/deflate/deflate_decoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/deflate/deflate_decoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/deflate/deflate_encoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/deflate/deflate_encoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/detail/lzma_alloc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/detail/lzma_handle.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/detail/lzma_header.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/detail/lzma_in_stream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/detail/lzma_out_stream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/lzma_decoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/lzma_decoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/lzma_encoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/lzma/lzma_encoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/store/store_decoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/store/store_decoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/store/store_encoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/compression/store/store_encoder_properties.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/EndOfCentralDirectoryBlock.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/ZipCentralDirectoryFileHeader.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/ZipGenericExtraField.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/ZipLocalFileHeader.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/bzlib.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/bzlib_private.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7z.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zAlloc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zBuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zCrc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zFile.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zVersion.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Alloc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Bcj2.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Bra.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/CpuArch.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Delta.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzFind.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzFindMt.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzHash.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma2Dec.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma2Enc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma86.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzmaDec.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzmaEnc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzmaLib.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/MtCoder.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Ppmd.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Ppmd7.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/RotateDefs.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Sha256.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Threads.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Types.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Xz.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/XzCrc64.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/XzEnc.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/crc32.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/deflate.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/gzguts.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inffast.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inffixed.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inflate.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inftrees.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/trees.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/zconf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/zlib.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/zutil.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/methods/Bzip2Method.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/methods/DeflateMethod.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/methods/ICompressionMethod.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/methods/LzmaMethod.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/methods/StoreMethod.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/methods/ZipMethodResolver.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/compression_decoder_stream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/compression_encoder_stream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/crc32stream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/memstream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/nullstream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/serialization.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/compression_decoder_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/compression_encoder_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/crc32_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/mem_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/null_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/sub_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/tee_streambuff.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/streambuffs/zip_crypto_streambuf.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/substream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/teestream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/streams/zip_cryptostream.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/utils/enum_utils.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/utils/stream_utils.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/utils/time_utils.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/ZipArchive.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/ZipArchiveEntry.h
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/ZipFile.h
)

SET(ZIPLIB_SOURCES
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/EndOfCentralDirectoryBlock.cpp
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/ZipCentralDirectoryFileHeader.cpp
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/ZipGenericExtraField.cpp
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/detail/ZipLocalFileHeader.cpp
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/ZipArchive.cpp
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/ZipArchiveEntry.cpp
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/ZipFile.cpp

  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/blocksort.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/bzerror.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/bzlib.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/compress.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/crctable.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/decompress.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/huffman.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/randtable.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zAlloc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zBuf.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zBuf2.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zCrc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zCrcOpt.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zDec.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zFile.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zIn.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/7zStream.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Alloc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Bcj2.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Bra.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Bra86.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/BraIA64.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/CpuArch.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Delta.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzFind.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzFindMt.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma2Dec.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma2Enc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma86Dec.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Lzma86Enc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzmaDec.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzmaEnc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/LzmaLib.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/MtCoder.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Ppmd7.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Ppmd7Dec.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Ppmd7Enc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Sha256.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Threads.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/Xz.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/XzCrc64.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/XzDec.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/XzEnc.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/XzIn.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/adler32.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/compress.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/crc32.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/deflate.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/infback.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inffast.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inflate.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/inftrees.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/trees.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/uncompr.c
  ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/zutil.c
)

SET(HEADER_FILES
  ${IMGUI_HEADERS}
  ${THIRD_PARTY_DIR}/glfw-3.3.8/include/GLFW/glfw3.h
  ${THIRD_PARTY_DIR}/glfw-3.3.8/include/GLFW/glfw3native.h
  ${THIRD_PARTY_DIR}/PerlinNoise/PerlinNoise.hpp
  ${ZIPLIB_HEADERS}
)

SET(SOURCE_FILES
  ${IMGUI_SOURCES}
  ${THIRD_PARTY_DIR}/CppDelegates/stdafx.cpp
  ${THIRD_PARTY_DIR}/glad/src/glad.c
  ${ZIPLIB_SOURCES}
)

ADD_LIBRARY(ThirdParty STATIC ${HEADER_FILES} ${SOURCE_FILES})
TARGET_USE_COMMON_OUTPUT_DIRECTORY(ThirdParty)

TARGET_LINK_LIBRARIES(ThirdParty glfw)

INSTALL(
  TARGETS ThirdParty
)

set_property(TARGET assimp PROPERTY FOLDER "ThirdParty")
set_property(TARGET glfw PROPERTY FOLDER "ThirdParty")
set_property(TARGET ThirdParty PROPERTY FOLDER "ThirdParty")
set_property(TARGET update_mappings PROPERTY FOLDER "ThirdParty")
set_property(TARGET UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTY FOLDER "ThirdParty")
set_property(TARGET zlibstatic PROPERTY FOLDER "ThirdParty")

SOURCE_GROUP(TREE ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib PREFIX "ZipLib Source Files" FILES ${ZIPLIB_SOURCES})
SOURCE_GROUP(TREE ${THIRD_PARTY_DIR}/ziplib/Source/ZipLib PREFIX "ZipLib Header Files" FILES ${ZIPLIB_HEADERS})
