
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"

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
