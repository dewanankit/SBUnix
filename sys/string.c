#include "sys/string.h"

/*
 * Function for copying string from src to dest
 */
char* strcpy(char* dest, char* src) {
	char* result = dest;
	while(*src != '\0'){
		*dest++ = *src++;
	}
	*dest = '\0';
	return result;
}

/*
 * Function for copying string from src to dest by removing character c
 */
char* strcpy2(char* dest, char* src, char c) {
	int i = 0, j = 0;
	while(src[i] != '\0') {
		if(src[i] != c) {
			dest[j] = src[i];
			i++;
			j++;
		}
		else {
			i++;
		}
	}
	return &dest[0];
}

int strncmp(char* s1, char* s2, int n) {
	int i;
	for(i=0; n>0; n--, i++) {
		if(s1[i] != s2[i]) {
			return (s1[i] - s2[i]);
		}
	}
	return n;
}

/*
 * Function for comparing two strings
 * Returns 0 if strings are equal, else returns value<0 if str1<str2, else returns value>0 if str1>str2
 */
int strcmp(char* str1, char* str2) {
	int i = 0;
	while(1) {
		if(str1[i] != str2[i]) {
			return (str1[i] > str2[i] ? 1 : -1);
		}
		if(str1[i] == '\0' && str2[i] == '\0') {
			return 0;
		}
		i++;
	}
}

/*
 * Function for string length
 */
int strlen(char* str) {
	int length = 0;
	while(str[length++] != '\0'){};
	return length-1;
}

/*
 * Function for string concatenation
 */
char* strcat(char *str1, char *str2) {
	char temp[1000];
	strcpy(temp, str1);
	int len = strlen(temp);
	strcpy(&temp[len], str2);
	strcpy(&str1[0], temp);
	return str1;
}


/*
 * Function returns number of tokens of string split on the basis of splitter character
 * as well as the tokens array which contains the tokenized string
 */

int strtok(char str[], char  splitter, char *tokens[]) {
	int i = 0;
	int j = 0;
	while(str[j] != '\0') {
		while(str[j] == splitter ) {
			str[j] = '\0';
			j++;
		}
		if(str[j] != '\0' ) {
			tokens[i] = str + j;
			while(((str[j]) != splitter) && (str[j] != '\0')) {
				j++;
			}
			i++;
		}
	}
	tokens[i] = 0;
	return i;
}

/*
 * Function returns index of character in string, else returns -1
 */
int str_contains_char(char* string, char sign) {
	int position = -1;
	int contains = 0;
	while(*string != '\0' && contains == 0) {
		position++;
		if(*string == sign) {
			contains = 1;
		}
		string++;
	}
	if(contains == 1) {
		return position;
	}
	else {
		return -1;
	}
}

/*
 * Function returns 1 if substring is present in string, else returns 0;
 * Can return index of substring in string by uncommenting commented parts
 */
int str_contains_substr(char* str, char* substr) {
	int found = 0, i = 0, j = 0;
//	int foundindex = 0;
	char *temp1, *temp2, *temp3;
	temp1 = str;
	temp2 = substr;
	for(i=0; i<strlen(str); i++) {
		if(*temp1 == *temp2) {
			temp3 = temp1;
			for(j=0; j<strlen(substr); j++) {
				if(*temp3 == *temp2) {
					temp2++;
					temp3++;
				}
				else {
					break;
				}
			}
			temp2 = substr;
			if(j == strlen(substr)) {
				found = 1;
//				foundindex = i;
				break;
			}
		}
		temp1++;
	}
	return found;
//	return foundindex;
}

int str_contains_substr2(char* str, char* substr, int start_ind, int end_ind) {
	int i = start_ind, j = 0;
	int found = 1;
	int substrlen = strlen(substr);
	while(i<=end_ind) {
		if(str[i] != substr[j]) {
			found = 0;
			i++;
			continue;
		}
		else if(str[i] == substr[j]) {
			while((str[i] == substr[j]) && (j < substrlen)) {
				i++;
				j++;
			}
			if(j == substrlen) {
				found = 1;
				return found;
			}
			else {
				j = 0;
				i--;
			}
		}
		i++;
	}
	return 0;
}

