
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"

int fbglPrint(FBGL * fbgl) {

	printf("FBGL:\n");

	printf("  screenSize  %d\n", fbgl->screenSize);
	printf("  dfg         %d\n", fbgl->dfg);
	printf("  dbg         %d\n", fbgl->dbg);
	printf("  borderWidth %d\n", fbgl->borderWidth);
	printf("  dev         %s\n", fbgl->dev);
	printf("  mouseDevice %s\n", fbgl->mouseDevice);
	printf("  touchDevice %s\n", fbgl->touchDevice);
	printf("  kbDevice    %s\n", fbgl->kbDevice);

	return 0;
}
