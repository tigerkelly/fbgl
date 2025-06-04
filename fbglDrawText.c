
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"
#include "fbglDraw.h"

int fbglDrawText(FBGL *fbgl, FbglWidget *gw) {

	if (fbgl == NULL) {
        return -1;
    }

	char *txt = gw->text;
	int x1 = gw->x1;
	int y1 = gw->y1;

	 while (*txt) {

        FT_Error error = FT_Load_Char(fbgl->defaultFace, *txt, FT_LOAD_RENDER);
        if (error) {
            fprintf(stderr, "Error loading character\n");
            return -1;
        }

        FT_GlyphSlot slot = fbgl->defaultFace->glyph;

        int advance = fbgl->defaultFace->glyph->metrics.horiAdvance / 64;

        // Print glyph bitmap data
        for (int y2 = 0; y2 < slot->bitmap.rows; y2++) {
            for (int x2 = 0; x2 < slot->bitmap.width; x2++) {
                // Access the bitmap screen directly
                unsigned char pixel_value = slot->bitmap.buffer[y2 * slot->bitmap.width + x2];
                int bbox_ymax = fbgl->defaultFace->bbox.yMax / 64;
                // int glyph_width = fbgl->defaultFace->glyph->metrics.width / 64;
                // int x_off = (advance - glyph_width) / 2;
                int y_off = bbox_ymax - fbgl->defaultFace->glyph->metrics.horiBearingY / 64;
                if (pixel_value > 128) {
                    fbglPixel(fbgl, x1 + x2, y1 + y2 + y_off, (gw->fg <= 0)? fbgl->dfg : gw->fg);
                } else {
                    fbglPixel(fbgl, x1 + x2, y1 + y2 + y_off, (gw->bg <= 0)? fbgl->dbg : gw->bg);
                }
            }
        }
        txt++;
        if (slot->bitmap.width <= 0)
            x1 += advance;
        else
            x1 += advance;
    }
	
	return 0;
}
