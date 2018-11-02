int main(){
	int a = 1;
	int *b = &a;
	a++;
	(*b)++;
	int c;
	c = 2;
	return 0;
}