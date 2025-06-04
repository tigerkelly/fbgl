
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "fbglBasic.h"

int fbglDrawCircle(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
		return -1;
	}

	if (gw == NULL) {
		return -2;
	}

	if (gw->filled == false) {
		for (int b = 0; b < gw->borderWidth; b++) {
			_fbglCircle(fbgl, gw->x1, gw->y1, gw->radius - b, gw->bc, gw->bg);

		}
	} else {
		for (int b = 0; b < gw->borderWidth; b++) {
			_fbglCircle(fbgl, gw->x1, gw->y1, gw->radius - b, gw->bc, gw->bg);
		}
		_fbglFilledCircle(fbgl, gw->x1, gw->y1, gw->radius - (gw->borderWidth+1), gw->fc, gw->bg);
	}

	return 0;
}
