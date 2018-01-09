#ifndef __SYSSTRING_H
#define __SYSSTRING_H


char* strcpy(char* dest, char* src);
char* strcpy2(char* dest, char* src, char c);
int strcmp(char* str1, char* str2);
int strncmp(char* s1, char* s2, int n);
int strlen(char* str);
char* strcat(char *str1, char *str2);
int strtok(char str[], char  splitter, char *tokens[]);
int str_contains_char(char* string, char sign);
int str_contains_substr(char* str, char* substr);
int str_contains_substr2(char* str, char* substr, int start_ind, int end_ind);

#endif
