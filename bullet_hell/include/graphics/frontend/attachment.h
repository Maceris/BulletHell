#pragma once

#include <cstdint>

enum class Format;
enum class ImageLayout;

struct Attachment
{
	enum class Flag;
	enum class SampleCount;
	enum class LoadOp;
	enum class StoreOp;

	uint8_t flag_bits;
	Format format;
	uint8_t sample_count_flag_bits;
	LoadOp load_op;
	StoreOp store_op;
	LoadOp stencil_load_op;
	StoreOp stencil_store_op;
	ImageLayout initial_layout;
	ImageLayout final_layout;
};

/// <summary>
/// Specifies additional properties of an attachment.
/// </summary>
enum class Attachment::Flag : uint8_t
{
	NONE = 0x0000'0000,
	/// <summary>
	/// An attachment can be aliased, sharing the same device memory as other
	/// attachments.
	/// </summary>
	CAN_ALIAS = 0x0000'0001
};

/// <summary>
/// Specifies the number of sample counts that are supported for an image.
/// There may be multiple.
/// </summary>
enum class Attachment::SampleCount :uint8_t
{
	SUPPORTS_1  = 0x0000'0001,
	SUPPORTS_2  = 0x0000'0002,
	SUPPORTS_4  = 0x0000'0004,
	SUPPORTS_8  = 0x0000'0008,
	SUPPORTS_16 = 0x0000'0010,
	SUPPORTS_32 = 0x0000'0020,
	SUPPORTS_64 = 0x0000'0040
};

/// <summary>
/// Specifies how attachments are initialized at the beginning of a subpass.
/// </summary>
enum class Attachment::LoadOp : uint8_t
{
	/// <summary>
	/// The previous contents of the render area will be preserved as initial
	/// values.
	/// </summary>
	LOAD,
	/// <summary>
	/// The contents of the render area will be cleared to a uniform value.
	/// </summary>
	CLEAR,
	/// <summary>
	/// The previous contents of the render area do not need to be preserved,
	/// the content is undefined.
	/// </summary>
	DONT_CARE,
	/// <summary>
	/// The contents of the image is undefined, and it is not used at all.
	/// </summary>
	NONE
};

/// <summary>
/// Specifies how attachments are stored to memory at the end of a subpass.
/// </summary>
enum class Attachment::StoreOp : uint8_t
{
	/// <summary>
	/// Contents generated during the render pass, within the render area,
	/// are written to memory.
	/// </summary>
	STORE,
	/// <summary>
	/// The contents of the render area are not needed after rendering,
	/// and can be discarded.
	/// </summary>
	DONT_CARE,
	/// <summary>
	/// The render area will not be written to.
	/// </summary>
	NONE
};
