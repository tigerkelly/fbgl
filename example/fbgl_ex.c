
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

		char *style = "fg=aqua,bg=black";
		FbglPanel *panel = fbglNewPanel(fbgl, "panel", 0, 0, 0, 0, style);

		style = "font=FreeSerif.ttf,size=96,fg=aqua,bg=black";
		FbglWidget *lbl = fbglNewText(fbgl, "lbl1", "Label text goes here.", 100, 100, style);
		fbglWidgetAdd(panel, lbl);

		style = "bc=blue,fc=fuchsia,fg=blue,bg=white";
		FbglWidget *circle1 = fbglNewCircle(fbgl, "txt1", 160, 500, 150, style);
		fbglWidgetAdd(panel, circle1);

		style = "bc=yellow,fc=fuchsia,fg=blue,bg=white,filled=true";
		FbglWidget *circle2 = fbglNewCircle(fbgl, "txt1", 500, 500, 150, style);
		fbglWidgetAdd(panel, circle2);

		style = "font=FreeSerif.ttf,size=64,fg=blue,bg=white";
		FbglWidget *box = fbglNewBox(fbgl, "box1", 100, 150, 200, 150, style);
		fbglWidgetAdd(panel, box);

		style = "font=FreeSerif.ttf,size=64,fc=cornsilk,fg=red,bg=white,filled=true";
		FbglWidget *box2 = fbglNewBox(fbgl, "box2", 325, 150, 200, 150, style);
		fbglWidgetAdd(panel, box2);

		fbglDrawPanel(fbgl, panel);

		sleep(15);

		fbglExit(fbgl);
	}


	return 0;
}

void signalHandler(int signum) {
	// printf("Signal %d received\n", signum);
	fbglExit(fbgl);
	exit(1);
}
