# 1. Carrega a biblioteca gráfica
library(ggplot2)

# 2. Lê o arquivo CSV gerado pelo programa em C
dados <- read.csv("dados_comparacao.csv")

# 3. Cria o gráfico mapeando as 3 curvas
ggplot(dados) +
  geom_line(aes(x = Sorteio, y = ListaDesordenada, color = "Lista Desordenada"), linewidth = 1) +
  geom_line(aes(x = Sorteio, y = ListaOrdenada, color = "Lista Ordenada"), linewidth = 1) +
  geom_line(aes(x = Sorteio, y = Arvore, color = "Árvore Binária (ABB)"), linewidth = 1) +
  labs(
    title = "Comparativo de Eficiência de Busca",
    subtitle = "Lista Desordenada vs. Lista Ordenada vs. Árvore de Busca Binária",
    x = "Número do Sorteio",
    y = "Quantidade de Comparações (Escala Log)",
    color = "Estrutura Utilizada"
  ) +
  theme_minimal() +
  scale_color_manual(values = c(
    "Lista Desordenada" = "#E41A1C",   # Vermelho
    "Lista Ordenada"    = "#FF7F00",   # Laranja
    "Árvore Binária (ABB)" = "#377EB8" # Azul
  )) +
  scale_y_log10() # Mantido em escala logarítmica para a ABB não virar uma linha "invisível" no zero