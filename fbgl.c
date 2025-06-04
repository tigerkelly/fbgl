
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>

#include "ini.h"
#include "strutils.h"
#include "fbgl.h"

#define _POSIX_C_SOURCE	200809L

int fbglLoadFontList(FBGL *fbgl) {

	if (fbgl == NULL) {
		return -1;
	}

	char fontFile[MAX_LINE_LENGTH + 1];

	char *v = getenv("FBGL_PATH");

	if (v == NULL) {
		printf("env FBGL_PATH not set.\n");
		return -2;
	}

	sprintf(fontFile, "%s/%s", v, FONT_DB_NAME);

	int fontCnt = 0;

	FILE *f = fopen(fontFile, "r");
	if (f != NULL) {

		char line[MAX_LINE_LENGTH];
		while (fgets(line, MAX_LINE_LENGTH, f) != NULL) {
			trim(line);
			if (line[0] == '#')
				continue;
			if (strlen(line) <= 0)
				continue;
			if (fontCnt < MAX_FONT_NUMBER) {
				fbgl->dbFont[fontCnt++] = strdup(line);
			}
		}

		fbgl->dbFontCnt = fontCnt;

		fclose(f);
	} else {
		printf("Font file '%s' does not exist.\n", fontFile);
		return -1;
	}

#if(0)
	for (int i = 0; i < MAX_FONT_NUMBER; i++) {
		if (fbgl->dbFont[i] != NULL) {
			printf("Font: %s\n", fbgl->dbFont[i]);
		}
	}
#endif

	return 0;
}

int fbglLoadColorList(FBGL *fbgl) {

	if (fbgl == NULL) {
		return -1;
	}

	char colorFile[MAX_LINE_LENGTH + 1];

	char *v = getenv("FBGL_PATH");

	if (v == NULL) {
		printf("env FBGLIGER_PATH not set.\n");
		return -2;
	}

	sprintf(colorFile, "%s/%s", v, COLOR_DB_NAME);

	int colorCnt = 0;

	FILE *f = fopen(colorFile, "r");
	if (f != NULL) {
		char *args[8];

		char line[MAX_LINE_LENGTH];
		while (fgets(line, MAX_LINE_LENGTH, f) != NULL) {
			trim(line);
			if (line[0] == '#')
				continue;
			if (strlen(line) <= 0)
				continue;

			int n = parse(line, " \t", args, 8);

			if (n != 4)
				continue;

			if (colorCnt < MAX_COLOR_NUMBER) {
				Color *c = (Color *)calloc(1, sizeof(Color));
				strcpy(c->name, args[0]);
				c->color = (FbglColor)strtol(args[2], NULL, 16);
				c->used = true;
				fbgl->dbColor[colorCnt++] = c;
			}
		}

		fbgl->dbColorCnt = colorCnt;

		fclose(f);
	} else {
		printf("Color file '%s' does not exist.\n", colorFile);
		return -1;
	}

#if(0)
	for (int i = 0; i < MAX_COLOR_NUMBER; i++) {
		if (fbgl->dbColor[i] != NULL) {
			printf("Color: %s=%04x\n", fbgl->dbColor[i]->name, fbgl->dbColor[i]->color);
		}
	}
#endif

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

	fbglLoadColorList(fbgl);

	fbgl->borderWidth = 1;
	fbgl->dfn = DEFAULT_FONTNAME;
	fbgl->dfs = DEFAULT_FONTSIZE;
	fbgl->dfg = fbglGetColor(fbgl, "white");
	fbgl->dbg = fbglGetColor(fbgl, "black");
	fbgl->bc = fbglGetColor(fbgl, "blue");
	fbgl->fc = fbglGetColor(fbgl, "cornsilk");

	fbglLoadFontList(fbgl);

	fbglClear(fbgl);
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

int fbglPixel(FBGL *fbgl, int x, int y, FbglColor fg) {

	if (fbgl == NULL) {
		return -1;
	}

	if (x >= 0 && x < fbgl->vinfo.xres && y >= 0 && y < fbgl->vinfo.xres) {
		// Set the pixel color (16-bit RGB565)
		((FbglColor *)fbgl->screen)[y * (fbgl->finfo.line_length / 2) + x] = fg;
	} else {
		return -1;
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

	for (int i = 0; i < fbgl->dbColorCnt; i++) {
		if (strcasecmp(fbgl->dbColor[i]->name, colorName) == 0) {
			return fbgl->dbColor[i]->color;
			break;
		}
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

	for (int i = 0; i < fbgl->dbFontCnt; i++) {
		if (strstr(fbgl->dbFont[i], fontName) != NULL)
			return fbgl->dbFont[i];
	}

	return NULL;
}

int fbglLoadFont(FBGL *fbgl, FbglFont *fbglFont, FT_Face face) {

	char *ff = fbglFindFont(fbgl, fbglFont->fontName);
	if (ff == NULL) {
		// font not found.
		return -3;
	}

	if (face == NULL) {
		return -4;
	}

	FT_Error err = FT_New_Face(fbgl->library, ff, 0, &face);
	if ( err == FT_Err_Unknown_File_Format ) {
		return -1;
	} else if ( err ) {
		return -2;
	}

	FT_Set_Pixel_Sizes(face, 0, fbglFont->fontSize);

	return 0;
}

FbglFont *fbglNewFbglFont(char *fontName, int fontSize) {
	static FbglFont font;

	strcpy(font.fontName, fontName);
	font.fontSize = fontSize;

	return &font;
}

FbglColor fbglGetColor(FBGL *fbgl, char *name) {
    unsigned short color = 0;

	bool flag = false;
	for (int i = 0; i < MAX_COLOR_NUMBER; i++) {
		if (fbgl->dbColor[i] != NULL) {
			if (fbgl->dbColor[i]->used == true) {
				if(strcasecmp(name, fbgl->dbColor[i]->name) == 0) {
					color = fbgl->dbColor[i]->color;
					flag = true;
					break;
				}
			}
		}
	}

	if (flag == false) {
		printf("Color '%s' not found.\n", name);
	}

    return color;
}
