library(ggplot2)

dados <- read.csv("dados_comparacao.csv")

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
    "Lista Desordenada" = "#E41A1C",   
    "Lista Ordenada"    = "#FF7F00",   
    "Árvore Binária (ABB)" = "#377EB8" 
  )) +
  scale_y_log10() 
