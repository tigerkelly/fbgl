# fbgl — Framebuffer Graphics Library

A lightweight C graphics library for drawing directly to the Linux framebuffer (`/dev/fb0`), designed primarily for embedded Linux systems such as the Raspberry Pi. No X11, Wayland, or display server required.

---

## Features

- Direct framebuffer rendering (no windowing system needed)
- Supports **16-bit (RGB565)** and **32-bit (RGBA8888)** color depths
- FreeType2-powered text rendering with support for a wide range of TTF fonts
- Named color database with **150+ CSS-style color names** (e.g. `"cornflowerblue"`, `"crimson"`)
- Widget-based drawing model organized around **panels**
- Built-in widget types: **Label**, **Box**, **Line**, **Circle**, **Arc**, **Image** (PNG)
- Style strings for concise widget configuration
- Screen save/restore on init and exit
- Static library output (`libfbgl.a`)

---

## Requirements

### System Dependencies

```bash
sudo apt install -y libfreetype-dev libpng-dev
```

### tigerkelly/utils

fbgl depends on the `strutils` and `ini` helpers from:

```bash
git clone https://github.com/tigerkelly/utils.git
```

Place the `utils` repository at the **same directory level** as `fbgl`:

```
parent/
├── fbgl/
└── utils/
```

---

## Building

```bash
git clone https://github.com/tigerkelly/fbgl.git
cd fbgl
make
```

This produces `libfbgl.a` in the `fbgl/` directory.

To build with profiling enabled:

```bash
make PROFILE=yes
```

### Building the Example

```bash
cd example
make
```

---

## Framebuffer Setup

Before running, confirm your framebuffer is set to 32-bit color:

```bash
fbset
```

Look for the `geometry` line. The last number is bits-per-pixel:

```
mode "1024x600"
    geometry 1024 600 1024 600 32
    ...
endmode
```

If it does not show 32 bits, set it manually:

```bash
fbset -g 1024 600 1024 600 32
```

If `fbset` does not take effect, you may need to comment out the following line in `/boot/firmware/config.txt` (or `/boot/config.txt`) and reboot:

```
#dtoverlay=vc4-kms-v3d
```

For a WaveShare 7-inch 1024×600 display, add to `config.txt`:

```
hdmi_group=2
hdmi_mode=87
hdmi_cvt 1024 600 60 6 0 0 0
hdmi_drive=1
```

---

## API Reference

### Initialization & Cleanup

```c
FBGL *fbglInit(char *dev);
```
Opens the framebuffer device (e.g. `"/dev/fb0"`), maps the screen into memory, initializes FreeType2, and loads the color and font databases. Returns `NULL` on failure.

```c
int fbglExit(FBGL *fbgl);
```
Restores the original screen contents and frees all resources.

```c
int fbglMapScreen(FBGL *fbgl);
int fbglRestoreScreen(FBGL *fbgl);
```
Low-level screen mapping and restoration helpers (called automatically by `fbglInit`/`fbglExit`).

---

### Screen Primitives

```c
int fbglClear(FBGL *fbgl);
```
Clears the screen to the default background color.

```c
int fbglPixel(FBGL *fbgl, int x, int y, FbglColor fg, int alpha);
```
Sets a single pixel. Supports both 16-bit and 32-bit color depths. Returns `-2` for out-of-bounds coordinates.

```c
int fbglSetBorderWidth(FBGL *fbgl, int borderWidth);
```
Sets the global default border width (1–31 pixels).

---

### Colors

Colors are represented as `FbglColor` (an `unsigned short` in RGB565 format). Named colors can be looked up by string:

```c
FbglColor fbglGetColor(FBGL *fbgl, char *colorName);
FbglColor fbglFindColor(FBGL *fbgl, char *colorName);
```

Over 150 CSS-standard color names are supported, including `"black"`, `"white"`, `"red"`, `"cornsilk"`, `"cornflowerblue"`, `"lightgreen"`, and many more. Color lookup is case-insensitive.

---

### Fonts

fbgl uses FreeType2. The font database is pre-populated with paths to common system fonts (DejaVu, FreeFonts, Piboto, Droid, etc.).

```c
char *fbglFindFont(FBGL *fbgl, char *fontName);
int   fbglLoadFont(FBGL *fbgl, FbglWidget *gw);
```

The default font is `FreeSerif` at size 32. Font names in styles omit the `.ttf` extension (e.g. `"FreeSansBold"`).

---

### Panels

Panels are containers that group widgets together. Drawing a panel draws all widgets it contains.

```c
FbglPanel *fbglNewPanel(FBGL *fbgl, char *panelName, int x, int y, int w, int h, char *style);
int        fbglDrawPanel(FBGL *fbgl, FbglPanel *panel);
int        fbglWidgetAdd(FbglPanel *panel, void *widget);
void       fbglPrintPanel(FBGL *fbgl, FbglPanel *fp);  // debug print
```

Pass `w=0, h=0` to use full screen dimensions.

---

### Widgets

All widget constructors accept a **style string** — a comma-separated list of `key=value` pairs with no spaces around `=`.

#### Label

Renders text at a given position.

```c
FbglWidget *fbglNewLabel(FBGL *fbgl, char *name, char *text, int x, int y, char *style);
```

Style keys: `fn` (font name), `fs` (font size), `fg` (foreground color), `bg` (background color)

```c
// Example
FbglWidget *lbl = fbglNewLabel(fbgl, "title", "Hello World", 100, 50, "fn=FreeSansBold,fs=64,fg=white,bg=black");
```

#### Box

Draws a rectangle, optionally filled.

```c
FbglWidget *fbglNewBox(FBGL *fbgl, char *name, int x, int y, int x2, int y2, char *style);
```

Style keys: `fg`, `bg`, `bc` (border color), `fc` (fill color), `filled` (`true`/`false`)

#### Line

Draws a horizontal or vertical line.

```c
FbglWidget *fbglNewLine(FBGL *fbgl, char *name, int x, int y, int x2, int y2, char *style);
```

Style keys: `fg`, `bg`, `lw` (line width), `vert` (`true` for vertical, `false` for horizontal)

#### Circle

Draws a circle, optionally filled.

```c
FbglWidget *fbglNewCircle(FBGL *fbgl, char *name, int x, int y, int radius, char *style);
```

Style keys: `fg`, `bg`, `bc`, `fc`, `filled` (`true`/`false`)

#### Arc

Draws an arc between two angles.

```c
FbglWidget *fbglNewArc(FBGL *fbgl, char *name, int x, int y, int radius, char *style);
```

Style keys: `fg`, `bg`, `lw` (line width), `sa` (start angle in degrees), `ea` (end angle — must be greater than `sa`)

#### Image (PNG)

Renders a PNG file at a given position.

```c
FbglWidget *fbglNewImage(FBGL *fbgl, char *name, char *imgFile, int x, int y, char *style);
```

Style keys: `bc` (border color), `bw` (border width)

---

### Widget Accessors

```c
char            *fbglGetWidgetName(FbglWidget *gw);
char            *fbglGetWidgetText(FbglWidget *gw);
char            *fbglGetWidgetUserData(FbglWidget *gw);
int              fbglGetWidgetId(FbglWidget *gw);
int              fbglGetWidgetX(FbglWidget *gw);
int              fbglGetWidgetY(FbglWidget *gw);
int              fbglGetWidgetWidth(FbglWidget *gw);
int              fbglGetWidgetheight(FbglWidget *gw);
BasicWidgetType  fbglGetWidgetType(FbglWidget *gw);
```

---

## Style String Reference

Style strings are comma-separated `key=value` pairs with **no whitespace** around `=`.

| Key       | Description                          | Applies To                   |
|-----------|--------------------------------------|------------------------------|
| `fg`      | Foreground / text color              | All widgets                  |
| `bg`      | Background color                     | All widgets                  |
| `bc`      | Border color                         | Box, Circle, Image           |
| `fc`      | Fill color                           | Box, Circle                  |
| `fn`      | Font name (no `.ttf` extension)      | Label                        |
| `fs`      | Font size in pixels                  | Label                        |
| `filled`  | `true` to fill shape with `fc`       | Box, Circle                  |
| `lw`      | Line width in pixels                 | Line, Arc                    |
| `vert`    | `true` = vertical, `false` = horiz. | Line                         |
| `sa`      | Arc start angle (degrees)            | Arc                          |
| `ea`      | Arc end angle (degrees, > `sa`)      | Arc                          |
| `bw`      | Border width in pixels               | Image                        |

---

## Example

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "fbgl.h"

FBGL *fbgl;

void signalHandler(int signum) {
    fbglExit(fbgl);
    exit(1);
}

int main() {
    struct sigaction sa = { .sa_handler = signalHandler };
    sigaction(SIGINT, &sa, NULL);

    fbgl = fbglInit("/dev/fb0");
    if (!fbgl) return 1;

    fbglSetBorderWidth(fbgl, 4);

    // Create a full-screen panel
    FbglPanel *panel = fbglNewPanel(fbgl, "main", 0, 0, 0, 0, "fg=blue,bg=black");

    // Add a label
    FbglWidget *lbl = fbglNewLabel(fbgl, "lbl1", "Hello, fbgl!", 100, 50,
                                   "fn=FreeSansBold,fs=64,fg=green,bg=black");
    fbglWidgetAdd(panel, lbl);

    // Add a filled circle
    FbglWidget *circle = fbglNewCircle(fbgl, "c1", 300, 300, 100,
                                       "bc=red,fc=yellow,filled=true");
    fbglWidgetAdd(panel, circle);

    // Add a PNG image
    FbglWidget *img = fbglNewImage(fbgl, "logo", "logo.png", 500, 200, "bc=white,bw=2");
    fbglWidgetAdd(panel, img);

    // Add an arc (90° to 150°)
    FbglWidget *arc = fbglNewArc(fbgl, "arc1", 700, 400, 80,
                                 "lw=6,fg=cyan,sa=90.0,ea=150.0");
    fbglWidgetAdd(panel, arc);

    // Draw everything
    fbglDrawPanel(fbgl, panel);

    sleep(10);
    fbglExit(fbgl);
    return 0;
}
```

Compile and link against the library:

```bash
gcc -std=gnu99 -I../fbgl -I../utils/incs \
    -o my_app my_app.c \
    -L../fbgl -lfbgl \
    -L../utils/libs -lstrutils \
    -lfreetype -lpng -lrt -lpthread
```

---

## Project Structure

```
fbgl/
├── fbgl.h              # Main public header — all types and function declarations
├── fbgl.c              # Core: init, exit, screen map, pixel drawing, color/font lookup
├── fbglBasic.c/.h      # Low-level shape drawing primitives (lines, boxes, circles, text)
├── fbglArc.c           # Arc drawing
├── fbglBox.c           # Box widget
├── fbglCircle.c        # Circle widget
├── fbglImage.c         # PNG image widget (uses libpng)
├── fbglLabel.c         # Label / text widget
├── fbglLine.c          # Line widget
├── fbglPanel.c         # Panel container and widget dispatch
├── fbglWidget.c        # Widget accessors
├── fbglColor.c         # Color parsing / style helpers
├── fbglColors.c        # Named color database (150+ RGB565 entries)
├── fbglFonts.c         # Font path database
├── fbglFont.db         # Font metadata
├── fbglColor.db        # Color metadata
├── fbglMisc.c          # Miscellaneous helpers
├── fbglLogIt.c         # Logging helper
├── fbglPrint.c         # Debug print
├── fbglDraw.h          # Internal draw dispatch header
├── fbglWidgetStyle.txt # Style string documentation
├── Makefile            # Builds libfbgl.a
└── example/
    ├── fbgl_ex.c       # Complete usage example
    └── Makefile
```

---

## License

See repository for license details.
