
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "fbglBasic.h"

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
