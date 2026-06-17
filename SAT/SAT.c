#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições dos máximos
#define MAX_LIT 500
#define MAX_CLAU 1000

// Definir termos
#define SAT 1
#define UNSAT 0
#define UNDEFINED 2

// Struct de uma cláusula
typedef struct Clausula {
    int literais[MAX_LIT];
    int tamanho;
} Clausula;

// Estrutura do conjunto de cláusulas (CNF)
typedef struct CNF {
    Clausula clausulas[MAX_CLAU];
    int num_clausulas;
    int num_literais;
} CNF;

// NOVA ESTRUTURA DA ÁRVORE (Agora formalmente uma Árvore Binária)
typedef struct Arvr {
    int variavel; // Qual variável este nó está decidindo
    int atribuicoes[MAX_LIT];
    struct Arvr *esq; // Ramo esquerdo: Atribuição FALSA (-1)
    struct Arvr *dir; // Ramo direito: Atribuição VERDADEIRA (1)
} Arvr;

// ========== LÓGICA DO SAT SOLVER ==========

void ler_arquivo_cnf(const char* nome_arquivo, CNF* problema) {
    FILE* arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro na abertura do arquivo CNF: %s\n", nome_arquivo);
        exit(1);
    }

    char linha[256];
    int idx_clausula = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == 'c') 
            continue;

        if (linha[0] == 'p') {
            sscanf(linha, "p cnf %d %d", &problema->num_literais, &problema->num_clausulas);
            continue;
        }

        Clausula c = {0};
        char *token = strtok(linha, " ");
        
        while (token != NULL) {
            int literal = atoi(token);
            if (literal == 0)
                break;
            c.literais[c.tamanho++] = literal;
            token = strtok(NULL, " ");
        }

        if (c.tamanho > 0) {
            problema->clausulas[idx_clausula++] = c;
        }
    }

    fclose(arquivo);
}

int verificar(CNF* cnf, int atribuido[]) {
    int tudo_ok = 1;

    for (int i = 0; i < cnf->num_clausulas; i++) {
        Clausula *clausula = &cnf->clausulas[i];
        int SATisfeita = 0;
        int indefinida = 0;

        for (int j = 0; j < clausula->tamanho; j++) {
            int literal = clausula->literais[j];
            int indice_variavel = abs(literal);
            int valor_atribuido = atribuido[indice_variavel];

            if (valor_atribuido == 0) {
                indefinida = 1;
            } else if ((literal > 0 && valor_atribuido == 1) || (literal < 0 && valor_atribuido == -1)) {
                SATisfeita = 1;
                break; 
            }
        }

        if (!SATisfeita && !indefinida) {
            return UNSAT; 
        }

        if (!SATisfeita && indefinida) {
            tudo_ok = 0; 
        }
    }

    return tudo_ok ? SAT : UNDEFINED;
}

int sat(Arvr *no, CNF *cnf, int solucao[]) {
    no->esq = NULL;
    no->dir = NULL;

    int resultado = verificar(cnf, no->atribuicoes);

    if (resultado == SAT) {
        memcpy(solucao, no->atribuicoes, sizeof(int) * (cnf->num_literais + 1));
        return SAT;
    }
    if (resultado == UNSAT) {
        return UNSAT;
    }

    int variavel_livre = -1;
    for (int i = 1; i <= cnf->num_literais; i++) {
        if (no->atribuicoes[i] == 0) {
            variavel_livre = i;
            break;
        }
    }

    if (variavel_livre == -1) {
        return UNSAT; 
    }

    no->variavel = variavel_livre;

    // TENTATIVA 1: Ramo Direito (Atribuir VERDADEIRO / 1)
    no->dir = malloc(sizeof(Arvr));
    no->dir->variavel = 0; // Será definido na próxima chamada
    memcpy(no->dir->atribuicoes, no->atribuicoes, sizeof(int) * MAX_LIT);
    no->dir->atribuicoes[variavel_livre] = 1;
    
    if (sat(no->dir, cnf, solucao) == SAT) {
        return SAT; 
    }
    
    // Poda o galho direito se falhou 
    free(no->dir);
    no->dir = NULL;

    
    no->esq = malloc(sizeof(Arvr));
    no->esq->variavel = 0;
    memcpy(no->esq->atribuicoes, no->atribuicoes, sizeof(int) * MAX_LIT);
    no->esq->atribuicoes[variavel_livre] = -1;
    
    if (sat(no->esq, cnf, solucao) == SAT) {
        return SAT; 
    }
    
    // Poda o galho esquerdo se falhou
    free(no->esq);
    no->esq = NULL;

    return UNSAT; 
}

// Função para liberar os nós da árvore criados com malloc
void liberar_arvore(Arvr *no) {
    if (no == NULL) return;
    liberar_arvore(no->esq);
    liberar_arvore(no->dir);
    free(no);
}

// ========== FUNÇÃO PRINCIPAL ==========

int main() {
    CNF problema = {0};
    char arquivo[100];
    
    printf("Digite o nome do arquivo (com extensao .cnf): ");
    fgets(arquivo, sizeof(arquivo), stdin);
    arquivo[strcspn(arquivo, "\n")] = 0;
    
    ler_arquivo_cnf(arquivo, &problema);
    
    // Inicializa a raiz da árvore
    Arvr raiz = {0};
    raiz.variavel = 0;
    raiz.esq = NULL;
    raiz.dir = NULL;
    for (int i = 0; i <= problema.num_literais; i++) {
        raiz.atribuicoes[i] = 0;
    }
    
    int solucao[MAX_LIT] = {0};
    
    // Executa o solver e imprime o resultado formatado
    if (sat(&raiz, &problema, solucao)) {
        printf("\nSAT!\n");
        for (int i = 1; i <= problema.num_literais; i++) {
            printf("%d = %d\n", i, solucao[i] == 1 ? 1 : 0);
        }
    } else {
        printf("\nUNSAT!\n");
    }
    
    // Limpeza de memória dos filhos da raiz (A raiz não usou malloc)
    liberar_arvore(raiz.esq);
    liberar_arvore(raiz.dir);
    
    return 0;
}
