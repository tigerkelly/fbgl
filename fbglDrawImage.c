
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <png.h>
#include <sys/stat.h>
#include <sys/stat.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "fbglBasic.h"
#include "strutils.h"

int readPngImage(FBGL *fbgl, FbglWidget *gw);

int fbglDrawImage(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
		return -1;
	}

	if (gw == NULL) {
		return -2;
	}

	for (int b = 0; b < gw->borderWidth; b++) {
		// _fbglImage(fbgl, gw->x1, gw->y1, gw->radius - b, gw->bc, gw->bg);
	}

	readPngImage(fbgl, gw);


	return 0;
}

int readPngImage(FBGL *fbgl, FbglWidget *gw){
	
	// 1. Open PNG file and initialize libpng
	FILE *fp = fopen(gw->imgFile, "rb");
	if (!fp) {
		perror("Error opening PNG file");
		return -1;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		fprintf(stderr, "Error creating PNG read struct\n");
		return -2;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(fp);
		fprintf(stderr, "Error creating PNG info struct\n");
		return -3;
	}

	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		// fprintf(stderr, "Error during PNG read\n");
		return -4;
	}

	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);

	png_uint_32 width = png_get_image_width(png_ptr, info_ptr);
	png_uint_32 height = png_get_image_height(png_ptr, info_ptr);
	png_byte color_type = png_get_color_type(png_ptr, info_ptr);
	png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	// Add transformations for different PNG types if needed (e.g., 16-bit to 8-bit)
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_palette_to_rgb(png_ptr);
	}
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
		// png_set_expand_gray_1_2_4(png_ptr);
	}
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
		png_set_tRNS_to_alpha(png_ptr);
	}
	// Convert to RGBA if not already
	if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE) {
		png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
	}
	png_read_update_info(png_ptr, info_ptr);

	// 3. Read PNG pixel data
	png_bytep *row_pointers = (png_bytep *)malloc(sizeof(png_bytep) * height);
	for (int y = 0; y < height; y++) {
		row_pointers[y] = (png_bytep)malloc(png_get_rowbytes(png_ptr, info_ptr));
	}
	png_read_image(png_ptr, row_pointers);

	// 4. Write to framebuffer (basic RGBA to 32-bit framebuffer)
	for (int y = 0; y < height && y < fbgl->vinfo.yres; y++) {
		for (int x = 0; x < width && x < fbgl->vinfo.xres; x++) {
			long int location = (x + fbgl->vinfo.xoffset) * (fbgl->vinfo.bits_per_pixel / 8) +
								gw->x1 * (fbgl->vinfo.bits_per_pixel / 8) +
								(y + fbgl->vinfo.yoffset + gw->y1) * fbgl->finfo.line_length;

			// Assuming 32-bit RGBA PNG and 32-bit framebuffer
			// Adjust this based on your framebuffer's bit_depth and color order
			unsigned char r = row_pointers[y][x * 4];
			unsigned char g = row_pointers[y][x * 4 + 1];
			unsigned char b = row_pointers[y][x * 4 + 2];
			unsigned char a = row_pointers[y][x * 4 + 3]; // Alpha channel

			// Simple direct copy for 32-bit RGBA to 32-bit framebuffer (adjust if needed)
			*(unsigned int *)(fbgl->screen + location) = (a << 24) | (r << 16) | (g << 8) | b;
		}
	}

	// Cleanup
    for (int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return 0;
}
