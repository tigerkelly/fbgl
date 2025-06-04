
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#include "fbgl.h"

char *getWidgetPrompt(FbglWidget *gw) {
	if (gw == NULL) {
		return NULL;
	}

	return gw->prompt;
}

char *fbglGetWidgetName(FbglWidget *gw) {
	if (gw == NULL) {
		return NULL;
	}

	return gw->name;
}

char *fbglGetWidgetText(FbglWidget *gw) {
	if (gw == NULL) {
		return NULL;
	}

	return gw->text;
}

char *fbglGetWidgetUserData(FbglWidget *gw) {
	if (gw == NULL) {
		return NULL;
	}

	return gw->userData;
}

int fbglGetWidgetId(FbglWidget *gw) {
	if (gw == NULL) {
		return -1;
	}

	return gw->wid;
}

int fbglGetWidgetX(FbglWidget *gw) {
	if (gw == NULL) {
		return -1;
	}

	return gw->x1;
}

int fbglGetWidgetY(FbglWidget *gw) {
	if (gw == NULL) {
		return -1;
	}

	return gw->y1;
}

int fbglGetWidgetWidth(FbglWidget *gw) {
	if (gw == NULL) {
		return -1;
	}

	return gw->w;
}

int fbglGetWidgetHeight(FbglWidget *gw) {
	if (gw == NULL) {
		return -1;
	}

	return gw->h;
}

BasicWidgetType fbglGetWidgetType(FbglWidget *gw) {
	if (gw == NULL) {
		return WIDGET_UNKNOWN;
	}

	return gw->wType;
}

/***************************************************************/

int fbglSetWidgetPrompt(FbglWidget *gw, char *prompt) {

	if (gw == NULL) {
		return -1;
	}

	gw->prompt = strdup(prompt);

	return 0;
}

int fbglSetWidgetName(FbglWidget *gw, char *name) {
	if (gw == NULL) {
		return -1;
	}

	if (gw->name != NULL) {
		free(gw->name);
	}

	gw->name = strdup(name);

	return 0;
}

int fbglSetWidgetText(FbglWidget *gw, char *text) {
	if (gw == NULL) {
		return -1;
	}

	if (gw->text != NULL) {
		free(gw->text);
	}

	gw->text = strdup(text);

	return 0;
}

int fbglSetWidgetUserData(FbglWidget *gw, char *userData) {
	if (gw == NULL) {
		return -1;
	}

	if (gw->text != NULL) {
		free(gw->userData);
	}

	gw->userData = strdup(userData);

	return 0;
}

int fbglSetWidgetWidgetType(FbglWidget *gw, BasicWidgetType wType) {
	if (gw == NULL) {
		return -1;
	}

	gw->wType = wType;

	return 0;
}
