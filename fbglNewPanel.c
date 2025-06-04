
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "fbgl.h"
#include "strutils.h"
#include "miscutils.h"

FbglPanel *fbglNewPanel(FBGL *fbgl, char *panelName, int x, int y, int w, int h, char *style) {	
	FbglPanel *panel = NULL;

	panel = (FbglPanel *)calloc(1, sizeof(FbglPanel));
	panel->panelName = strdup(panelName);

	char *args[32];
	char *s = strdup(style);
	int n = parse(s, ",", args, 32);

	for (int i = 0; i < n; i++) {
		trim(args[i]);

		if (strncasecmp(args[i], "fc=", 3) == 0) {
			panel->fc = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "fg=", 3) == 0) {
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
