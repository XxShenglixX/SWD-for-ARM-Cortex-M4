#ifndef StringObject_H
#define StringObject_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

extern char *opSet;
extern char *numSet;
extern char *alphaSet;
extern char *hexNumSet;
extern char *folderNameSet;
extern char *alphaNumericSet;

typedef struct
{
	char *string;
	int startindex;
	int length;
}String;

String *stringNew(char *charString);
void stringTrimLeft(String *str);
void stringTrimRight(String *str);
void stringTrim(String *str);
String *stringRemoveWordContaining (String *str , char *containSet);
String *stringRemoveWordNotContaining (String *str , char *notContainSet);
int stringCharAt(String *str ,  int relativePos);
int stringRemoveChar(String *str);
void stringSkip(String *str , int numOfCharToSkip);
char *stringSubStringInChars(String *str , int length);
int subStringToInteger(char *subString);
void stringDel(String *str);
void subStringDel(char *subString);
int stringCharAtInSet(String *str , int relativePos , char *containSet);
String *stringSubString(String *str , int start , int length);
String *stringRemoveOperator(String *str , char *containSet);
#endif // StringObject_H
