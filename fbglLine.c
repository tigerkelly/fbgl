
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "strutils.h"

/**
 * @brief Draws a line on the screen.
 *
 * @param fbgl, Pointer to FBGL structure returned by the fbglInit() function.
 * @param gw, FbglWidget structure to draw line.
 * @return zero on success.
 */
int fbglDrawLine(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
		return -1;
	}

	if (gw == NULL) {
		return -2;
	}

	int x1 = gw->x1;
	int y1 = gw->y1;

	if (gw->vert == false) {
		int len = gw->x2 - gw->x1;

		for (int b = 0; b < gw->lineWidth; b++) {
			for (int i = 0; i < len; i++) {
				fbglPixel(fbgl, x1 + i, y1 + b, gw->fg, 0);
			}
		}
	} else {
		int len = gw->y2 - y1;

		for (int b = 0; b < gw->lineWidth; b++) {
			for (int i = 0; i < len; i++) {
				fbglPixel(fbgl, x1 + b, y1 + i, gw->fg, 0);
			}
		}
	}

	return 0;
}

/**
 * @brief Create a new line widget.
 *
 * @param fbgl, Pointer to FBGL structure returned by the fbglInit() function.
 * @param name, A unique name for the widget.
 * @param x, starting X position of widget.
 * @param y, starting Y position of widget.
 * @param x2, ending X position of widget.
 * @param y2, ending Y position of widget.
 * @param style, A comma seperated list of styles for line.
 * @return Zero on success
 */

FbglWidget *fbglNewLine(FBGL *fbgl, char *name, int x, int y, int x2, int y2, char *style) {
	if (fbgl == NULL) {
		return NULL;
	}

	FbglWidget *gw = (FbglWidget *)calloc(1, sizeof(FbglWidget));

	gw->wType = WIDGET_LINE;
	if (name != NULL) {
		gw->name = strdup(name);
	}

	if (style != NULL) {
		gw->style = strdup(style);
	} else {
		gw->style = strdup("fg=white,bg=black,vert=false");
	}

	gw->x1 = x;
	gw->y1 = y;
	gw->x2 = x2;
	gw->y2 = y2;
	gw->allocated = true;

	char *args[64];
	char *s = strdup(gw->style);

	int n = parse(s, ",", args, 64);

	for (int i = 0; i < n; i++) {
		// printf("args[%d] = %s\n", i, args[i]);
		if (strncasecmp(args[i], "fg=", 3) == 0) {
			gw->fg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bg=", 3) == 0) {
			gw->bg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "lw=", 3) == 0) {
			gw->lineWidth = atoi(&args[i][3]);
		} else if (strncasecmp(args[i], "vert=", 5) == 0) {
			if (args[i][5] == 't' || args[i][5] == 'T' ||
					args[i][5] == 'y' || args[i][5] == 'Y') {
				gw->vert = true;
			} else {
				gw->vert = false;
			}
		}
	}
	free(s);

	if (gw->fg <= 0)
		gw->fg = fbglGetColor(fbgl, "white");
	if (gw->bg <= 0)
		gw->bg = fbglGetColor(fbgl, "black");
	if (gw->lineWidth <= 0)
		gw->lineWidth = 1;
	
	// printf("%s, fs=%d, fg=%04x, bg=%04x\n", gw->fontName, gw->fontSize, gw->fg, gw->bg);
	
	return gw;
}
