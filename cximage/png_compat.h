/*
 * Compatibility macros for libpng 1.6+ with CxImage
 * This file provides backward compatibility by defining macros for opaque
 * struct access
 */

#ifndef CXIMAGE_PNG16_COMPAT_H
#define CXIMAGE_PNG16_COMPAT_H

#if PNG_LIBPNG_VER >= 10600 // libpng 1.6.0 and later

// Helper macros to get/set info_ptr fields using accessor functions

// READ accessors (frequently used)
#define PNG_GET_VALID(png_ptr, info_ptr, flag)                                 \
  png_get_valid(png_ptr, info_ptr, flag)
#define PNG_GET_ROWBYTES(png_ptr, info_ptr) png_get_rowbytes(png_ptr, info_ptr)
#define PNG_GET_WIDTH(png_ptr, info_ptr) png_get_image_width(png_ptr, info_ptr)
#define PNG_GET_HEIGHT(png_ptr, info_ptr)                                      \
  png_get_image_height(png_ptr, info_ptr)
#define PNG_GET_COLOR_TYPE(png_ptr, info_ptr)                                  \
  png_get_color_type(png_ptr, info_ptr)
#define PNG_GET_BIT_DEPTH(png_ptr, info_ptr)                                   \
  png_get_bit_depth(png_ptr, info_ptr)
#define PNG_GET_CHANNELS(png_ptr, info_ptr) png_get_channels(png_ptr, info_ptr)
#define PNG_GET_PIXEL_DEPTH(png_ptr, info_ptr)                                 \
  (png_get_bit_depth(png_ptr, info_ptr) * png_get_channels(png_ptr, info_ptr))
#define PNG_GET_INTERLACE_TYPE(png_ptr, info_ptr)                              \
  png_get_interlace_type(png_ptr, info_ptr)

// WRITE accessors
static inline void
PNG_SET_IHDR_FROM_STRUCT(png_structp png_ptr, png_infop info_ptr,
                         png_uint_32 width, png_uint_32 height, int bit_depth,
                         int color_type, int interlace_type) {
  png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
               interlace_type, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
}

#endif // PNG_LIBPNG_VER >= 10600

#endif // CXIMAGE_PNG16_COMPAT_H
