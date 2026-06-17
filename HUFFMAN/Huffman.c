#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

typedef struct {
    unsigned char byte;
    int frequencia;
} Conteudo;

typedef struct no {
    void *info; 
    struct no* proximo;
    struct no* esquerda;
    struct no* direita;
} No;

typedef struct {
    No * inicio;
    int tam;
} Lista;

#define TAM 256

//----------- 2 ----------//
void criar_lista(Lista * lista ){
    lista->inicio = NULL;
    lista->tam = 0;
}

// Insere os nós em uma lista encadeada de forma ordenada
void inserir_ordenado(Lista * lista, No * no){
    No * aux = NULL;
    
    Conteudo *c_no = (Conteudo*)no->info;


    if(lista->inicio == NULL){
        lista->inicio = no;
    } 
    else {
        
        Conteudo *c_inicio = (Conteudo*)lista->inicio->info;
        
    
        if(c_no->frequencia <= c_inicio->frequencia){
            no->proximo = lista->inicio;
            lista->inicio = no;
        }
        else {
            aux = lista->inicio;
            
            while(aux->proximo) {
                Conteudo *c_prox = (Conteudo*)aux->proximo->info;
                
                if(c_prox->frequencia <= c_no->frequencia) {
                    aux = aux->proximo;
                } else {
                    break; 
                }
            }
            no->proximo = aux->proximo;
            aux->proximo = no;
        }
    }
    lista->tam++;
}

void preencher_lista(unsigned int *tab, Lista * lista){
    int i;
    No * novo;
    
    for(i = 0; i < TAM; i++){
        if(tab[i] > 0){
            novo = malloc(sizeof(No));
            Conteudo *c = malloc(sizeof(Conteudo));

            if(novo && c){
                c->byte = i;
                c->frequencia = tab[i];
                
                novo->info = c; // Guarda o conteúdo dentro do nó genérico
                novo->esquerda = NULL;
                novo->direita = NULL;
                novo->proximo = NULL;
                
                inserir_ordenado(lista, novo);
            } else {
                printf("Erro ao alocar memoria em preencher_lista\n");
                break;
            }
        }
    }
}

//----------- 3 ----------//
void remove_no_inicio(Lista * lista, No ** remover){
    if (lista->inicio == NULL) {
        *remover = NULL;
        return; 
    }

    *remover = lista->inicio;
    lista->inicio = (*remover)->proximo;
    (*remover)->proximo = NULL;
    lista->tam--;
}

void arvore_huffman(Lista * lista, No ** arvore){
    No * primeiro, *segundo, * novo;

    if (lista->tam == 1) {
        remove_no_inicio(lista, &primeiro);
        novo = malloc(sizeof(No));
        Conteudo *c = malloc(sizeof(Conteudo));
        
        if (novo && c) {
            Conteudo *c_prim = (Conteudo*)primeiro->info;
            c->frequencia = c_prim->frequencia;
            c->byte = '*';
            
            novo->info = c;
            novo->esquerda = primeiro;
            novo->direita = NULL;
            novo->proximo = NULL;
            
            inserir_ordenado(lista, novo);
        }
    }

    while(lista->tam > 1 ){
        remove_no_inicio(lista, &primeiro);
        remove_no_inicio(lista, &segundo);
        
        novo = malloc(sizeof(No));
        Conteudo *c = malloc(sizeof(Conteudo));

        if(novo && c){
            Conteudo *c_prim = (Conteudo*)primeiro->info;
            Conteudo *c_seg = (Conteudo*)segundo->info;
            

            c->frequencia = c_prim->frequencia + c_seg->frequencia;
            c->byte = '*';
            
            novo->info = c;
            novo->esquerda = primeiro;
            novo->direita = segundo;
            novo->proximo = NULL;

            inserir_ordenado(lista, novo);
        } else {
            printf("Erro ao alocar memoria em arvore_huffman\n");
            break;  
        }
    }
    *arvore = lista->inicio;
}

//----------- 4 ----------//
void calcular_altura(No *raiz, int *altura) {
    if (raiz == NULL) {
        *altura = -1; 
        return;
    }

    int alt_esq, alt_dir;
    calcular_altura(raiz->esquerda, &alt_esq);
    calcular_altura(raiz->direita, &alt_dir);
    
    if (alt_esq > alt_dir) {
        *altura = alt_esq + 1;
    } else {
        *altura = alt_dir + 1;
    }
}

void aloca_dicionario(char *** dicionario, int colunas){
    *dicionario = malloc(TAM * sizeof(char*));

    if (*dicionario == NULL) {
        printf("Erro ao alocar memoria para o dicionario!\n");
        return;
    }

    for (int i = 0; i < TAM; i++) {
        (*dicionario)[i] = calloc(colunas, sizeof(char)); 
    }
}

void gerar_dicionario(char ** dicionario, No * raiz, char * caminho, int colunas){
    if (raiz == NULL) return;
    char esquerda[colunas], direita[colunas];

    if(raiz->esquerda == NULL && raiz->direita == NULL){
        Conteudo *c = (Conteudo*)raiz->info;
        strcpy(dicionario[c->byte], caminho);
    } else {
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);

        strcat(esquerda, "0");
        strcat(direita, "1");

        gerar_dicionario(dicionario, raiz->esquerda, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->direita, direita, colunas);
    }
}


//----------- 5 ----------//
void calcular_tamanho_arvore(No *raiz, int *tamanho) {
    if (raiz == NULL) {
        return; 
    }
    
    Conteudo *c = (Conteudo*)raiz->info;

    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        if (c->byte == '*' || c->byte == '\\') {
            *tamanho += 2; 
        } else {
            *tamanho += 1; 
        }  
    } else {
    *tamanho += 1;
    }
    calcular_tamanho_arvore(raiz->esquerda, tamanho);
    calcular_tamanho_arvore(raiz->direita, tamanho);
}

void salvar_arvore_arquivo(No *raiz, FILE *arquivo) {
    if (raiz == NULL) {
        return;
    }

    Conteudo *c = (Conteudo*)raiz->info;

    if (raiz->esquerda == NULL && raiz->direita == NULL) {
        if (c->byte == '*' || c->byte == '\\') {
            unsigned char escape = '\\';
            fwrite(&escape, 1, 1, arquivo);
        }
        fwrite(&(c->byte), 1, 1, arquivo);
    } 
    else {
        unsigned char asterisco = '*';
        fwrite(&asterisco, 1, 1, arquivo);
    
        salvar_arvore_arquivo(raiz->esquerda, arquivo);
        salvar_arvore_arquivo(raiz->direita, arquivo);
    }
}

void compactar(const char *nome_arquivo_entrada, char **dicionario, No *raiz, int total_bits) {
    char nome_saida[256];
    sprintf(nome_saida, "%s.huff", nome_arquivo_entrada);

    FILE *entrada = fopen(nome_arquivo_entrada, "rb");
    FILE *saida = fopen(nome_saida, "wb");

    if (entrada == NULL || saida == NULL) {
        printf("Erro ao abrir os arquivos em compactar.\n");
        return;
    }

    int lixo = (8 - (total_bits % 8)) % 8;
    int tamanho_arvore = 0;
    calcular_tamanho_arvore(raiz, &tamanho_arvore);

    unsigned char byte1 = (lixo << 5) | (tamanho_arvore >> 8);
    unsigned char byte2 = tamanho_arvore & 255;
    fwrite(&byte1, 1, 1, saida);
    fwrite(&byte2, 1, 1, saida);

    salvar_arvore_arquivo(raiz, saida);

    unsigned char byte_atual = 0;
    int contador_bits = 0;
    unsigned char buffer;

    while (fread(&buffer, 1, 1, entrada) == 1) {
        char *codigo = dicionario[buffer];
        for (int i = 0; codigo[i] != '\0'; i++) {
            if (codigo[i] == '1') {
                byte_atual = byte_atual | (1 << (7 - contador_bits));
            }
            contador_bits++;

            if (contador_bits == 8) {
                fwrite(&byte_atual, 1, 1, saida);
                byte_atual = 0;
                contador_bits = 0;
            }
        }
    }
    if (contador_bits > 0) {
        fwrite(&byte_atual, 1, 1, saida);
    }

    fclose(entrada);
    fclose(saida);
    printf("\nArquivo compactado com sucesso: %s\n", nome_saida);
}

// -------- 6 -------- //
void bit_esta_ativo(unsigned char byte, int i, int *ativo) {
    *ativo = (byte >> (7 - i)) & 1;
}

void reconstruir_arvore(unsigned char *buffer, int *indice, No **no_atual) {
    unsigned char byte = buffer[*indice];
    (*indice)++;

    *no_atual = malloc(sizeof(No));
    Conteudo *c = malloc(sizeof(Conteudo));
    (*no_atual)->info = c;
    (*no_atual)->proximo = NULL;

    if (byte == '\\') {
        byte = buffer[*indice]; 
        (*indice)++;
        c->byte = byte;
        (*no_atual)->esquerda = NULL;
        (*no_atual)->direita = NULL;
    }
    else if (byte == '*') {
        c->byte = '*';
        reconstruir_arvore(buffer, indice, &((*no_atual)->esquerda));
        reconstruir_arvore(buffer, indice, &((*no_atual)->direita));
    }
    else {
        c->byte = byte;
        (*no_atual)->esquerda = NULL;
        (*no_atual)->direita = NULL;
    }
}

void descompactar(const char *nome_arquivo_entrada) {
    FILE *entrada = fopen(nome_arquivo_entrada, "rb");
    if (entrada == NULL) {
        printf("Erro ao abrir o arquivo compactado: %s\n", nome_arquivo_entrada);
        return;
    }

    char nome_saida[256];
    char *ponto = strrchr(nome_arquivo_entrada, '.');
    
    if (ponto && strcmp(ponto, ".huff") == 0) {
        size_t len = ponto - nome_arquivo_entrada;
        strncpy(nome_saida, nome_arquivo_entrada, len);
        nome_saida[len] = '\0'; 
        
        char *extensao_original = strrchr(nome_saida, '.');
        if (extensao_original) {
            char aux_ext[50];
            strcpy(aux_ext, extensao_original); 
            *extensao_original = '\0';          
            strcat(nome_saida, "_restaurado");   
            strcat(nome_saida, aux_ext);        
        } else {
            strcat(nome_saida, "_restaurado");
        }
    } else {
        sprintf(nome_saida, "%s.unhuf", nome_arquivo_entrada);
    }

    FILE *saida = fopen(nome_saida, "wb"); 
    if (saida == NULL) {
        printf("Erro ao criar o arquivo de saida descompactado.\n");
        fclose(entrada);
        return;
    }

    unsigned char byte1, byte2;
    if (fread(&byte1, 1, 1, entrada) != 1 || fread(&byte2, 1, 1, entrada) != 1) {
        printf("Arquivo corrompido ou formato inválido.\n");
        fclose(entrada);
        fclose(saida);
        return;
    }

    int lixo = byte1 >> 5; 
    int tamanho_arvore = ((byte1 & 31) << 8) | byte2;

    unsigned char *buffer_arvore = malloc(tamanho_arvore * sizeof(unsigned char));
    if (buffer_arvore == NULL) {
        printf("Erro de memória ao alocar o buffer da árvore.\n");
        fclose(entrada);
        fclose(saida);
        return;
    }
    
    fread(buffer_arvore, 1, tamanho_arvore, entrada);

    No *raiz_arvore = NULL;
    int indice = 0;
    reconstruir_arvore(buffer_arvore, &indice, &raiz_arvore);
    free(buffer_arvore);

    unsigned char byte_atual;
    No *no_atual = raiz_arvore;
    int ativo = 0; 

    if (fread(&byte_atual, 1, 1, entrada) == 1) {
        unsigned char proximo_byte;
        
        while (fread(&proximo_byte, 1, 1, entrada) == 1) {
            for (int i = 0; i < 8; i++) {
                bit_esta_ativo(byte_atual, i, &ativo);
                
                if (ativo) {
                    no_atual = no_atual->direita;
                } else {
                    no_atual = no_atual->esquerda;
                }

                if (no_atual->esquerda == NULL && no_atual->direita == NULL) {
                    Conteudo *c = (Conteudo*)no_atual->info;
                    fwrite(&(c->byte), 1, 1, saida);
                    no_atual = raiz_arvore; 
                }
            }
            byte_atual = proximo_byte;
        }

        for (int i = 0; i < (8 - lixo); i++) {
            bit_esta_ativo(byte_atual, i, &ativo);
            
            if (ativo) {
                no_atual = no_atual->direita;
            } else {
                no_atual = no_atual->esquerda;
            }

            if (no_atual->esquerda == NULL && no_atual->direita == NULL) {
                Conteudo *c = (Conteudo*)no_atual->info;
                fwrite(&(c->byte), 1, 1, saida);
                no_atual = raiz_arvore;
            }
        }
    }

    fclose(entrada);
    fclose(saida);
    printf("Arquivo descompactado com sucesso: %s\n", nome_saida);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso correto:\n");
        printf("  Para compactar:    %s c <nome_do_arquivo>\n", argv[0]);
        printf("  Para descompactar: %s d <nome_do_arquivo>\n", argv[0]);
        return 1;
    }
    char *opcao = argv[1];
    char *nome_arquivo = argv[2];

    if(strcmp(opcao, "c") == 0) {
        FILE *f = fopen(argv[2], "rb");
        if (f == NULL) {
            printf("Erro ao abrir o arquivo %s para leitura.\n", argv[2]);
            return 1;
        }

        unsigned char buffer;
        unsigned int freq[TAM] = {0};

        while (fread(&buffer, 1, 1, f) == 1) {
            freq[buffer]++;
        }
        fclose(f);

        Lista lista;
        criar_lista(&lista);
        preencher_lista(freq, &lista);

        No* arvore;
        arvore_huffman(&lista, &arvore);

        char ** dicionario;
        int altura = 0;

        calcular_altura(arvore, &altura);
        int colunas = altura + 1;
        aloca_dicionario(&dicionario, colunas);
        gerar_dicionario(dicionario, arvore, "", colunas);

        int tam = 0;
        for (int i = 0; i < TAM; i++) {
            if (freq[i] > 0) {
                tam += freq[i] * strlen(dicionario[i]);
            }
        }

        compactar(argv[2], dicionario, arvore, tam);
    } else if(strcmp(opcao, "d") == 0) {
        descompactar(nome_arquivo);
    }
    
    return 0;
}
