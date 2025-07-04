
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <libxml/parser.h>

#include "fbgl.h"

int fbglLoadXml(char *xmlFile) {

	xmlDoc         *document;
    xmlNode        *root, *first_child, *node;

    if (xmlFile == NULL) {
        fprintf(stderr, "No XML file name given\n");
        return 1;
    }

    document = xmlReadFile(xmlFile, NULL, 0);
    root = xmlDocGetRootElement(document);
    fprintf(stdout, "Root is <%s> (%i)\n", root->name, root->type);
    first_child = root->children;
    for (node = first_child; node; node = node->next) {
        fprintf(stdout, "\t Child is <%s> (%i)\n", node->name, node->type);
    }
    fprintf(stdout, "...\n");

	return 0;
}
