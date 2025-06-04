
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "fbglBasic.h"

int fbglDrawPanel(FBGL *fbgl, FbglPanel *panel) {
	if (fbgl == NULL) {
		return -1;
	}

	if (panel == NULL) {
		return -2;
	}

	_fbglBox(fbgl, panel->x, panel->y, panel->w, panel->h, panel->fg, panel->bg);

	for (int i = 0; i < MAX_WIDGET_COUNT; i++) {
		if (panel->gw[i] != NULL) {
			// printf("Widget: %s %04x %04x\n", panel->gw[i]->name, panel->gw[i]->fg, panel->gw[i]->bg);

			FbglWidget *gw = panel->gw[i];

			// printf("Type %d\n", gw->wType);

			switch (gw->wType) {
				case WIDGET_LABEL:
					fbglDrawText(fbgl, gw);
					break;
				case WIDGET_CIRCLE:
					fbglDrawCircle(fbgl, gw);
					break;
				case WIDGET_BOX:
					fbglDrawBox(fbgl, gw);
					break;
				case WIDGET_UNKNOWN:
					break;
			}
		}
	}


	return 0;
}

