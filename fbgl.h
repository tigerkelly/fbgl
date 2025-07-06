
#ifndef _FBGL_H
#define _FBGL_H

#include <stdbool.h>
#include <linux/fb.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <pthread.h>

#define LOG_NAME			"fbgl.log"

#define MAX_LINE_LENGTH		256
#define MAX_FONT_NUMBER		256
#define MAX_COLOR_NUMBER	150

#define MAX_WIDGET_COUNT	256

#define DEFAULT_FONTNAME	"FreeSerif"
#define DEFAULT_FONTSIZE	32
#define MAX_FONT_SIZE		200
#define MAX_COLOR_NAME		32

#define FONT_DB_NAME		"fbglFont.db"
#define COLOR_DB_NAME		"fbglColor.db"

typedef unsigned short FbglColor;

typedef enum {
	WIDGET_LABEL = 1,
	WIDGET_LINE,
	WIDGET_BOX,
	WIDGET_CIRCLE,
	WIDGET_IMAGE,
	WIDGET_UNKNOWN
} BasicWidgetType;

typedef struct _fbglwidget {
	bool allocated;
	bool vert;
	bool filled;
	bool readOnly;
	char *text;
	char *prompt;		// prompt text fro text field
	char *userData;
	char *style;
	char *name;
	char *fontName;
	char *imgFile;
	int wid;		// widget ID number
	int x1;
	int y1;
	int x2;
	int y2;
	int w;
	int h;
	int lineWidth;		// thickness of line.
	int radius;
	int fontSize;
	int borderWidth;
	BasicWidgetType wType;
	FT_Face fontFace;
	FbglColor fg;
	FbglColor bg;
	FbglColor bc;		// Border color
	FbglColor fc;		// Text field back ground color
	int (*callBack)(int event, struct _fbglwidget *w);
} FbglWidget;

typedef struct _colorlist {
	char *colorName;
	unsigned short colorValue;
} Color;

typedef struct _fbglFont {
	char *fontName;
	char *fontPath;
} FbglFont;

typedef struct _fbgl {
	bool stopEvent;
	bool stopTouch;
	bool stopMouse;
	bool stopKeyboard;
	int fd;
	int dfs;			// Default Font Size.
	int dbFontCnt;
	int dbColorCnt;
	int screenSize;
	int borderWidth;
	char *tmpDir;
	char *dev;
	char *screen;
	char *savedScreen;	// screen before program run.
	char *mouseDevice;
	char *touchDevice;
	char *kbDevice;
	char *dfn;			// Default Font Name.
	FbglFont *dbFont;
	Color *dbColor;
	FT_Library library;
	FT_Face defaultFace;
	FbglColor dfg;		// Default foreground color
	FbglColor dbg;		// Default background color
	FbglColor bc;		// Default border color
	FbglColor fc;		// Default text field color
	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;
} FBGL;

typedef struct _panel {
	char *panelName;
	int x;
	int y;
	int w;
	int h;
	FbglColor fg;
	FbglColor bg;
	FbglColor fc;
	FbglColor bc;
	FbglWidget *gw[MAX_WIDGET_COUNT];
} FbglPanel;

#if(0)
typedef struct {
    int rows;
    int cols;
    int **rmap;
    int **gmap;
    int **bmap;
} Image;
#endif

typedef struct {
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
} __attribute__((packed)) BMPFileHeader;

typedef struct {
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    // ... other fields
} __attribute__((packed)) BMPInfoHeader;

extern char *_fontPaths[];
extern Color _colors[];

void fbglLogIt(char *msg, ...);

int fbglPrint(FBGL *fbgl);

FbglPanel *fbglNewPanel(FBGL *fbgl, char *panelName, int x, int y, int w, int h, char *style);
int fbglDrawPanel(FBGL *fbgl, FbglPanel *panel);
FbglColor fbglGetColor(FBGL *fbgl, char *color);

int fbglWidgetAdd(FbglPanel *panel, void *widget);

FBGL *fbglInit(char *dev);
int fbglRestoreScreen(FBGL *fbgl);
char *fbglScanDevices(FBGL *fbgl, char *devType);
int fbglExit(FBGL *fbgl);
int fbglMapScreen(FBGL *fbgl);
int fbglPixel(FBGL *fbgl, int x, int y, FbglColor fg, int alpha);
int fbglClear(FBGL *fbgl);
int fbglSetBorderWidth(FBGL *fbgl, int borderWidth);
int fbglLoadFont(FBGL *fbgl, FbglWidget *gw);
int fbglLoadColors(FBGL *fbgl, FbglFont *fbglFont, FT_Face face);
char *fbglFindFont(FBGL *fbgl, char *fontName);
FbglColor fbglFindColor(FBGL *fblg, char *colorName);

void fbglPrintPanel(FBGL *fbgl, FbglPanel *fp);

char *fbglGetWidgetName(FbglWidget *gw);
char *fbglGetWidgetText(FbglWidget *gw);
char *fbglGetWidgetUserData(FbglWidget *gw);
int fbglGetWidgetId(FbglWidget *gw);
int fbglGetWidgetX(FbglWidget *gw);
int fbglGetWidgetY(FbglWidget *gw);
int fbglGetWidgetWidth(FbglWidget *gw);
int fbglGetWidgetheight(FbglWidget *gw);
BasicWidgetType fbglGetWidgetType(FbglWidget *gw);

// Create widgets
FbglWidget *fbglNewLabel(FBGL *fbgl, char *name, char *text, int x, int y, char *style);
FbglWidget *fbglNewBox(FBGL *fbgl, char *name, int x, int y, int x2, int y2, char *style);
FbglWidget *fbglNewLine(FBGL *fbgl, char *name, int x, int y, int x2, int y2, char *style);
FbglWidget *fbglNewCircle(FBGL *fbgl, char *name, int x, int y, int radius, char *style);
FbglWidget *fbglNewImage(FBGL *fbgl, char *name, char *imgFile, int x, int y, char *style);

#endif
