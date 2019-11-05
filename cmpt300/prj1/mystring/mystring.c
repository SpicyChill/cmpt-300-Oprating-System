#include "mystring.h"
#include <stdio.h>
#include <stdlib.h>

/*
 *   Implement the following functions: 
 * 
 *   You are NOT allowed to use any standard string functions such as 
 *   strlen, strcpy or strcmp or any other string function.
 */

/*
 *  mystrlen() calculates the length of the string s, 
 *  not including the terminating character '\0'.
 *  Returns: length of s.
 */
int mystrlen (const char *s) 
{
	/* Complete the body of the function */
	int len = 0;

	while(s[len]!='\0')
		len++;

	return len;
}

/*
 *  mystrcpy()  copies the string pointed to by src (including the terminating 
 *  character '\0') to the array pointed to by dst.
 *  Returns: a pointer to the destination string dst.
 */
char  *mystrcpy (char *dst, const char *src)
{
	/* Complete the body of the function */
	int i=0;
	while(src[i]!='\0')
	{
		dst[i]=src[i];
		i++;
	}
	dst[i]='\0';
	return dst;
}

/*
 * mystrcmp() compares two strings alphabetically
 * Returns: 
 * 	-1 if s1  < s2
 *  	0 if s1 == s2
 *  	1 if s1 > s2
 */
int mystrcmp(const char *s1, const char *s2)
{
	/* Complete the body of the function */
	int i=0;
	while(s1[i]==s2[i]){
		if(s1[i]=='\0'&&s2[i]=='\0')
			break;
		i++;
	}
	int t = s1[i]-s2[i];
	if(t>0)
		return 1;
	if(t<0)
		return -1;
	return t;
}

/*
 * mystrdup() creates a duplicate of the string pointed to by s. 
 * The space for the new string is obtained using malloc.  
 * If the new string can not be created, a null pointer is returned.
 * Returns:  a pointer to a new string (the duplicate) 
 	     or null If the new string could not be created for 
	     any reason such as insufficient memory.
 */
char *mystrdup(const char *s1)
{
	/* Complete the body of the function */
	char * dupStr;
	int i = 0;
	
	if (s1==NULL)
		return NULL;

	dupStr = (char *)malloc(mystrlen(s1)+1);
	if(dupStr==NULL)
	{
	    return NULL;
	}
	
	for(i=0; s1[i]!='\0'; i++)
	{
	    dupStr[i] = s1[i];
	}
	dupStr[i]='\0';
	
	return dupStr;
}

