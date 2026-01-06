/*
 * File:	ximapng.cpp
 * Purpose:	Platform Independent PNG Image Class Loader and Writer
 * 07/Aug/2001 Davide Pizzolato - www.xdp.it
 * CxImage version 6.0.0 02/Feb/2008
 */

#include "ximapng.h"
#include "png_compat.h"

#if CXIMAGE_SUPPORT_PNG

#include "ximaiter.h"

////////////////////////////////////////////////////////////////////////////////
void CxImagePNG::ima_png_error(png_struct *png_ptr, char *message) {
  strcpy(info.szLastError, message);
  png_longjmp(png_ptr, 1);
}
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_DECODE
////////////////////////////////////////////////////////////////////////////////
void CxImagePNG::expand2to4bpp(BYTE *prow) {
  BYTE *psrc, *pdst;
  BYTE pos, idx;
  for (long x = head.biWidth - 1; x >= 0; x--) {
    psrc = prow + ((2 * x) >> 3);
    pdst = prow + ((4 * x) >> 3);
    pos = (BYTE)(2 * (3 - x % 4));
    idx = (BYTE)((*psrc & (0x03 << pos)) >> pos);
    pos = (BYTE)(4 * (1 - x % 2));
    *pdst &= ~(0x0F << pos);
    *pdst |= (idx & 0x0F) << pos;
  }
}
////////////////////////////////////////////////////////////////////////////////
bool CxImagePNG::Decode(CxFile *hFile) {
  png_struct *png_ptr;
  png_info *info_ptr;
  BYTE *row_pointers = NULL;
  CImageIterator iter(this);

  cx_try {
    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also supply the
     * the compiler header file version, so that we know if the application
     * was compiled with a compatible version of the library.  REQUIRED    */
    png_ptr =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, (void *)NULL, NULL, NULL);
    if (png_ptr == NULL)
      cx_throw("Failed to create PNG structure");

    /* Allocate/initialize the memory for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
      png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
      cx_throw("Failed to initialize PNG info structure");
    }

    /* Set error handling if you are using the setjmp/longjmp method (this is
     * the normal method of doing things with libpng).  REQUIRED unless you
     * set up your own error handlers in the png_create_read_struct() earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      /* Free all of the memory associated with the png_ptr and info_ptr */
      delete[] row_pointers;
      png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
      cx_throw("");
    }

    // use custom I/O functions
    png_set_read_fn(png_ptr, hFile, /*(png_rw_ptr)*/ user_read_data);
    png_set_error_fn(png_ptr, info.szLastError,
                     /*(png_error_ptr)*/ user_error_fn, NULL);

    /* read the file information */
    png_read_info(png_ptr, info_ptr);

    if (info.nEscape == -1) {
      head.biWidth = png_get_image_width(png_ptr, info_ptr);
      head.biHeight = png_get_image_height(png_ptr, info_ptr);
      info.dwType = CXIMAGE_FORMAT_PNG;
      longjmp(png_jmpbuf(png_ptr), 1);
    }

    /* calculate new number of channels */
    int channels = 0;
    switch (png_get_color_type(png_ptr, info_ptr)) {
    case PNG_COLOR_TYPE_GRAY:
    case PNG_COLOR_TYPE_PALETTE:
      channels = 1;
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:
      channels = 2;
      break;
    case PNG_COLOR_TYPE_RGB:
      channels = 3;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:
      channels = 4;
      break;
    default:
      strcpy(info.szLastError, "unknown PNG color type");
      longjmp(png_jmpbuf(png_ptr), 1);
    }

    // find the right pixel depth used for cximage
    int pixel_depth = png_get_bit_depth(png_ptr, info_ptr) *
                      png_get_channels(png_ptr, info_ptr);
    if (channels == 1 && pixel_depth > 8)
      pixel_depth = 8;
    if (channels == 2)
      pixel_depth = 8;
    if (channels >= 3)
      pixel_depth = 24;

    if (!Create(png_get_image_width(png_ptr, info_ptr),
                png_get_image_height(png_ptr, info_ptr), pixel_depth,
                CXIMAGE_FORMAT_PNG)) {
      longjmp(png_jmpbuf(png_ptr), 1);
    }

    /* get metrics */
    png_uint_32 res_x, res_y;
    int unit_type;
    if (png_get_pHYs(png_ptr, info_ptr, &res_x, &res_y, &unit_type)) {
      switch (unit_type) {
      case PNG_RESOLUTION_UNKNOWN:
        SetXDPI(res_x);
        SetYDPI(res_y);
        break;
      case PNG_RESOLUTION_METER:
        SetXDPI((long)floor(res_x * 254.0 / 10000.0 + 0.5));
        SetYDPI((long)floor(res_y * 254.0 / 10000.0 + 0.5));
        break;
      }
    }

    png_colorp palette;
    int num_palette;
    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);

    if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
      SetPalette((rgb_color *)palette, num_palette);
      SetClrImportant(num_palette);
    } else if (bit_depth == 2) { //<DP> needed for 2 bpp grayscale PNGs
      SetPaletteColor(0, 0, 0, 0);
      SetPaletteColor(1, 85, 85, 85);
      SetPaletteColor(2, 170, 170, 170);
      SetPaletteColor(3, 255, 255, 255);
    } else
      SetGrayPalette(); //<DP> needed for grayscale PNGs

    int nshift = max(0, (bit_depth >> 3) - 1) << 3;

    png_bytep trans_alpha;
    int num_trans;
    png_color_16p trans_color;

    if (png_get_tRNS(png_ptr, info_ptr, &trans_alpha, &num_trans,
                     &trans_color)) { // palette transparency
      if (num_trans == 1) {
        if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_PALETTE) {
          info.nBkgndIndex = trans_color->index;
        } else {
          info.nBkgndIndex = trans_color->gray >> nshift;
        }
      }
      if (num_trans > 1) {
        RGBQUAD *pal = GetPalette();
        if (pal) {
          DWORD ip;
          for (ip = 0; ip < min(head.biClrUsed, (unsigned long)num_trans); ip++)
            pal[ip].rgbReserved = trans_alpha[ip];
          for (ip = num_trans; ip < head.biClrUsed; ip++) {
            pal[ip].rgbReserved = 255;
          }
          info.bAlphaPaletteEnabled = true;
        }
      }
    }

    if (channels == 3) { // check RGB binary transparency
      png_bytep trans;
      int num_trans;
      png_color_16 *image_background;
      if (png_get_tRNS(png_ptr, info_ptr, &trans, &num_trans,
                       &image_background)) {
        info.nBkgndColor.rgbRed = (BYTE)(image_background->red >> nshift);
        info.nBkgndColor.rgbGreen = (BYTE)(image_background->green >> nshift);
        info.nBkgndColor.rgbBlue = (BYTE)(image_background->blue >> nshift);
        info.nBkgndColor.rgbReserved = 0;
        info.nBkgndIndex = 0;
      }
    }

    int alpha_present = (channels - 1) % 2;
    if (alpha_present) {
#if CXIMAGE_SUPPORT_ALPHA // <vho>
      AlphaCreate();
#else
      png_set_strip_alpha(png_ptr);
#endif // CXIMAGE_SUPPORT_ALPHA
    }

    // <vho> - flip the RGB pixels to BGR (or RGBA to BGRA)
    if (png_get_color_type(png_ptr, info_ptr) & PNG_COLOR_MASK_COLOR) {
      png_set_bgr(png_ptr);
    }

    // <vho> - handle cancel
    if (info.nEscape)
      png_longjmp(png_ptr, 1);

    // row_bytes is the width x number of channels x (bit-depth / 8)
    row_pointers = new BYTE[png_get_rowbytes(png_ptr, info_ptr) + 8];

    // turn on interlace handling
    int number_passes = png_set_interlace_handling(png_ptr);

    if (number_passes > 1) {
      SetCodecOption(1);
    } else {
      SetCodecOption(0);
    }

    int chan_offset = png_get_bit_depth(png_ptr, info_ptr) >> 3;
    int pixel_offset = (png_get_bit_depth(png_ptr, info_ptr) *
                        png_get_channels(png_ptr, info_ptr)) >>
                       3;

    for (int pass = 0; pass < number_passes; pass++) {
      iter.Upset();
      int y = 0;
      do {

        // <vho> - handle cancel
        if (info.nEscape)
          png_longjmp(png_ptr, 1);

#if CXIMAGE_SUPPORT_ALPHA // <vho>
        if (AlphaIsValid()) {

          // compute the correct position of the line
          long ax, ay;
          ay = head.biHeight - 1 - y;
          BYTE *prow = iter.GetRow(ay);

          // recover data from previous scan
          if (png_get_interlace_type(png_ptr, info_ptr) && pass > 0 &&
              pass != 7) {
            for (ax = 0; ax < head.biWidth; ax++) {
              long px = ax * pixel_offset;
              if (channels == 2) {
                row_pointers[px] = prow[ax];
                row_pointers[px + chan_offset] = AlphaGet(ax, ay);
              } else {
                long qx = ax * 3;
                row_pointers[px] = prow[qx];
                row_pointers[px + chan_offset] = prow[qx + 1];
                row_pointers[px + chan_offset * 2] = prow[qx + 2];
                row_pointers[px + chan_offset * 3] = AlphaGet(ax, ay);
              }
            }
          }

          // read next row
          png_read_row(png_ptr, row_pointers, NULL);

          // RGBA -> RGB + A
          for (ax = 0; ax < head.biWidth; ax++) {
            long px = ax * pixel_offset;
            if (channels == 2) {
              prow[ax] = row_pointers[px];
              AlphaSet(ax, ay, row_pointers[px + chan_offset]);
            } else {
              long qx = ax * 3;
              prow[qx] = row_pointers[px];
              prow[qx + 1] = row_pointers[px + chan_offset];
              prow[qx + 2] = row_pointers[px + chan_offset * 2];
              AlphaSet(ax, ay, row_pointers[px + chan_offset * 3]);
            }
          }
        } else
#endif // CXIMAGE_SUPPORT_ALPHA		// vho
        {
          // recover data from previous scan
          if (png_get_interlace_type(png_ptr, info_ptr) && pass > 0) {
            iter.GetRow(row_pointers, png_get_rowbytes(png_ptr, info_ptr));
            // re-expand buffer for images with bit depth > 8
            if (png_get_bit_depth(png_ptr, info_ptr) > 8) {
              for (long ax = (head.biWidth * channels - 1); ax >= 0; ax--)
                row_pointers[ax * chan_offset] = row_pointers[ax];
            }
          }

          // read next row
          png_read_row(png_ptr, row_pointers, NULL);

          // shrink 16 bit depth images down to 8 bits
          if (png_get_bit_depth(png_ptr, info_ptr) > 8) {
            for (long ax = 0; ax < (head.biWidth * channels); ax++)
              row_pointers[ax] = row_pointers[ax * chan_offset];
          }

          // copy the pixels
          iter.SetRow(row_pointers, png_get_rowbytes(png_ptr, info_ptr));
          //<DP> expand 2 bpp images only in the last pass
          if (png_get_bit_depth(png_ptr, info_ptr) == 2 &&
              pass == (number_passes - 1))
            expand2to4bpp(iter.GetRow());

          // go on
          iter.PrevRow();
        }

        y++;
      } while (y < head.biHeight);
    }

    delete[] row_pointers;

    /* read the rest of the file, getting any additional chunks in info_ptr */
    png_read_end(png_ptr, info_ptr);

    /* clean up after the read, and free any memory allocated - REQUIRED */
    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
  }
  cx_catch {
    if (strcmp(message, ""))
      strncpy(info.szLastError, message, 255);
    if (info.nEscape == -1 && info.dwType == CXIMAGE_FORMAT_PNG)
      return true;
    return false;
  }
  /* that's it */
  return true;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_DECODE
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
bool CxImagePNG::Encode(CxFile *hFile) {
  if (EncodeSafeCheck(hFile))
    return false;

  CImageIterator iter(this);
  BYTE trans[256]; // for transparency (don't move)
  png_struct *png_ptr;
  png_info *info_ptr;

  cx_try {
    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (void *)NULL, NULL,
                                      NULL);
    if (png_ptr == NULL)
      cx_throw("Failed to create PNG structure");

    /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
      png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
      cx_throw("Failed to initialize PNG info structure");
    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
     * error hadnling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
      /* If we get here, we had a problem reading the file */
      if (png_get_PLTE(png_ptr, info_ptr, NULL,
                       NULL)) // free palette if allocated by libpng (unlikely
                              // here) but good practice? No, we allocated it
                              // ourselves potentially? No, we used set_PLTE.
      { /* do nothing, libpng handles info_ptr destruction */
      }
      png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
      cx_throw("Error saving PNG file");
    }

    /* set up the output control */
    // png_init_io(png_ptr, hFile);

    // use custom I/O functions
    png_set_write_fn(png_ptr, hFile, /*(png_rw_ptr)*/ user_write_data,
                     /*(png_flush_ptr)*/ user_flush_data);

    /* set the file information here */
    png_uint_32 width = GetWidth();
    png_uint_32 height = GetHeight();
    int pixel_depth = (BYTE)GetBpp();
    int channels = (GetBpp() > 8) ? (BYTE)3 : (BYTE)1;
    int bit_depth = (BYTE)(GetBpp() / channels);
    int interlace_type;
    int compression_type = 0;
    int filter_type = 0;

    switch (GetCodecOption(CXIMAGE_FORMAT_PNG)) {
    case 1:
      interlace_type = PNG_INTERLACE_ADAM7;
      break;
    default:
      interlace_type = PNG_INTERLACE_NONE;
    }

    /* set compression level */
    // png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    bool bGrayScale = IsGrayScale();
    int color_type;

    if (GetNumColors()) {
      if (bGrayScale) {
        color_type = PNG_COLOR_TYPE_GRAY;
      } else {
        color_type = PNG_COLOR_TYPE_PALETTE;
      }
    } else {
      color_type = PNG_COLOR_TYPE_RGB;
    }
#if CXIMAGE_SUPPORT_ALPHA
    if (AlphaIsValid()) {
      color_type |= PNG_COLOR_MASK_ALPHA;
      channels++;
      bit_depth = 8;
      pixel_depth += 8;
    }
#endif

    /* set background */
    png_color_16 image_background = {0, 255, 255, 255, 0};
    RGBQUAD tc = GetTransColor();
    if (info.nBkgndIndex >= 0) {
      image_background.blue = tc.rgbBlue;
      image_background.green = tc.rgbGreen;
      image_background.red = tc.rgbRed;
    }
    png_set_bKGD(png_ptr, info_ptr, &image_background);

    /* set metrics */
    png_set_pHYs(png_ptr, info_ptr, head.biXPelsPerMeter, head.biYPelsPerMeter,
                 PNG_RESOLUTION_METER);

    png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
                 interlace_type, PNG_COMPRESSION_TYPE_BASE,
                 PNG_FILTER_TYPE_BASE);

    //<DP> simple transparency
    if (info.nBkgndIndex >= 0) {
      png_byte trans_alpha =
          255; // Placeholder, not used for index transparency?
      // For index transparency, trans is array of bytes.
      // But here we have one index.
      // libpng expects an array for tRNS in palette mode.
      // Wait, original code: info_ptr->trans_alpha = trans; (trans is BYTE[256]
      // local var). info_ptr->trans_color.index = ... If we use png_set_tRNS,
      // we pass the array.

      // Let's reconstruct.
      trans[info.nBkgndIndex] = 0; // The transparency index has alpha 0?
      // Old code did NOT set trans array for nBkgndIndex >= 0 explicitly here?
      // info_ptr->trans_alpha = trans;
      // info_ptr->num_trans = 1;
      // But trans array was uninitialized?!
      // BYTE trans[256]; declared at top.
      // If num_trans is 1, libpng uses trans_alpha[0]? No.
      // For PNG_COLOR_TYPE_PALETTE, tRNS array corresponds to palette indices.
      // If num_trans is 1, the first entry in palette is transparent?
      // No, "The tRNS chunk contains a series of one-byte alpha values,
      // corresponding to entries in the PLTE chunk."

      // The old code:
      // info_ptr->trans_color.index = (BYTE)info.nBkgndIndex;
      // This is for non-palette? Or simple transparency?
      // If PNG_COLOR_TYPE_PALETTE: trans_alpha is relevant.
      // If PNG_COLOR_TYPE_RGB: trans_color is relevant.

      // Let's look at logic:
      // if (info.nBkgndIndex >= 0) -> this handles simple transparency index.
      // But wait! Old code SwapIndex(0, info.nBkgndIndex) if !gray && used &&
      // index>0.

      png_color_16 translucent_color;
      translucent_color.index = (BYTE)info.nBkgndIndex;
      translucent_color.red = tc.rgbRed;
      translucent_color.green = tc.rgbGreen;
      translucent_color.blue = tc.rgbBlue;
      translucent_color.gray = translucent_color.index;

      // Ideally we need to know if we are in Palette mode or not.
      // color_type variable is available.

      if (color_type & PNG_COLOR_MASK_PALETTE) {
        // Managing transparency in palette is complex if we swap indices.
        // But CXImage swaps 0 and BkgndIndex at the end of block?
        // This block sets up tRNS.
        // Old code just set fields.

        // Assuming old code logic:
        // info_ptr->trans_alpha = trans; (trans is uninit buffer?)
        // Likely CXImage logic expects trans to be filled later?
        // Or maybe trans is NOT used if num_trans=1?

        // In 1.6, png_set_tRNS(png_ptr, info_ptr, trans_alpha, num_trans,
        // trans_color)

        // NOTE: The swap logic is at the end:
        // if (!bGrayScale && head.biClrUsed && info.nBkgndIndex)
        // SwapIndex(0,(BYTE)info.nBkgndIndex);

        // If we are strictly replicating logic:
        // We might need to defer tRNS setting?
        // No, we can set it.

        // Actually, if num_trans=1, and we are in palette mode, it usually
        // implies we want one color to be transparent. But libpng tRNS for
        // palette is an array.
      }

      // To be safe, let's use what the old code did via accessors.
      // Old code set info_ptr->trans_color values.
      // And trans_alpha = trans.
      // num_trans = 1.

      // If we are RGB, tRNS uses trans_color.
      // If we are Palette, tRNS uses trans_alpha array.

      // The code sets BOTH trans_color AND trans_alpha=trans.
      // This implies it might rely on both depending on color type.

      // Let's create a temp buffer for trans alpha if we need to pass it.
      // trans was declared at top.

      png_set_tRNS(png_ptr, info_ptr, trans, 1, &translucent_color);

      if (!bGrayScale && head.biClrUsed && info.nBkgndIndex)
        SwapIndex(0, (BYTE)info.nBkgndIndex);
    }

    /* set the palette if there is one */
    if (GetPalette()) {
      int nc = GetClrImportant();
      if (nc == 0)
        nc = GetNumColors();

      if (info.bAlphaPaletteEnabled) {
        for (WORD ip = 0; ip < nc; ip++)
          trans[ip] = GetPaletteColor((BYTE)ip).rgbReserved;
        // info_ptr->num_trans = (WORD)nc;
        // info_ptr->valid |= PNG_INFO_tRNS;
        // info_ptr->trans_alpha = trans;

        // We reuse trans buffer.
        // And we call png_set_tRNS.
        // But wait, we might have called png_set_tRNS above!
        // If info.nBkgndIndex >= 0 AND bAlphaPaletteEnabled?
        // Usually mutually exclusive or one overwrites?
        // Old code:
        // if (info.nBkgndIndex >= 0) { ... set num_trans=1 ... }
        // if (GetPalette()) {
        //    if (info.bAlphaPaletteEnabled) { ... set num_trans=nc ... }
        // }
        // So the latter overwrites the former if both true.
        // We should follow suit.

        png_set_tRNS(png_ptr, info_ptr, trans, nc, NULL);
      }

      // copy the palette colors
      // info_ptr->palette = new png_color[nc];
      // copy...
      // png_write_info uses this.

      // We can allocate a temporary palette array to pass to png_set_PLTE.
      png_color *tmp_palette = new png_color[nc];
      for (int i = 0; i < nc; i++) {
        RGBQUAD c = GetPaletteColor(i);
        tmp_palette[i].red = c.rgbRed;
        tmp_palette[i].green = c.rgbGreen;
        tmp_palette[i].blue = c.rgbBlue;
      }

      png_set_PLTE(png_ptr, info_ptr, tmp_palette, nc);
      delete[] tmp_palette;
    }

#if CXIMAGE_SUPPORT_ALPHA // <vho>
    // Merge the transparent color with the alpha channel
    if (AlphaIsValid() && head.biBitCount == 24 && info.nBkgndIndex >= 0) {
      for (long y = 0; y < head.biHeight; y++) {
        for (long x = 0; x < head.biWidth; x++) {
          RGBQUAD c = GetPixelColor(x, y, false);
          if (*(long *)&c == *(long *)&tc)
            AlphaSet(x, y, 0);
        }
      }
    }
#endif // CXIMAGE_SUPPORT_ALPHA	// <vho>

    int row_size =
        max(info.dwEffWidth, png_get_image_width(png_ptr, info_ptr) *
                                 png_get_channels(png_ptr, info_ptr) *
                                 (png_get_bit_depth(png_ptr, info_ptr) / 8));
    // info_ptr->rowbytes = row_size; // Not needed for writing
    BYTE *row_pointers = new BYTE[row_size];

    /* write the file information */
    png_write_info(png_ptr, info_ptr);

    // interlace handling
    int num_pass = png_set_interlace_handling(png_ptr);
    for (int pass = 0; pass < num_pass; pass++) {
      // write image
      iter.Upset();
      long ay = head.biHeight - 1;
      RGBQUAD c;
      do {
#if CXIMAGE_SUPPORT_ALPHA // <vho>
        if (AlphaIsValid()) {
          for (long ax = head.biWidth - 1; ax >= 0; ax--) {
            c = BlindGetPixelColor(ax, ay);
            int px = ax * png_get_channels(png_ptr, info_ptr);
            if (!bGrayScale) {
              row_pointers[px++] = c.rgbRed;
              row_pointers[px++] = c.rgbGreen;
            }
            row_pointers[px++] = c.rgbBlue;
            row_pointers[px] = AlphaGet(ax, ay);
          }
          png_write_row(png_ptr, row_pointers);
          ay--;
        } else
#endif // CXIMAGE_SUPPORT_ALPHA	// <vho>
        {
          iter.GetRow(row_pointers, row_size);
          if (png_get_color_type(png_ptr, info_ptr) ==
              PNG_COLOR_TYPE_RGB) // HACK BY OP
            RGBtoBGR(row_pointers, row_size);
          png_write_row(png_ptr, row_pointers);
        }
      } while (iter.PrevRow());
    }

    delete[] row_pointers;

    // if necessary, restore the original palette
    if (!bGrayScale && head.biClrUsed && info.nBkgndIndex > 0)
      SwapIndex((BYTE)info.nBkgndIndex, 0);

    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);

    /* if you malloced the palette, free it here */
    /* if you malloced the palette, free it here */
    // if (info_ptr->palette) {
    //   delete[] (info_ptr->palette);
    //   info_ptr->palette = NULL;
    // }

    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
  }
  cx_catch {
    if (strcmp(message, ""))
      strncpy(info.szLastError, message, 255);
    return FALSE;
  }
  /* that's it */
  return TRUE;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_PNG
