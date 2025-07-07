
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "strutils.h"

/*
 * The circle starts at 0.0 degrees, this is the right most side of the circle
 * and goes clockwise to 360.0.
 *
 */

/**
 * @brief Draws an arc on the screen.
 *
 * @param fbgl, Pointer to FBGL structure returned by the fbglInit() function.
 * @param gw, FbglWidget structure to draw line.
 * @return zero on success.
 */
int fbglDrawArc(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
		return -1;
	}

	if (gw == NULL) {
		return -2;
	}

	for (int b = 0; b < gw->lineWidth; b++) {
		int r = gw->radius + b;
		for (double i = gw->sa; i < gw->ea; i += 0.01) {
			double angle = (double)i * M_PI / 180.0; // Convert degrees to radians

			int x = gw->x1 + (int)(r * cos(angle));
			int y = gw->y1 + (int)(r * sin(angle));
			fbglPixel(fbgl, x, y, gw->fg, 0);
		}
	}

	return 0;
}

/**
 * @brief Create a new arc widget.
 *
 * @param fbgl, Pointer to FBGL structure returned by the fbglInit() function.
 * @param name, A unique name for the widget.
 * @param x, center X position of widget.
 * @param y, center Y position of widget.
 * @param radius, radius of arc in pixels
 * @param style, A comma seperated list of styles for line.
 * @return Zero on success
 */

FbglWidget *fbglNewArc(FBGL *fbgl, char *name, int x, int y, int radius, char *style) {
	if (fbgl == NULL) {
		return NULL;
	}

	FbglWidget *gw = (FbglWidget *)calloc(1, sizeof(FbglWidget));

	gw->wType = WIDGET_ARC;
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
	gw->radius = radius;
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
		} else if (strncasecmp(args[i], "sa=", 3) == 0) {
			gw->sa = atof(&args[i][3]);
		} else if (strncasecmp(args[i], "ea=", 3) == 0) {
			gw->ea = atof(&args[i][3]);
		}
	}
	free(s);

	if (gw->fg <= 0)
		gw->fg = fbglGetColor(fbgl, "white");
	if (gw->bg <= 0)
		gw->bg = fbglGetColor(fbgl, "black");
	if (gw->sa < 0.0)
		gw->sa = 0.0;
	if (gw->ea > 360.0)
		gw->ea = 360.0;
	if (gw->lineWidth <= 0)
		gw->lineWidth = 1;
	
	// printf("%s, lw=%d, fg=%04x, bg=%04x, sa=%.2f, ea=%.2f\n",
			// gw->name, gw->lineWidth, gw->fg, gw->bg, gw->sa, gw->ea);
	
	return gw;
}
