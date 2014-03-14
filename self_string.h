//I am too lazy so I wrote all the things in the head file
#ifndef SELF_STRING_H
#define SELF_STRING_H
#include <ctype.h>
int strbcmp(char *a,char *b){
unsigned int i=0;
if(strlen(a) != strlen(b)) return 1;
while(a[i] != '\0' && b[i] != '\0'){
if(toupper(a[i]) != toupper(b[i])) return 1;
i++;
}
return 0;
}
#endif
