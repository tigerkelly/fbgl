
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"
#include "strutils.h"

int fbglDrawLabel(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
        return -1;
    }

	char *txt = gw->text;
	int x1 = gw->x1;
	int y1 = gw->y1;
	FT_Error error;

	while (*txt) {

		if (gw->fontFace != NULL) {
			error = FT_Load_Char(gw->fontFace, *txt, FT_LOAD_RENDER);
		} else {
			error = FT_Load_Char(fbgl->defaultFace, *txt, FT_LOAD_RENDER);
		}
        if (error) {
            fprintf(stderr, "Error loading character\n");
            return -1;
        }


        FT_GlyphSlot slot;
		if (gw->fontFace != NULL) {
			slot = gw->fontFace->glyph;
		} else {
			slot = fbgl->defaultFace->glyph;
		}

		int advance = 0;
		if (gw->fontFace != NULL) {
			advance = gw->fontFace->glyph->metrics.horiAdvance / 64;
		} else {
			advance = fbgl->defaultFace->glyph->metrics.horiAdvance / 64;
		}

        // Print glyph bitmap data
        for (int y2 = 0; y2 < slot->bitmap.rows; y2++) {
            for (int x2 = 0; x2 < slot->bitmap.width; x2++) {
                // Access the bitmap screen directly
                unsigned char pixelValue = slot->bitmap.buffer[y2 * slot->bitmap.width + x2];
                int bbox_ymax = 0;
                int y_off = 0;
				if (gw->fontFace != NULL) {
					bbox_ymax = gw->fontFace->bbox.yMax / 64;
					y_off = bbox_ymax - gw->fontFace->glyph->metrics.horiBearingY / 64;
				} else {
					bbox_ymax = fbgl->defaultFace->bbox.yMax / 64;
					y_off = bbox_ymax - fbgl->defaultFace->glyph->metrics.horiBearingY / 64;
				}
                if (pixelValue > 128) {
                    fbglPixel(fbgl, x1 + x2, y1 + y2 + y_off, (gw->fg <= 0)? fbgl->dfg : gw->fg, 0);
                } else {
                    fbglPixel(fbgl, x1 + x2, y1 + y2 + y_off, (gw->bg <= 0)? fbgl->dbg : gw->bg, 0);
                }
            }
        }
        txt++;
		x1 += advance;
    }
	
	return 0;
}

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
