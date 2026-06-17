#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define SAT 1
#define UNSAT 0
#define UNDEFINED 2

// Estrutura de Inequação adaptada para o formato posicional limpo
typedef struct {
    int atomID;         // ID da variável booleana correspondente (ex: 1 para f1)
    int a;              // Coeficiente angular (ex: 2)
    int b;              // Coeficiente linear / deslocamento com sinal (ex: +1 ou -2)
    char op[3];         // Operador ("<=", "<", ">=", ">")
    int c;              // Constante alvo do lado direito (ex: 9)
} Inequacao;

// Estruturas para o verdadeiro motor de cláusulas SAT (CNF)
typedef struct {
    int literais[20];   // Lista de literais na cláusula (ex: {1, -2} significa x1 v ~x2)
    int qtd;            // Quantidade de literais nesta cláusula
} Clausula;

typedef struct {
    int num_variaveis;
    int num_clausulas;
    Clausula clausulas[100];
} CNF;

typedef struct Arvr {
    int *atribuido;  
    struct Arvr *esq, *dir;
} Arvr;

int min_busca = INT_MIN;
int max_busca = INT_MAX;


int DIVteto(int numerador, int denominador) {
    return numerador / denominador + (((numerador ^ denominador) > 0) && (numerador % denominador != 0));
}

int DIVchao(int numerador, int denominador) {
    return numerador / denominador - (((numerador ^ denominador) < 0) && (numerador % denominador != 0));
}

static void negar(char op[3]) {
    if      (strcmp(op, "<=") == 0) strcpy(op, ">");
    else if (strcmp(op, "<")  == 0) strcpy(op, ">=");
    else if (strcmp(op, ">=") == 0) strcpy(op, "<");
    else if (strcmp(op, ">")  == 0) strcpy(op, "<=");
}

static void inverter(char op[3]) {
    if      (strcmp(op, "<=") == 0) strcpy(op, ">=");
    else if (strcmp(op, "<")  == 0) strcpy(op, ">");
    else if (strcmp(op, ">=") == 0) strcpy(op, "<=");
    else if (strcmp(op, ">")  == 0) strcpy(op, "<");
}

// --- AVALIADOR DO MOTOR SAT ---
// Analisa se a atribuição booleana atual viola as regras lógicas passadas no arquivo
int verificar(CNF *cnf, int atribuido[]) {
    int tudo_ok = 1;

    for (int i = 0; i < cnf->num_clausulas; i++) {
        int SATisfeita = 0;
        int indefinida = 0;

        for (int j = 0; j < cnf->clausulas[i].qtd; j++) {
            int literal = cnf->clausulas[i].literais[j];
            int indice_variavel = abs(literal);
            int valor_atribuido = atribuido[indice_variavel];

            if (valor_atribuido == 0) {
                indefinida = 1; // Variável ainda não decidida pelo Backtracking
            } else if ((literal > 0 && valor_atribuido == 1) || (literal < 0 && valor_atribuido == -1)) {
                SATisfeita = 1;
                break; // Se um literal for verdadeiro, a cláusula inteira já está salva
            }
        }

        if (!SATisfeita) {
            if (!indefinida) {
                return UNSAT; 
            }
            tudo_ok = 0; 
        }
    }
    return tudo_ok ? SAT : UNDEFINED;
}

// --- ASSISTENTE DA TEORIA LIA ---
int LIA(Inequacao problema[], int num_inequacoes, int atribuido[], int *lim_inf_final, int *lim_sup_final) {
    int limite_inferior = min_busca;
    int limite_superior = max_busca;

    for (int i = 0; i < num_inequacoes; i++) {
        int varID = problema[i].atomID;
        int valor_atribuido = atribuido[varID];

        if (valor_atribuido == 0) continue; // Ignora se o SAT ainda não tomou uma decisão sobre ela

        int a = problema[i].a;
        int c = problema[i].c;
        char op[3];
        strcpy(op, problema[i].op);

        int c_normalizado = c - problema[i].b;

        
        if (valor_atribuido == -1) { 
            negar(op); 
        }

        // Trata coeficiente de x negativo
        if (a < 0) {
            a = -a;
            c_normalizado = -c_normalizado;
            inverter(op);
        }

        // Aplica o estreitamento de intervalos com base no operador ativo
        if (strcmp(op, "<=") == 0) {
            int limit = DIVchao(c_normalizado, a);
            if (limit < limite_superior) limite_superior = limit;
        } 
        else if (strcmp(op, "<") == 0) {
            int limit = DIVchao(c_normalizado - 1, a);
            if (limit < limite_superior) limite_superior = limit;
        } 
        else if (strcmp(op, ">=") == 0) {
            int limit = DIVteto(c_normalizado, a);
            if (limit > limite_inferior) limite_inferior = limit;
        } 
        else if (strcmp(op, ">") == 0) {
            int limit = DIVteto(c_normalizado + 1, a);
            if (limit > limite_inferior) limite_inferior = limit;
        }
    }

    *lim_inf_final = limite_inferior;
    *lim_sup_final = limite_superior;

    if (limite_inferior > limite_superior) return UNSAT; // Conflito matemático!
    return SAT; 
}


int SMT(Arvr *no, CNF *cnf, Inequacao problema[], int num_inequacoes, int solucao[], int *lim_inf_final, int *lim_sup_final) {
    // 1. Checagem Lógica do SAT
    int status_sat = verificar(cnf, no->atribuido);
    if (status_sat == UNSAT) return UNSAT; 

    // 2. Checagem Matemática do LIA (Poda por Teoria)
    int estado_lia = LIA(problema, num_inequacoes, no->atribuido, lim_inf_final, lim_sup_final);
    if (estado_lia == UNSAT) return UNSAT; 

    // Encontra a próxima variável livre
    int variavel_livre = -1;
    for (int i = 1; i <= cnf->num_variaveis; i++) {
        if (no->atribuido[i] == 0) {
         variavel_livre = i; 
         break; 
        }

    }

    // Se todas as decisões foram tomadas com sucesso
    if (variavel_livre == -1) {
        for (int i = 1; i <= cnf->num_variaveis; i++) solucao[i] = no->atribuido[i];
        return SAT; 
    }

    
    no->dir = malloc(sizeof(Arvr));
    no->dir->atribuido = malloc((cnf->num_variaveis + 1) * sizeof(int));
    for (int i = 1; i <= cnf->num_variaveis; i++) no->dir->atribuido[i] = no->atribuido[i];
    no->dir->esq = no->dir->dir = NULL;
    no->dir->atribuido[variavel_livre] = 1;
    if (SMT(no->dir, cnf, problema, num_inequacoes, solucao, lim_inf_final, lim_sup_final) == SAT) return SAT;

    free(no->dir->atribuido);
    free(no->dir);
    no->dir = NULL;

    no->esq = malloc(sizeof(Arvr));
    no->esq->atribuido = malloc((cnf->num_variaveis + 1) * sizeof(int));
    for (int i = 1; i <= cnf->num_variaveis; i++) no->esq->atribuido[i] = no->atribuido[i];
    no->esq->esq = no->esq->dir = NULL;
    no->esq->atribuido[variavel_livre] = -1;
    if (SMT(no->esq, cnf, problema, num_inequacoes, solucao, lim_inf_final, lim_sup_final) == SAT) return SAT;

    free(no->esq->atribuido);
    free(no->esq);
    no->esq = NULL;

    return UNSAT;
}

int main() {
    CNF cnf;
    Inequacao problema[100];
    int num_inequacoes = 0;

    FILE *arq = fopen("entrada.txt", "r");
    if (!arq) {
        printf("Erro ao abrir arquivo entrada.txt\n");
        return 1;
    }

    char p_str[10], smt_str[10];
    if (fscanf(arq, "%s %s %d %d", p_str, smt_str, &cnf.num_variaveis, &cnf.num_clausulas) != 4) {
        printf("Erro ao ler cabecalho DIMACS.\n");
        fclose(arq);
        return 1;  
    }

    // 2. LER CLÁUSULAS SAT (Terminadas por 0)
    for (int i = 0; i < cnf.num_clausulas; i++) {
        int literal;
        cnf.clausulas[i].qtd = 0;
        while (fscanf(arq, "%d", &literal) == 1 && literal != 0) {
            cnf.clausulas[i].literais[cnf.clausulas[i].qtd++] = literal;
        }
    }

    // 3. LER RESTRIÇÕES DA TEORIA LIA (Linhas iniciadas com 't')
    char tipo_linha;
    while (fscanf(arq, " %c", &tipo_linha) == 1) {
        if (tipo_linha == 't') {
            fscanf(arq, "%d %d %d %2s %d", 
                   &problema[num_inequacoes].atomID,
                   &problema[num_inequacoes].a,
                   &problema[num_inequacoes].b,
                   &problema[num_inequacoes].op,
                   &problema[num_inequacoes].c);
            num_inequacoes++;
        }
    }
    fclose(arq);

    // Inicialização da Árvore de Decisão
    Arvr *raiz = malloc(sizeof(Arvr));
    raiz->atribuido = calloc((cnf.num_variaveis + 1), sizeof(int));
    raiz->esq = raiz->dir = NULL;

    int solucao[101];
    int lim_inf_final = min_busca, lim_sup_final = max_busca;

    if (SMT(raiz, &cnf, problema, num_inequacoes, solucao, &lim_inf_final, &lim_sup_final) == SAT) {
        printf("RESULTADO: SAT\n");
        printf("Configuracao Booleana Valida:\n");
        for(int i = 1; i <= cnf.num_variaveis; i++) {
            printf("  f%d = %s\n", i, solucao[i] == 1 ? "True" : "False");
        }
        printf("\nIntervalo de Intersecao Matematica: [%d, %d]\n", lim_inf_final, lim_sup_final);
        printf("Valores validos para x: ");
        for (int x = lim_inf_final; x <= lim_sup_final; x++) {
            printf("%d ", x);
        }
        printf("\n");
    } else {
        printf("RESULTADO: UNSAT\n");
    }

    return 0;
}
