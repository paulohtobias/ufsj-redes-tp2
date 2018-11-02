#include <stdio.h>
#include <stdlib.h>

void preencheVetor(int *vet, int size){
	int i;
	vet = (int*) malloc(sizeof(int) * size);
	for(i=0; i < size; i++){
		vet[i] = i;
	}
	for(i=0; i < size; i++){
		printf("%d ",vet[i]);
	}
	printf("\n");
}

int main(){
	char c = 'c';
	int i;
	int size = 5;
	int *vetor;

	preencheVetor(vetor, size);

	printf("Antes do for\n");
	for(i=0; i < size; i++){
		printf("%d ",vetor[i]);
	}
	printf("Depois do for\n");

	return 0;
}