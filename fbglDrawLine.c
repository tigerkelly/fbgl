
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"

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

		for (int b = 0; b < fbgl->borderWidth; b++) {
			for (int i = 0; i < len; i++) {
				fbglPixel(fbgl, x1 + i, y1 + b, gw->fg, 0);
			}
		}
	} else {
		int len = gw->y2 - y1;

		for (int b = 0; b < fbgl->borderWidth; b++) {
			for (int i = 0; i < len; i++) {
				fbglPixel(fbgl, x1 + b, y1 + i, gw->fg, 0);
			}
		}
	}

	return 0;
}
