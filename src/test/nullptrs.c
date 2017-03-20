/********************************************
 * Summary: demonstrate null ptr evaluation *
 * Author: devopsec                         *
 ********************************************/

#include <stdio.h>
#include <unistd.h>

main(){
	char *ptr1;
	char *ptr2 = '\0';
	char *ptr3 = {'\0'};
	char *ptr4 = NULL;

if (ptr1 == NULL) {
	printf("ptr1 is null\n");
}
else {
	printf("ptr1 is not null\n");
}

if (ptr2 == NULL) {
	printf("ptr2 is null\n");
}
else {
	printf("ptr2 is not null\n");
}

if (ptr3 == NULL) {
	printf("ptr3 is null\n");
}
else {
	printf("ptr3 is not null\n");
}

if (ptr4 == NULL) {
	printf("ptr4 is null\n");
}
else {
	printf("ptr4 is not null\n");
}
printf("done");
exit(0);
}
