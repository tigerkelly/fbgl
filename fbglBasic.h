
#ifndef FBGLSHAPES_H
#define FBGLSHAPES_H

int _fbglHLine(FBGL *fbgl, int x, int y, int x2, int y2, FbglColor fg, FbglColor bg);
int _fbglVLine(FBGL *fbgl, int x, int y, int x2, int y2, FbglColor fg, FbglColor bg);
int _fbglBox(FBGL *fbgl, int x, int y, int w, int h, FbglColor fg, FbglColor bg);
int _fbglText(FBGL *fbgl, int x, int y, char *text, char *fontName, int fontSize, FbglColor fg, FbglColor bg);
int _fbglFilledBox(FBGL *fbgl, int x1, int y1, int w, int h, FbglColor fg, FbglColor bg);
int _fbglCircle(FBGL *fbgl, int x0, int y0, int radius, FbglColor fg, FbglColor bg);
int _fbglFilledCircle(FBGL *fbgl, int x0, int y0, int radius, FbglColor fg, FbglColor bg);

#endif
