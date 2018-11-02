#include <stdio.h>

#define FIRST_TIPO "INT","FLOAT","CHAR"
#define FIRST_COMMAND "READ","PRINT","WHILE","FOR"
#define FIRST_TOTAL FIRST_TIPO,FIRST_COMMAND

char tokens[][30] = {FIRST_TOTAL};

int toCharMatrix(char mat[][30], int size){
	int i;
	for(i = 0; i < size; i++){
		printf("%s\n",mat[i]);
	}
	return 1;
}

int main(){
	int i;
	char conjunto_first[][30] = 
		{FIRST_TOTAL};
	
	toCharMatrix((char [][30]){FIRST_TIPO}, 
				sizeof((char [][30]){FIRST_TIPO}) / 30);
	return 0;
}
