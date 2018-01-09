#include "util.h"

uint64_t atoi(char* str) {
	uint64_t int_frm_str = 0;
	int i = 0;
	for(i = 0; str[i] != '\0'; i++) {
		if((str[i] < '0') || (str[i] > '9')) {
			return -1;
		}
		else {
			int_frm_str = int_frm_str*10 + (str[i] - '0');
		}
	}
	return int_frm_str;
}

uint64_t pow(uint64_t x, int e) {
	if(e == 0) {
		return 1;
	}
	return (x*pow(x, e-1));
}

uint64_t oct_to_dec(char* oct) {
	uint64_t dec = 0, i = 0;
	for(i = 0; oct[i] != '\0'; i++) {
		dec = dec<<3 | (oct[i] - '0');
	}
	return ((uint64_t)dec);
}

void memset(char* startAddress,  char setter, int numberOfBytes)
{
	char* temp = startAddress;
	int i = 0;
	while(i<numberOfBytes){
		*temp = setter;
		temp+=1;
		i++;
	}
}

int charToInt(char chan_num){
	return chan_num - 48;
}
