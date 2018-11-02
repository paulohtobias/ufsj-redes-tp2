#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    char nome[200];

    void (*set_dados)();
}Object;
void osd(void *vo, void *nome){ //função da "classe" objeto
    printf("OOO\n");
}

typedef struct{ //extende Object
    Object obj; //Aqui é o "extends". Eu queria declarar uma referencia mas só funcionou aqui...
    int a1;
    int a2;
    int a3;
}AAA;
void asd(void *va, void *a1){ //função que vai sobrescrever a função da classe pai (osd)
    printf("AAA\n");
}

typedef struct{ //extende AAA
    AAA a; //...Se eu declaro esse como referencia o metodo de casting que eu tinha feito nao funcionava pra esse.
    double b1;
}BBB;
void bsd(void *vb, void *b1){ //função que vai sobrescrever a função da classe pai (asd)
    printf("BBB\n");
}

//função generica que chama a função certa pra qualquer objeto que "herda" da classe Object
void set(void *raw, void *data){
    //printf("set\n  r: %p\n", raw);
    
    //Casting
    Object *obj = malloc(sizeof(Object));
    memcpy(obj, raw, sizeof(Object));
    
    //printf("  o2: %p | st: %p\n\n", obj, obj->set_dados);
    
    //Chamada da função
    obj->set_dados(raw, data);
    
}

int main(){
    Object *o = malloc(sizeof(Object));
    o->set_dados = osd;
    
    AAA *a = malloc(sizeof(AAA));
    a->obj.set_dados = asd; //Essa atribuiçao vai ser feitas no "construtor" da classe AAA
    int n = 22;
    
    BBB *b = malloc(sizeof(BBB));
    b->a.obj.set_dados = bsd; //Essa atribuiçao vai ser feitas no "construtor" da classe BBB
    double m = 1.234;
    
    
    //A chamada das funções é a mesma pros tres dados. A saida é diferente pra cada um.
    set((void*)o, (void *)"paulo");
    set((void*)a, (void*)&n);
    set((void*)b, (void*)&m);

    return 0;
}
