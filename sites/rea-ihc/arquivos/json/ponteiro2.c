#include <stdlib.h>
#include <string.h>

void muda(int *b) {
	int a = 3;
	*b = 2;
}

int main()
{
	int a = 1;
	int *b;
	char c1[] = "str1";
	char *c2 = "str2";
	char c3[6];
	strcpy(c3, "str3");
	muda(&a);
	b = malloc(4 * sizeof(int));
	b[2] = 5;
	return 0;
}