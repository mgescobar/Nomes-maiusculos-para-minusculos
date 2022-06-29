#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>


typedef struct
{
  char **nomes;
  int nrlinhas;
}vetor_de_linhas;

void libera_memoria_de_linhas(vetor_de_linhas *nomes)
{
  free(nomes);
}

void grava_linhas_em_arquivo(vetor_de_linhas *nome, char *arquivo)
{
  FILE *arq;

  arq = fopen(arquivo, "w");
  if (arq == NULL) {
    printf("Problema na criacao do arquivo de saida\n");
    return;
  }
  for (int i=0; i<nome->nrlinhas; i++) {
    // as strings não têm \n no final, mas tem que ter no arquivo.
    fprintf(arq, "%s\n", nome->nomes[i]);
  }
  fclose(arq);
}

int contalinha(char *nome)
{
  FILE *arq;
  int n = 0;
  bool to_no_inicio_da_linha = true;
  arq = fopen(nome, "r");
  if (arq == NULL) {
    printf("Problema na abertura do arquivo de entrada\n");
    exit(0);
  }
  while (true) {
    int c = getc(arq);
    if (c == EOF) break;
    if (to_no_inicio_da_linha) {
      n++;
    }
    to_no_inicio_da_linha = (c == '\n');
  }
  return n;
}

char *le_linha(FILE *arq)
{
    char *lin;
    int p = 0;  // posicao na linha onde vai ser colocado o proximo caractere.
    int n = 35; // tamanho do espaço total para a linha sendo lida.
    
    lin = malloc(n);
    while (true) {
        int c = getc(arq);
        if (c == '\n') {
            // fim da linha, a string em lin está completa (falta o \0)(e sem o \n)
            break;
        }
        if (c == EOF) {
            if (p == 0) {
                // o arquivo terminou no início de uma linha (quer dizer que essa linha não existe)
                free(lin);
                return NULL;
            }
            // o arquivo terminou no meio de uma linha -- trata como se tivesse visto um \n
            break;
        }
        // foi lido um caractere normal -- coloca no final da linha que está sendo lida.
        lin[p] = c;
        p++;
        if (p >= n) {
            // a linha é maior que o espaço que temos -- realoca
            n *= 2;
            lin = realloc(lin, n);
            if (lin == NULL) {
                printf("faltou memoria");
                exit(1);
            }
        }
    }
    lin[p] = '\0';
    // lin tem n bytes alocados, a string precisa de somente p+1 bytes -- devolve o excesso pro sistema
    if (n != p+1) {
      lin = realloc(lin, p+1);
      if (lin == NULL) {
          printf("faltou memoria");
          exit(1);
      }
    }
    return lin;
}

vetor_de_linhas *le_linhas_de_arquivo(char *nome)
{
  FILE *arq;
  vetor_de_linhas *a;
  int n = contalinha(nome);

  arq = fopen(nome, "r");
  if (arq == NULL) {
    printf("Problema na abertura do arquivo de entrada\n");
    return 0;
  }
  a = malloc(sizeof(vetor_de_linhas));
  a->nomes = malloc(n * sizeof(vetor_de_linhas));
  a->nrlinhas = n;
  for (int i=0; i<n; i++) {
    char *lin = le_linha(arq);
    if (lin == NULL) {
      return a;
    }
    a->nomes[i] = lin;
  }
  fclose(arq);
  return a;
}

bool maiuscula(char c)
{
    return c >= 'A' && c <= 'Z';
}

bool minuscula(char c)
{
    return c >= 'a' && c <= 'z';
}

bool letra(char c)
{
    return maiuscula(c) || minuscula(c);
}

char vira_minuscula(char c)
{
    if (!maiuscula(c)) return c;
    return c - 'A' + 'a';
}

void minusculiza_des(char *nome)
{
    // TODO: suporte para mais partículas
    char *des[] = { "De ", "Dos ", "Da " };
    for (int i=0; i<3; i++) {
        char *de;
        do {
            de = strstr(nome, des[i]);
            if (de != NULL) {
                *de = vira_minuscula(*de);
            }
        } while (de != NULL);
    }
}

void minusculiza_linhas(vetor_de_linhas *nome)
{
  for (int i=0;i<nome->nrlinhas;i++)
  {
    char *p = nome->nomes[i];
    bool transforma = false;
    while (*p != '\0') {
      if (maiuscula(*p) && transforma) {
        *p = vira_minuscula(*p);
      }
      transforma = letra(*p);
      p++;
    }
    minusculiza_des(nome->nomes[i]);
  }
}


int main()
{
  vetor_de_linhas *nomes;

  nomes = le_linhas_de_arquivo("entrada");
  minusculiza_linhas(nomes);
  grava_linhas_em_arquivo(nomes, "saida");
  
  libera_memoria_de_linhas(nomes);

  return 0;
}