
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "fbgl.h"

char *getColorName(FBGL *fbgl, unsigned short value);

void fbglPrintPanel(FBGL *fbgl, FbglPanel *fp) {

    if (fbgl == NULL || fp == NULL) {
        return;
    }

	printf("FbglPanel Name: %s\n", fp->panelName);
	printf("    x = %d\n", fp->x);
	printf("    y = %d\n", fp->y);
	printf("    w = %d\n", fp->w);
	printf("    h = %d\n", fp->h);
	printf("    fg = 0x%04x %s\n", fp->fg, getColorName(fbgl, fp->fg));
	printf("    bg = 0x%04x %s\n", fp->bg, getColorName(fbgl, fp->bg));
	printf("    fc = 0x%04x %s\n", fp->fc, getColorName(fbgl, fp->fc));
	printf("    bc = 0x%04x %s\n", fp->bc, getColorName(fbgl, fp->bc));

	for (int i = 0; i < MAX_WIDGET_COUNT; i++) {
		if (fp->gw[i] == NULL)
			break;
		char *s = NULL;
		switch (fp->gw[i]->wType) {
			case WIDGET_LABEL:
				s = "Label";
				break;
			case WIDGET_BOX:
				s = "Box";
				break;
			case WIDGET_CIRCLE:
				s = "Circle";
				break;
			case WIDGET_IMAGE:
				s = "Image";
				break;
			case WIDGET_UNKNOWN:
				s = "Unknown";
				break;
			default:
				s = "Unknown";
				break;
		}
		printf("  Widget Name: %s, Type: %s\n", fp->gw[i]->name, s);
		printf("    allocated:    %d\n", fp->gw[i]->allocated);
		printf("    vert:         %s\n", (fp->gw[i]->vert) ? "T":"F");
		printf("    filled:       %s\n", (fp->gw[i]->filled) ? "T":"F");
		printf("    readOnly:     %s\n", (fp->gw[i]->readOnly) ? "T":"F");
		printf("    text:         %s\n", fp->gw[i]->text);
		printf("    prompt:       %s\n", fp->gw[i]->prompt);
		printf("    userData:     %s\n", fp->gw[i]->userData);
		printf("    style:        %s\n", fp->gw[i]->style);
		printf("    fontname:     %s\n", fp->gw[i]->fontName);
		printf("    imgFile:      %s\n", fp->gw[i]->imgFile);
		printf("    wid:          %d\n", fp->gw[i]->wid);
		printf("    x1:           %d\n", fp->gw[i]->x1);
		printf("    y1:           %d\n", fp->gw[i]->y1);
		printf("    x2:           %d\n", fp->gw[i]->x2);
		printf("    y2:           %d\n", fp->gw[i]->y2);
		printf("    w:            %d\n", fp->gw[i]->w);
		printf("    h:            %d\n", fp->gw[i]->h);
		printf("    radius:       %d\n", fp->gw[i]->radius);
		printf("    fontSize:     %d\n", fp->gw[i]->fontSize);
		printf("    borderWidth:  %d\n", fp->gw[i]->borderWidth);
		printf("    fg:           0x%04x %s\n", fp->gw[i]->fg, getColorName(fbgl, fp->gw[i]->fg));
		printf("    bg:           0x%04x %s\n", fp->gw[i]->bg, getColorName(fbgl, fp->gw[i]->bg));
		printf("    bc:           0x%04x %s\n", fp->gw[i]->bc, getColorName(fbgl, fp->gw[i]->bc));
		printf("    fc:           0x%04x %s\n", fp->gw[i]->fc, getColorName(fbgl, fp->gw[i]->fc));

		printf("\n");
	}

}

char *getColorName(FBGL *fbgl, unsigned short value) {
	char *colorName = NULL;

	Color *cp = fbgl->dbColor;
    for (int i = 0; i < fbgl->dbColorCnt; i++) {
        if (cp->colorValue == value) {
            colorName = cp->colorName;
            break;
        }
        cp++;
    }

	return colorName;
}
