
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "strutils.h"

// Style is fbglWidgetStyle.txt

FbglWidget *fbglNewLabel(FBGL *fbgl, char *name, char *text, int x, int y, char *style) {
	if (fbgl == NULL) {
		return NULL;
	}

	FbglWidget *gw = (FbglWidget *)calloc(1, sizeof(FbglWidget));

	gw->wType = WIDGET_LABEL;
	if (name != NULL)
		gw->name = strdup(name);
	if (text != NULL)
		gw->text = strdup(text);
	if (style != NULL)
		gw->style = strdup(style);
	else
		gw->style = strdup("font=FreeSerif.tff,size=32,fg=white,bg=black");
	gw->x1 = x;
	gw->y1 = y;
	gw->allocated = true;

	char *args[64];
	char *s = strdup(style);

	int n = parse(s, ",", args, 64);

	for (int i = 0; i < n; i++) {
		// printf("args[%d] = %s\n", i, args[i]);
		if (strncasecmp(args[i], "fn=", 3) == 0) {			// font name
			gw->fontName = strdup(&args[i][3]);
		} else if (strncasecmp(args[i], "fs=", 3) == 0) {	// font size
			gw->fontSize = atoi(&args[i][3]);
		} else if (strncasecmp(args[i], "fg=", 3) == 0) {	// foreground color
			gw->fg = fbglGetColor(fbgl, &args[i][3]);
		} else if (strncasecmp(args[i], "bg=", 3) == 0) {	// background color
			gw->bg = fbglGetColor(fbgl, &args[i][3]);
		}
	}
	free(s);

	if (gw->fontSize <= 0) {
		gw->fontSize = DEFAULT_FONTSIZE;
	}

	if (gw->fontName != NULL) {
		fbglLoadFont(fbgl, gw);
		// printf("gw->fontface %p\n", gw->fontFace);
	} else {
		gw->fontName = strdup(fbgl->dfn);
		fbglLoadFont(fbgl, gw);
	}

	if (gw->fg <= 0)
		gw->fg = fbglGetColor(fbgl, "white");
	if (gw->bg <= 0)
		gw->bg = fbglGetColor(fbgl, "black");
	
	// printf("%s, fs=%d, fg=%04x, bg=%04x\n", gw->fontName, gw->fontSize, gw->fg, gw->bg);
	
	return gw;
}
