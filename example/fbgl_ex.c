
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#include "fbgl.h"

int fd;

FBGL *fbgl;

void hideCursor();
void signalHandler(int signum);

int main(int argc, char *argv[]) {

	struct sigaction sa;
	sa.sa_handler = signalHandler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		return 1;
	}

	fbgl = fbglInit("/dev/fb0");
	if (fbgl != NULL) {
		fbglSetBorderWidth(fbgl, 4);

		sleep(1);

		char *style = "fg=blue,bg=black";
		FbglPanel *panel = fbglNewPanel(fbgl, "panel", 0, 0, 0, 0, style);

		style = "fn=FreeSanBold,fs=64,fg=green,bg=black";
		FbglWidget *lbl1 = fbglNewLabel(fbgl, "lbl1", "Text goes here.", 600, 100, style);
		fbglWidgetAdd(panel, lbl1);

		// Use default font and size.
		style = "fg=green,bg=black";
		FbglWidget *lbl2 = fbglNewLabel(fbgl, "lbl2", "Default font.", 600, 150, style);
		fbglWidgetAdd(panel, lbl2);

		style = "bc=red,fc=yellow,fg=white,bg=black,filled=true";
		FbglWidget *circle2 = fbglNewCircle(fbgl, "txt1", 300, 300, (300 - (fbgl->borderWidth *2)), style);
		fbglWidgetAdd(panel, circle2);

		style = "bc=red,fc=lightgreen,fg=white,bg=black,filled=true";
		FbglWidget *box1 = fbglNewBox(fbgl, "box1", 600, 300, 100, 100, style);
		fbglWidgetAdd(panel, box1);

		style = "bc=red,bw=3";
		FbglWidget *img1 = fbglNewImage(fbgl, "img1", "logo.png", 800, 250, style);
		fbglWidgetAdd(panel, img1);

		style = "lw=8,fg=red,bg=black,vert=false";	// horizontal line
		FbglWidget *hline1 = fbglNewLine(fbgl, "hline1", 750, 450, 950, 450, style);
		fbglWidgetAdd(panel, hline1);

		style = "lw=8,fg=red,bg=black,vert=true";	// vertical line
		FbglWidget *vline1 = fbglNewLine(fbgl, "vline1", 750, 455, 475, 555, style);
		fbglWidgetAdd(panel, vline1);

		// fbglPrintPanel(fbgl, panel);

		fbglDrawPanel(fbgl, panel);

		sleep(10);

		fbglExit(fbgl);
	}


	return 0;
}

void signalHandler(int signum) {
	// printf("Signal %d received\n", signum);
	fbglExit(fbgl);
	exit(1);
}
