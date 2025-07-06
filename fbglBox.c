
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "strutils.h"
#include "fbglDraw.h"
#include "fbglBasic.h"

/**
 * @brief Create a new Box widget.
 *
 * @param fbgl, FBGL pointer from the fbglINit() function.
 * @param name, unique name of box.
 * @param x, X position of box, upper left corner.
 * @param y, Y position of box, upper left corner.
 * @param w, Width of box in pixels.
 * @param h, Height of box in pixels.
 * @param style, A comma seperated list of styles for box.
 * @return FbglWidget pointer on success else NULL.
 */

FbglWidget *fbglNewBox(FBGL *fbgl, char *name, int x, int y, int w, int h, char *style) {
	if (fbgl == NULL) {
		return NULL;
	}

	FbglWidget *gw = (FbglWidget *)calloc(1, sizeof(FbglWidget));

	gw->wType = WIDGET_BOX;
	if (name != NULL)
		gw->name = strdup(name);
	if (style != NULL)
		gw->style = strdup(style);
	else
		gw->style = strdup("fg=white,bg=black");
	gw->x1 = x;
	gw->y1 = y;
	gw->w = w;
	gw->h = h;
	gw->allocated = true;

	char *args[64];
	char *s = strdup(style);

	int n = parse(s, ",", args, 64);

	for (int i = 0; i < n; i++) {
		// printf("args[%d] = %s\n", i, args[i]);
		trim(args[i]);
		if (strncasecmp(args[i], "fg=", 3) == 0) {
			gw->fg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bg=", 3) == 0) {
			gw->bg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "fc=", 3) == 0) {
			gw->fc = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bc=", 3) == 0) {
			gw->bc = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "filled=", 7) == 0) {
			if (strcasecmp(&args[i][7], "true") == 0 || strcasecmp(&args[i][7], "yes") == 0)
				gw->filled = true;
			else
				gw->filled = false;
		} else {
			printf("Unknown style '%s',\n", args[i]);
		}
	}
	free(s);

	if (gw->fg <= 0)
		gw->fg = fbgl->dfg;
	if (gw->bg <= 0)
		gw->bg = fbgl->dbg;
	if (gw->fc <= 0)
		gw->fc = fbgl->fc;
	if (gw->bc <= 0)
		gw->bc = fbgl->bc;
	if (gw->borderWidth <= 0)
		gw->borderWidth = fbgl->borderWidth;
	
	return gw;
}

// Called by the fbglDrawPanel function.

int fbglDrawBox(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
		return -1;
	}

	if (gw == NULL) {
		return -2;
	}

	if (gw->filled == false) {
		_fbglBox(fbgl, gw->x1, gw->y1, gw->w, gw->h, gw->fg, gw->bg);
	} else {
		int b = gw->borderWidth;
		_fbglBox(fbgl, gw->x1, gw->y1, gw->w, gw->h, gw->fg, gw->bg);
		_fbglFilledBox(fbgl, gw->x1 + b, gw->y1 + b, gw->w - (b * 2), gw->h - (b * 2), gw->fc, gw->bg);
	}

	return 0;
}
