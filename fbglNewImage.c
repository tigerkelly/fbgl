
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "strutils.h"

// Style is fbglWidgetStyle.txt

FbglWidget *fbglNewImage(FBGL *fbgl, char *name, char *imgFile, int x, int y, char *style) {
	if (fbgl == NULL) {
		return NULL;
	}

	FbglWidget *gw = (FbglWidget *)calloc(1, sizeof(FbglWidget));

	gw->wType = WIDGET_IMAGE;
	if (name != NULL)
		gw->name = strdup(name);

	if (style != NULL)
		gw->style = strdup(style);
	else
		gw->style = strdup("bc=white,bw=black");

	gw->x1 = x;
	gw->y1 = y;
	gw->imgFile = strdup(imgFile);
	gw->allocated = true;

	char *args[64];
	char *s = strdup(style);

	int n = parse(s, ",", args, 64);

	for (int i = 0; i < n; i++) {
		// printf("args[%d] = %s\n", i, args[i]);
		trim(args[i]);
		if (strncasecmp(args[i], "bc=", 3) == 0) {
			gw->bc = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bw=", 3) == 0) {
			gw->borderWidth = atoi(&args[i][3]);
		} else {
			printf("Unknown style '%s'.\n", args[i]);
		}
	}
	free(s);

	if (gw->bc <= 0)
		gw->bc = fbgl->bc;
	if (gw->borderWidth <= 0)
		gw->borderWidth = fbgl->borderWidth;
	
	// printf("%s, fs=%d, fg=%04x, bg=%04x\n", gw->fontName, gw->fontSize, gw->fg, gw->bg);
	
	return gw;
}
