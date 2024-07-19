#pragma once

#include <cstdint>

enum class ImageLayout :uint8_t
{
    /// <summary>
    /// Unknown layout. Memory cannot transition to this layout, but it can be
    /// the initial layout. In that case, the contents are undefined.
    /// </summary>
    UNDEFINED,
    /// <summary>
    /// Any kind of device access.
    /// </summary>
    GENERAL,
    /// <summary>
    /// Used only as a color or resolve attachment in a frame buffer.
    /// </summary>
    COLOR_ATTACHMENT,
    /// <summary>
    /// Specifies a layout for both depth and stencil formats, allowing for
    /// both read and write access as a depth / stencil attachment.
    /// Equivalent to both DEPTH_ATTACHMENT and STENCIL_ATTACHMENT.
    /// </summary>
    DEPTH_STENCIL_ATTACHMENT,
    /// <summary>
    /// Specifies a layout for both depth and stencil formats, allowing for
    /// read-only access as a depth/stencil attachment. Equivalent to both
    /// DEPTH_READ_ONLY and STENCIL_READ_ONLY.
    /// </summary>
    DEPTH_STENCIL_READ_ONLY,
    /// <summary>
    /// Specifies a layout allowing read-only access in a shader as a sampled
    /// image, combined image/sampler, or input attachment. Only valid for
    /// images that allow sampling or usage as an input attachment.
    /// </summary>
    SHADER_READ_ONLY,
    /// <summary>
    /// Used only as a source image for a transfer command.
    /// </summary>
    TRANSFER_SOURCE,
    /// <summary>
    /// Used only as a destination image for a transfer command.
    /// </summary>
    TRANSFER_DESTINATION,
    /// <summary>
    /// Memory that is in a defined layout, but not been initialized. Memory
    /// cannot transition to this layout, but it can be the initial layout.
    /// Data can be written to this without transitioning layouts.
    /// </summary>
    PREINITIALIZED,
    /// <summary>
    /// Specifies a layout for both depth and stencil formats, allowing for
    /// read and write access to the stencil aspect, and read - only access to
    /// the depth aspect as a depth attachment, or in shaders as a sampled
    /// image, combined image / sampler, or input attachment.Equivalent to
    /// both DEPTH_READ_ONLY and STENCIL_ATTACHMENT.
    /// </summary>
    DEPTH_READ_ONLY_STENCIL_ATTACHMENT,
    /// <summary>
    /// Specifies a layout for both depth and stencil formats, allowing for
    /// read and write access to the depth aspect as a depth attachment, and
    /// read - only access to the stencil aspect as a stencil attachment, or
    /// in shaders as a sampled image, combined image / sampler, or input
    /// attachment.Equivalent to both DEPTH_ATTACHMENT and STENCIL_READ_ONLY.
    /// </summary>
    DEPTH_ATTACHMENT_STENCIL_READ_ONLY,
    /// <summary>
    /// Specifies a layout for the depth aspect of a depth/stencil format
    /// image, allowing read and write access as a depth attachment, or in
    /// shaders as a sample image, combined image / sampler, or input
    /// attachment.
    /// </summary>
    DEPTH_ATTACHMENT,
    /// <summary>
    /// Specifies a layout for the depth aspect of a depth/stencil format
    /// image, allowing read-only access as a depth attachment, or in shaders
    /// as a sample image, combined image / sampler, or input attachment.
    /// </summary>
    DEPTH_READ_ONLY,
    /// <summary>
    /// Specifies a layout for the stencil aspect of a depth/stencil format
    /// image, allowing read and write access as a depth attachment, or in
    /// shaders as a sample image, combined image / sampler, or input
    /// attachment.
    /// </summary>
    STENCIL_ATTACHMENT,
    /// <summary>
    /// Specifies a layout for the stencil aspect of a depth/stencil format
    /// image, allowing read-only access as a depth attachment, or in shaders
    /// as a sample image, combined image / sampler, or input attachment.
    /// </summary>
    STENCIL_READ_ONLY,
    /// <summary>
    /// Specifies a layout allowing read-only access as an attachment, or in
    /// shaders as a sampled image, combined image / sampler, or input
    /// attachment.
    /// </summary>
    READ_ONLY,
    /// <summary>
    /// A layout that must only be used with attachment access in the pipeline.
    /// </summary>
    ATTACHMENT,
    /// <summary>
    /// Used only for presenting an image for display.
    /// </summary>
    PRESENT
};
