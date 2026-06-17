# Projetos da Disciplina de Estrutura de Dados

**Universidade Federal de Alagoas (UFAL)**
**Curso:** Ciência da Computação
**Disciplina:** Estrutura de Dados

## Descrição

Este repositório reúne os trabalhos desenvolvidos durante a disciplina de Estrutura de Dados do curso de Ciência da Computação da Universidade Federal de Alagoas (UFAL).

Os projetos contemplam diferentes aplicações de estruturas de dados, algoritmos de compressão e técnicas de resolução de problemas de satisfatibilidade, proporcionando uma abordagem prática dos conceitos estudados ao longo da disciplina.

## Autores

* Akyris Germano M. dos Santos
* Calebe de Oliveira Urbano
* Mayara Lins dos Santos

## Trabalhos Desenvolvidos

### 1. Índices em Bancos de Dados com Árvore B+

Seminário dedicado ao estudo das Árvores B+, uma das principais estruturas utilizadas na implementação de índices em sistemas de gerenciamento de bancos de dados. O trabalho aborda suas propriedades, funcionamento e aplicações em operações de busca, inserção e remoção de registros.

### 2. Comparação de Desempenho entre Lista Encadeada e Árvore Binária de Busca

Implementação e análise comparativa entre uma Lista Encadeada e uma Árvore Binária de Busca (ABB). O projeto inclui a realização de experimentos, a contagem de operações executadas durante buscas e a geração de gráficos para avaliação de desempenho das estruturas.

### 3. Algoritmo de Huffman

Implementação do algoritmo de Huffman para compressão de dados. O projeto contempla a construção da árvore de Huffman, a geração dos códigos binários e a análise da eficiência da técnica na redução do espaço de armazenamento.

### 4. SAT Solver

Desenvolvimento de um solucionador para o problema da satisfatibilidade booleana (SAT). A aplicação recebe fórmulas lógicas em Forma Normal Conjuntiva (CNF) e determina se existe uma atribuição de valores capaz de satisfazer todas as cláusulas da fórmula.

#### Funcionamento das Entradas

O programa lê arquivos de texto no formato padrão **DIMACS CNF**, que é a representação clássica para problemas de satisfatibilidade. A estrutura do arquivo é dividida da seguinte forma:

1. **Linha de Cabeçalho:**
   `p cnf num_variaveis num_clausulas`
   A letra `p` indica o início da definição do problema, seguida pelo tipo `cnf`, o número total de variáveis lógicas e o número de cláusulas.

2. **Cláusulas (Forma Normal Conjuntiva):**
   Cada linha (ou conjunto de valores terminados em `0`) representa uma disjunção lógica (operações `OR`). 
   * Valores positivos representam o literal verdadeiro 
   * Valores negativos indicam a negação do literal
   * O número `0` é usado estritamente como caractere de quebra para indicar o fim de uma cláusula.

##### Exemplo de Entrada (`exemplo.cnf`)

```text
p cnf 4 5
1 2 -3 0
-1 -2 0
2 4 0
-4 0
-2 3 0
```
### 5. SMT Solver

Implementação de um SMT Solver (*Satisfiability Modulo Theories*), estendendo os conceitos de SAT para lidar com restrições pertencentes a teorias específicas, como relações aritméticas e expressões envolvendo variáveis inteiras.

#### Funcionamento das Entradas

O SMT Solver recebe arquivos de texto que estendem o formato tradicional DIMACS CNF para incorporar as restrições aritméticas. A estrutura do arquivo é dividida em três partes principais:

1. **Linha de Cabeçalho:**
   `p smt numvariaveis numclausulas`
   Identifica o tipo de problema (`smt`), seguido pelo número de variáveis booleanas (`numvariaveis`) e pelo número total de cláusulas lógicas (`numclausulas`).

2. **Cláusulas Proposicionais (CNF):**
   Linhas que definem a lógica booleana do problema. Cada linha representa uma cláusula composta por literais e finalizada pelo dígito `0`.

3. **Mapeamento da Teoria:**
   Linhas iniciadas com o identificador `t` que associam uma variável booleana a uma inequação linear da teoria aritmética:
   `t idliteral coeficientes operador constante`

##### Exemplo de Entrada

```text
p smt 2 2
1 0
2 0
t 1 2 1 <= 9
t 2 1 -2 >= 1

## Tecnologias Utilizadas
```
* Linguagem C
* Estruturas de Dados
* Árvores de Busca
* Algoritmos de Compressão
* Lógica Proposicional
* SAT (Boolean Satisfiability)
* SMT (Satisfiability Modulo Theories)

## Objetivo

O objetivo deste repositório é documentar os projetos desenvolvidos ao longo da disciplina, servindo como registro acadêmico e como material de referência para estudos relacionados a estruturas de dados, algoritmos e métodos de resolução de problemas computacionais.



#### Compilação e Execução

Para compilar o código-fonte do SMT Solver, utilize um compilador C padrão (como o `gcc`). O programa foi projetado para ler os dados automaticamente de um arquivo chamado `entrada.txt` localizado no mesmo diretório do executável.

**Passo a passo:**

1. Certifique-se de ter o `gcc` instalado em sua máquina.
2. Compile o arquivo fonte (supondo que o nome do arquivo seja `smt.c`):
   ```bash
   gcc smt.c -o smt
