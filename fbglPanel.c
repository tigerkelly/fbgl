
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "fbglBasic.h"
#include "strutils.h"

int fbglDrawPanel(FBGL *fbgl, FbglPanel *panel) {
	if (fbgl == NULL) {
		return -1;
	}

	if (panel == NULL) {
		return -2;
	}

	fbglClear(fbgl);

	_fbglBox(fbgl, panel->x, panel->y, panel->w, panel->h, panel->fg, panel->bg);

	for (int i = 0; i < MAX_WIDGET_COUNT; i++) {
		if (panel->gw[i] != NULL) {
			// fbglLogIt("Widget: %s %04x %04x\n", panel->gw[i]->name, panel->gw[i]->fg, panel->gw[i]->bg);

			FbglWidget *gw = panel->gw[i];

			// printf("Type %d\n", gw->wType);

			switch (gw->wType) {
				case WIDGET_LABEL:
					fbglDrawLabel(fbgl, gw);
					break;
				case WIDGET_LINE:
					fbglDrawLine(fbgl, gw);
					break;
				case WIDGET_CIRCLE:
					fbglDrawCircle(fbgl, gw);
					break;
				case WIDGET_BOX:
					fbglDrawBox(fbgl, gw);
					break;
				case WIDGET_IMAGE:
					fbglDrawImage(fbgl, gw);
					break;
				case WIDGET_UNKNOWN:
					break;
			}
		}
	}


	return 0;
}

/**
 * @brief Creates a new Panel structure.
 *
 * This funciton is used to create a panel structure to place widgets into every widget
 * should be plasced into a panel.
 *
 * @param fbgl, The FBGL pointer returned by the fbglInit("/dev/fb0") function.
 * @param panelName, A unique user name for panel.
 * @param x, The X postion of panel, if <= 0 then use zero.
 * @param y, The Y postion of panel, if <= 0 then use zero.
 * @param w, The width of panel in pixels, if <= 0 then use screenSizeWidth - 1.
 * @param h, The height of panel in pixels, if <= 0 then use screenSizeHeight - 1.
 * @param style, A comma seperated list of styles for panel.
 *		fg = foreground color
 *		bg = background color
 *		bc = border color
 * @return Returns pointer to panel structure on success else NULL.
 */

FbglPanel *fbglNewPanel(FBGL *fbgl, char *panelName, int x, int y, int w, int h, char *style) {	
	FbglPanel *panel = NULL;

	panel = (FbglPanel *)calloc(1, sizeof(FbglPanel));
	panel->panelName = strdup(panelName);

	char *args[32];
	char *s = strdup(style);
	int n = parse(s, ",", args, 32);

	for (int i = 0; i < n; i++) {
		trim(args[i]);

		if (strncasecmp(args[i], "fg=", 3) == 0) {
			panel->fg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bg=", 3) == 0) {
			panel->bg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bc=", 3) == 0) {
			panel->bc = fbglGetColor(fbgl, &args[i][3]);
		}
	}

	free(s);
	if (x < 0)
		panel->x = 0;
	else
		panel->x = x;
	if (y < 0)
		panel->y = 0;
	else
		panel->y = y;
	
	if (w <= 0)
		panel->w = fbgl->vinfo.xres - 1;
	else
		panel->w = w;
	
	if (h <= 0)
		panel->h = fbgl->vinfo.yres - 1;
	else
		panel->h = h;
	
	if (panel->fg <= 0)
		panel->fg = fbglGetColor(fbgl, "white");
	
	if (panel->bg <= 0)
		panel->bg = fbglGetColor(fbgl, "black");
	
	if (panel->bc <= 0)
		panel->bc = fbglGetColor(fbgl, "blue");

	// printf("Panel %ld\n", sizeof(FbglPanel));

	return panel;
}

int fbglWidgetAdd(FbglPanel *panel, void *widget) {

	FbglWidget *gw = (FbglWidget *)widget;

	bool found = false;
	for (int i = 0; i < MAX_WIDGET_COUNT; i++) {
		if (panel->gw[i] == NULL) {
			panel->gw[i] = gw;
			found = true;
			break;
		}
	}

	if (found == false) {
		printf("Out of Widget slots.\n");
		return -1;
	}

	return 0;
}
