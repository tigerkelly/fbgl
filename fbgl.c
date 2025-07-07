
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>

#include "ini.h"
#include "strutils.h"
#include "fbgl.h"

#define _POSIX_C_SOURCE	200809L

/* This is for either 16 or 32 bit color depth.
 * Use the fbset command to change bits per pixel of the console.
 *    fbset -g 1024 1600 1024 600 16
 *    or
 *    fbset -g 1024 1600 1024 600 32
 *		Current monitor is a WaveShare 7inch 1024x600 (H) display.
 * If the above does not work then you may need to disable vc4-kms-v3d in the
 *		#dtoverlay=vc4-kms-v3d
 * /boot/firmware/config.txt file or /boot/config.txt file.
 * With the Waveshare display I also had to set the following in the config.txt file.
 *      hdmi_group=2
 *      hdmi_mode=87
 *      hdmi_cvt 1024 600 60 6 0 0 0
 *      hdmi_drive=1
 * Review the displays manual for settings based on your RPI model and display model.
 */

int fbglLoadFontList(FBGL *fbgl) {

	if (fbgl == NULL) {
		return -1;
	}

	if (fbgl->dbFont != NULL) {
		free(fbgl->dbFont);
	}

	int fontCnt = 0;
	char *args[64];

	while (_fontPaths[fontCnt] != NULL) {
		fontCnt++;
	}

	fbgl->dbFont = (FbglFont *)calloc(fontCnt, sizeof(FbglFont));
	FbglFont *fp = fbgl->dbFont;

	fontCnt = 0;

	while (_fontPaths[fontCnt] != NULL) {
		char *s = strdup(_fontPaths[fontCnt]);
		int n = parse(s, "/", args, 64);
		char *p = strrchr(args[n-1], '.');
		if (p != NULL) {
			*p = '\0';
		}
		fp->fontName = strdup(args[n-1]);
		fp->fontPath = strdup(_fontPaths[fontCnt]);
		free(s);
		fontCnt++;
		fp++;
	}

	fbgl->dbFontCnt = fontCnt;

	return 0;
}

int fbglLoadColorList(FBGL *fbgl) {

	if (fbgl == NULL) {
		return -1;
	}

	if (fbgl->dbColor != NULL) {
		free(fbgl->dbColor);
	}

	int colorCnt = 0;
	while (_colors[colorCnt].colorName != NULL) {
		colorCnt++;
	}

	fbgl->dbColorCnt = colorCnt;
	fbgl->dbColor = _colors;

	return 0;
}

FBGL *fbglInit(char *dev) {

	FBGL *fbgl = (FBGL *)calloc(1, sizeof(FBGL));

	int fd;

	fd = open(dev, O_RDWR);
	if (fd != -1) {
		fbgl->dev = strdup(dev);
		fbgl->fd = fd;
		fbglMapScreen(fbgl);
	} else {
		return NULL;
	}


	FT_Error err = FT_Init_FreeType(&fbgl->library);
	if (err) {
		fprintf(stderr, "Error initializing FreeType\n");
		return NULL;
	}

	err = FT_New_Face(fbgl->library, "/usr/share/fonts/truetype/freefont/FreeMono.ttf", 0, &fbgl->defaultFace);
	if (err) {
		fprintf(stderr, "Error loading font face\n");
		return NULL;
	}

	err = FT_Set_Pixel_Sizes(fbgl->defaultFace, 0, 48); // 0 for width means 'same as height'
	if (err) {
		fprintf(stderr, "Error setting font size\n");
		return NULL;
	}

	// if (ioctl(fd, KDSETMODE, KD_GRAPHICS) < 0) {
        // perror("Failed to set graphics mode.");
        // close(fd);
        // return NULL;
    // }

	// printf("Screen width %d, height %d depth %d\n",
			// fbgl->vinfo.xres, fbgl->vinfo.yres, fbgl->vinfo.bits_per_pixel);

	fbglLoadColorList(fbgl);

	fbgl->borderWidth = 1;
	fbgl->dfn = DEFAULT_FONTNAME;
	fbgl->dfs = DEFAULT_FONTSIZE;
	fbgl->dfg = fbglGetColor(fbgl, "white");
	fbgl->dbg = fbglGetColor(fbgl, "black");
	fbgl->bc = fbglGetColor(fbgl, "blue");
	fbgl->fc = fbglGetColor(fbgl, "cornsilk");

	fbglLoadFontList(fbgl);

	// fbglClear(fbgl);
	// printf("1 Temporary directory created: '%s'\n", fbgl->tmpDir);

	return fbgl;
}

int fbglExit(FBGL *fbgl) {
	fbglRestoreScreen(fbgl);
	if (fbgl->library != NULL) {
		fbgl->stopEvent = true;
		fbgl->stopTouch = true;
		fbgl->stopMouse = true;
		fbgl->stopKeyboard = true;
		FT_Done_Face(fbgl->defaultFace);
		FT_Done_FreeType(fbgl->library);
	}

	if (fbgl->tmpDir != NULL) {
		if (remove(fbgl->tmpDir) == -1) {
			printf("Failed to delete temp directory. '%s'\n%s\n", fbgl->tmpDir, strerror(errno));
		}
		fbgl->tmpDir = NULL;
	}

	return 0;
}

int fbglSetBorderWidth(FBGL *fbgl, int borderWidth) {

	if (fbgl == NULL) {
		return -1;
	}

	if (borderWidth > 0 && borderWidth < 32)
		fbgl->borderWidth = borderWidth;

	return 0;
}

int fbglMapScreen(FBGL *fbgl) {

	size_t buflen;
	int r = 1;

	if (fbgl == NULL) {
		return -1;
	}

	if (!ioctl(fbgl->fd, FBIOGET_VSCREENINFO, &fbgl->vinfo) &&
			!ioctl(fbgl->fd, FBIOGET_FSCREENINFO, &fbgl->finfo)) {

		buflen = fbgl->vinfo.yres_virtual * fbgl->finfo.line_length;

		// printf("buflen %ld, yres_virtual %d, line_length %d\n",
				// buflen, fbgl->vinfo.yres_virtual, fbgl->finfo.line_length);

		fbgl->screen = (char *)mmap(NULL, buflen, PROT_READ|PROT_WRITE, MAP_SHARED, fbgl->fd, 0);
		fbgl->savedScreen = (char *)calloc(1, buflen);
		memcpy(fbgl->savedScreen, fbgl->screen, buflen);

		if (fbgl->screen != MAP_FAILED) {
			fbgl->screenSize = buflen;
			/*
			 * TODO: something interesting here.
			 * "screen" now points to screen pixels.
			 * Each individual pixel might be at:
			 *    screen + x * screen_info.bits_per_pixel/8
			 *           + y * fixed_info.line_length
			 * Then you can write pixels at locations such as that.
			 */

			r = 0;   /* Indicate success */
		} else {
			perror("mmap");
		}
	}

	return r;
}

int fbglRestoreScreen(FBGL *fbgl) {
	if (fbgl == NULL) {
		return -1;
	}

	memcpy(fbgl->screen, fbgl->savedScreen, fbgl->screenSize);

	return 0;
}

void convert_rgb565_to_rgb888(uint16_t rgb565_pixel, uint8_t *red_8bit, uint8_t *green_8bit, uint8_t *blue_8bit) {
    // Extract 5-bit Red component
    uint8_t red_5bit = (rgb565_pixel >> 11) & 0x1F; 
    // Extract 6-bit Green component
    uint8_t green_6bit = (rgb565_pixel >> 5) & 0x3F;
    // Extract 5-bit Blue component
    uint8_t blue_5bit = rgb565_pixel & 0x1F;

    // Scale to 8-bit values by left-shifting and replicating MSB
    *red_8bit = (red_5bit << 3) | (red_5bit >> 2); // 5-bit to 8-bit (replicate 3 MSBs)
    *green_8bit = (green_6bit << 2) | (green_6bit >> 4); // 6-bit to 8-bit (replicate 2 MSBs)
    *blue_8bit = (blue_5bit << 3) | (blue_5bit >> 2); // 5-bit to 8-bit (replicate 3 MSBs)
}

int fbglPixel(FBGL *fbgl, int x, int y, FbglColor fg, int alpha) {

	if (fbgl == NULL) {
		return -1;
	}

	if (x < 0 || x >= fbgl->vinfo.xres || y < 0 || y >= fbgl->vinfo.yres) {
        return -2; // Out of bounds
    }

	if (x >= 0 && x < fbgl->vinfo.xres && y >= 0 && y < fbgl->vinfo.xres) {
		// Set the pixel color (16-bit RGB565)
		if (fbgl->vinfo.bits_per_pixel == 16) {
			((FbglColor *)fbgl->screen)[y * (fbgl->finfo.line_length / 2) + x] = fg;
		} else if (fbgl->vinfo.bits_per_pixel == 32) {
			uint8_t r, g, b;
			convert_rgb565_to_rgb888(fg, &r, &g, &b);
			int bpp = fbgl->vinfo.bits_per_pixel / 8;
			// long location = (x + fbgl->vinfo.xoffset) * bpp + (y + fbgl->vinfo.yoffset) * fbgl->vinfo.xres * bpp;
			long location = (x + fbgl->vinfo.xoffset) * bpp + (y + fbgl->vinfo.yoffset) * fbgl->finfo.line_length;
			fbgl->screen[location] = b;		// Blue
			fbgl->screen[location + 1] = g; // Green
			fbgl->screen[location + 2] = r; // Red
			fbgl->screen[location + 3] = alpha & 0xff;      // Alpha
			// ((FbglColor *)fbgl->screen)[y * (fbgl->finfo.line_length / 4) + x] = fg;
		}
	} else {
		// return out of bounds.
		return -3;
	}

	return 0;
}

int fbglClear(FBGL *fbgl) {

	if (fbgl == NULL) {
		return -1;
	}

	memset(fbgl->screen, fbgl->dbg, fbgl->screenSize);

	return 0;
}

FbglColor fbglFindColor(FBGL *fbgl, char *colorName) {

	if (fbgl == NULL) {
		return -1;
	}

	if (fbgl->dbColorCnt <= 0) {
		printf("No Color database loaded.\n");
		return -2;
	}

	Color *cp = fbgl->dbColor;
	for (int i = 0; i < fbgl->dbColorCnt; i++) {
		if (strcasecmp(cp->colorName, colorName) == 0) {
			return cp->colorValue;
			break;
		}
		cp++;
	}

	return 0;
}

char *fbglFindFont(FBGL *fbgl, char *fontName) {

	if (fbgl == NULL) {
		return NULL;
	}

	if (fbgl->dbFontCnt <= 0) {
		printf("No font database loaded.\n");
		return NULL;
	}

	FbglFont *fp = fbgl->dbFont;
	for (int i = 0; i < fbgl->dbFontCnt; i++) {
		// printf("%s, %s\n", fbgl->dbFont[i].fontName, fontName);
		if (strcmp(fp->fontName, fontName) == 0)
			return fp->fontPath;
		fp++;
	}

	return NULL;
}

int fbglLoadFont(FBGL *fbgl, FbglWidget *gw) {

	char *ff = fbglFindFont(fbgl, gw->fontName);
	if (ff == NULL) {
		// font not found.
		return -1;
	}

	FT_Error err = FT_New_Face(fbgl->library, ff, 0, &gw->fontFace);
	if ( err == FT_Err_Unknown_File_Format ) {
		printf("Here 2.\n");
		return -2;
	} else if ( err ) {
		printf("Here 3.\n");
		return -3;
	}

	FT_Set_Pixel_Sizes(gw->fontFace, 0, gw->fontSize);

	return 0;
}

FbglColor fbglGetColor(FBGL *fbgl, char *colorName) {
    unsigned short color = 0;

	bool flag = false;
	Color *cp = fbgl->dbColor;;
	for (int i = 0; i < MAX_COLOR_NUMBER; i++) {
		if (cp != NULL) {
			if(strcasecmp(colorName, cp->colorName) == 0) {
				color = cp->colorValue;
				flag = true;
				break;
			}
		}
		cp++;
	}

	if (flag == false) {
		printf("Color '%s' not found.\n", colorName);
	}

    return color;
}
