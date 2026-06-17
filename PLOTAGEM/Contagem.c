#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct NoLista {
    void* dado;
    struct NoLista* proximo;
} NoLista;

typedef struct NoArvore {
    void* dado;
    struct NoArvore* esquerda;
    struct NoArvore* direita;
} NoArvore;

int comparar(void* a, void* b) {
    return (*(int*)a) - (*(int*)b);
}

NoLista* inserirListaDesordenada(NoLista* topo, void* dado) {
    NoLista* novo = (NoLista*)malloc(sizeof(NoLista));
    novo->dado = dado;
    novo->proximo = topo;
    return topo = novo;
}

int buscarListaDesordenada(NoLista* topo, void* dado, int* comparacoes) {
    NoLista* atual = topo;
    *comparacoes = 0;
    int valor_buscado = *(int*)dado;

    while (atual != NULL) {
        (*comparacoes)++;
        int valor_atual = *(int*)(atual->dado);
        
        printf("[LISTA DESORDENADA] Comparacao %d: Buscando %d vs Atual %d\n", *comparacoes, valor_buscado, valor_atual);
        
        if (comparar(atual->dado, dado) == 0) {
            return 1; 
        }
        atual = atual->proximo;
    }
    return 0; 
}

NoLista* inserirListaOrdenada(NoLista* topo, void* dado) {
    NoLista* novo = (NoLista*)malloc(sizeof(NoLista));
    novo->dado = dado;
    novo->proximo = NULL;
    
    if (topo == NULL || comparar(dado, topo->dado) < 0) {
        novo->proximo = topo;
        return novo;
    }

    NoLista* atual = topo;
    while (atual->proximo != NULL && comparar(atual->proximo->dado, dado) < 0) {
        atual = atual->proximo;
    }
    
    novo->proximo = atual->proximo;
    atual->proximo = novo;
    return topo;
}

int buscarListaOrdenada(NoLista* topo, void* dado, int* comparacoes) {
    NoLista* atual = topo;
    *comparacoes = 0;
    int valor_buscado = *(int*)dado;

    while (atual != NULL) {
        (*comparacoes)++;
        int valor_atual = *(int*)(atual->dado);
        
        printf("[LISTA ORDENADA] Comparacao %d: Buscando %d vs Atual %d\n", *comparacoes, valor_buscado, valor_atual);
        
        int comp = comparar(atual->dado, dado);
        if (comp == 0) {
            return 1; 
        }
        if (comp > 0) {
            break; 
        }
        atual = atual->proximo;
    }
    return 0; 
}

NoArvore* inserirArvore(NoArvore* raiz, void* dado) {
    if (raiz == NULL) {
        NoArvore* novo = (NoArvore*)malloc(sizeof(NoArvore));
        novo->dado = dado;
        novo->esquerda = NULL;
        novo->direita = NULL;
        return novo;
    }
    if (comparar(dado, raiz->dado) < 0) {
        raiz->esquerda = inserirArvore(raiz->esquerda, dado);
    } else {
        raiz->direita = inserirArvore(raiz->direita, dado);
    }
    return raiz;
}

int buscarArvore(NoArvore* raiz, void* dado, int* comparacoes) {
    if (raiz == NULL) return 0;
    
    (*comparacoes)++;
    int valor_buscado = *(int*)dado;
    int valor_atual = *(int*)(raiz->dado);
    int comp = comparar(dado, raiz->dado);
    
    printf("[ARVORE] Comparacao %d: Buscando %d vs Atual %d\n", *comparacoes, valor_buscado, valor_atual);
    
    if (comp == 0) return 1;
    if (comp < 0) return buscarArvore(raiz->esquerda, dado, comparacoes);
    return buscarArvore(raiz->direita, dado, comparacoes);
}

int main() {
    srand(time(NULL));
    
    NoLista* lista_desordenada = NULL;
    NoLista* lista_ordenada = NULL;
    NoArvore* arvore = NULL;
    
    int n_elementos = 5000; 
    int n_sorteios = 100;    
    
    printf("Inserindo elementos... (A lista ordenada pode demorar alguns segundos)\n");
    for (int i = 0; i < n_elementos; i++) {
        int* valor = (int*)malloc(sizeof(int));
        *valor = rand() % 100000;
        
        lista_desordenada = inserirListaDesordenada(lista_desordenada, valor);
        lista_ordenada = inserirListaOrdenada(lista_ordenada, valor);
        arvore = inserirArvore(arvore, valor);
    }
    
    FILE* arquivo = fopen("dados_comparacao.csv", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo CSV!\n");
        return 1;
    }
 
    fprintf(arquivo, "Sorteio,ListaDesordenada,ListaOrdenada,Arvore\n");
    
    printf("Iniciando buscas...\n");
    for (int i = 1; i <= n_sorteios; i++) {
        int numero_sorteado = rand() % 100000;
        
        int comp_lista_d = 0;
        int comp_lista_o = 0;
        int comp_arvore = 0;
        
        printf("\n=== SORTEIO %d: Procurando o numero %d ===\n", i, numero_sorteado);
        
        buscarListaDesordenada(lista_desordenada, &numero_sorteado, &comp_lista_d);
        buscarListaOrdenada(lista_ordenada, &numero_sorteado, &comp_lista_o);
        buscarArvore(arvore, &numero_sorteado, &comp_arvore);
        
        fprintf(arquivo, "%d,%d,%d,%d\n", i, comp_lista_d, comp_lista_o, comp_arvore);
    }
    
    fclose(arquivo);
    printf("\nSucesso! Arquivo 'dados_comparacao.csv' gerado.\n");
    
    return 0;
}
