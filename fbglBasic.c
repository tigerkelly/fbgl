
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

#include "fbgl.h"
#include "fbglBasic.h"

int _fbglHLine(FBGL *fbgl, int x, int y, int x2, int y2, FbglColor fg, FbglColor bg) {

    if (fbgl == NULL) {
        return -1;
    }

    int len = x2 - x;

    for (int b = 0; b < fbgl->borderWidth; b++) {
        for (int i = 0; i < len; i++) {
            fbglPixel(fbgl, x + i, y + b, fg, 0);
        }
    }

    return 0;
}

int _fbglVLine(FBGL *fbgl, int x, int y, int x2, int y2, FbglColor fg, FbglColor bg) {

    if (fbgl == NULL) {
        return -1;
    }

    int len = y2 - y;

    for (int b = 0; b < fbgl->borderWidth; b++) {
        for (int i = 0; i < len; i++) {
            fbglPixel(fbgl, x + b, y + i, fg, 0);
        }
    }

    return 0;
}

int _fbglBox(FBGL *fbgl, int x, int y, int w, int h, FbglColor fg, FbglColor bg) {

    if (fbgl == NULL) {
        return -1;
    }

    for (int b = 0; b < fbgl->borderWidth; b++) {
        // Draw top line.
        for (int i = 0; i < w; i++) {
            fbglPixel(fbgl, x + i, y, fg, 0);
        }

        // Draw left line.
        for (int i = 0; i < h; i++) {
            fbglPixel(fbgl, x, y + i, fg, 0);
        }

        // Draw right line.
        for (int i = 0; i < h; i++) {
            fbglPixel(fbgl, x + w, y + i, fg, 0);
        }

        // Draw bottom line.
        for (int i = 0; i < w; i++) {
            fbglPixel(fbgl, x + i, y + h, fg, 0);
        }

        x++;
        y++;
        h -= 2;
        w -= 2;
    }

    return 0;
}

int _fbglFilledBox(FBGL *fbgl, int x1, int y1, int w, int h, FbglColor fg, FbglColor bg) {

    if (fbgl == NULL) {
        return -1;
    }

    for (int y = y1; y < y1 + h; y++) {
        for (int x = x1; x < x1 + w; x++) {
            fbglPixel(fbgl, x, y, fg, 0);
        }
    }

    return 0;
}

int _fbglCircle(FBGL *fbgl, int x0, int y0, int radius, FbglColor fg, FbglColor bg) {

    if (fbgl == NULL) {
        return -1;
    }

    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        fbglPixel(fbgl, x0 + x, y0 + y, fg, 0);
        fbglPixel(fbgl, x0 + y, y0 + x, fg, 0);
        fbglPixel(fbgl, x0 - y, y0 + x, fg, 0);
        fbglPixel(fbgl, x0 - x, y0 + y, fg, 0);
        fbglPixel(fbgl, x0 - x, y0 - y, fg, 0);
        fbglPixel(fbgl, x0 - y, y0 - x, fg, 0);
        fbglPixel(fbgl, x0 + y, y0 - x, fg, 0);
        fbglPixel(fbgl, x0 + x, y0 - y, fg, 0);

        if (err <= 0) {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0) {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    return 0;
}

int _fbglFilledCircle(FBGL *fbgl, int x0, int y0, int radius, FbglColor fg, FbglColor bg) {

    if (fbgl == NULL) {
        return -1;
    }

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                fbglPixel(fbgl, x0 + x, y0 + y, fg, 0);
            }
        }
    }

    return 0;
}

int _fbglSetFont(FBGL *fbgl, char *fontName, int fontsize, FT_Face *face) {

	if (fbgl == NULL) {
		return -1;
	}

	char *fontPath = fbglFindFont(fbgl, fontName);
	if (fontPath == NULL) {
		return -2;
	}

	FT_Error err = FT_New_Face(fbgl->library, fontPath, 0, face);
    if (err) {
        fprintf(stderr, "Error loading font face\n");
        return -1;
    }

    err = FT_Set_Pixel_Sizes(*face, 0, 48); // 0 for width means 'same as height'
    if (err) {
        fprintf(stderr, "Error setting font size\n");
        return -2;
    }

	return 0;
}

int _fbglText(FBGL *fbgl, int x, int y, char *text, char *fontName, int fontSize, FbglColor fg, FbglColor bg) {
	if (fbgl == NULL) {
        return -1;
    }

    if (text == NULL) {
        return -2;
    }

	FT_Face face;

	_fbglSetFont(fbgl, fontName, fontSize, &face);

    while (*text) {

        FT_Error error = FT_Load_Char(face, *text, FT_LOAD_RENDER);
        if (error) {
            fprintf(stderr, "Error loading character\n");
            return -1;
        }

        FT_GlyphSlot slot = fbgl->defaultFace->glyph;

        int advance = face->glyph->metrics.horiAdvance / 64;

        // Print glyph bitmap data
        for (int y2 = 0; y2 < slot->bitmap.rows; y2++) {
            for (int x2 = 0; x2 < slot->bitmap.width; x2++) {
                // Access the bitmap screen directly
                unsigned char pixel_value = slot->bitmap.buffer[y2 * slot->bitmap.width + x2];
                int bbox_ymax = face->bbox.yMax / 64;
                int y_off = bbox_ymax - face->glyph->metrics.horiBearingY / 64;

                if (pixel_value > 128) {
                    fbglPixel(fbgl, x + x2, y + y2 + y_off, (fg <= 0)? fbgl->dfg : fg, 0);
                } else {
                    fbglPixel(fbgl, x + x2, y + y2 + y_off, (bg <= 0)? fbgl->dbg : bg, 0);
                }
            }
        }
        text++;
        if (slot->bitmap.width <= 0)
            x += advance;
        else
            x += advance;
    }

	return 0;
}
