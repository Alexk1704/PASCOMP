#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void *cmalloc_fatal(size_t size) {
	if(size==0)
		return NULL;
	fprintf(stderr, "Ran out of memory.\n");
	exit(1);
}

/* checked malloc and returns a pointer */
void *cmalloc(size_t size) {
	void *ptr = malloc(size);
	if(ptr==NULL)
		return cmalloc_fatal(size);
	return ptr;
}

/* checked malloc string copy, returns a char pointer */
char *cstrdup(const char *s) {
	void *ptr = cmalloc(strlen(s)+1);
	strcpy(ptr, s);
	return (char*) ptr;
}

int boolval(const char *s) {
	if((strcmp(s, "true")) == 0) return 1;
	return 0;
}
