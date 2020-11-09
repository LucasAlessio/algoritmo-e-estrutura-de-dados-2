#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>
#include <ctype.h>

struct lista {
	int valor;
	struct lista *prox;
} typedef lista;

struct nodo {
	char hashtag[40];
	struct lista *posicoes;
	struct lista *ultimo;
	struct nodo *esq;
	struct nodo *dir;
	int altura;
} typedef nodo;

typedef struct {
	unsigned long long int id;
	unsigned long long int usuario;
	char texto[280];
	char dia[10];
	char lang[5];
	char hahstags[200];
} registro;

typedef struct {
	char hashtag[40];
	char posicao[3000];
} indice;

int posicao(char palavra []) {
	return (int) tolower(palavra[0]) - 97;
}

const char* toLowerCase(char str[]) {
	for(int i = 0; str[i]; i++) {
		str[i] = tolower(str[i]);
	}
	
	return str;
}

// --- BALANCEAMENTO DA ARVORE --- //
static int max(int e, int d) {
	return e > d ? e : d;
}

static int altura(nodo *n) {
	if (n == NULL) return -1;

	return n->altura;
}

int balanceamento(nodo *n) {
	if (n == NULL) return 0;
	return altura(n->esq) - altura(n->dir);
}

nodo* nodoDeMenorValor(nodo *nodo) {
	struct nodo *atual = nodo;

	/* Loop para encontrar a folha mais a esquerda (menor valor) */
	while (atual->esq != NULL)
		atual = atual->esq;

	return atual;
}

// Funcao para girar a direita a subarvore enraizada em y
nodo* rotacao_a_direita(nodo *y) {
	nodo *x = y->esq;
	nodo *T2 = x->dir;

	// Executar rotacao
	x->dir = y;
	y->esq = T2;

	// Atualiza alturas
	y->altura = max(altura(y->esq), altura(y->dir)) + 1;
	x->altura = max(altura(x->esq), altura(x->dir)) + 1;

	// Retorna nova raiz
	return x;
}

// Funcao para girar a direita a subarvore enraizada em x
nodo* rotacao_a_esquerda(nodo *x) {
	nodo *y = x->dir;
	nodo *T2 = y->esq;

	// Executar rotacao
	y->esq = x;
	x->dir = T2;

	// Atualiza alturas
	x->altura = max(altura(x->esq), altura(x->dir)) + 1;
	y->altura = max(altura(y->esq), altura(y->dir)) + 1;

	// Retorna nova raiz
	return y;
}

nodo* _insere(nodo *raiz, char hashtag[], int posicao) {
	if (raiz == NULL) {
		raiz = (nodo *)malloc(sizeof(nodo));
		strcpy((raiz)->hashtag, hashtag);
		(raiz)->esq = (raiz)->dir = NULL;
		(raiz)->posicoes = (lista *)malloc(sizeof(lista));
		((raiz)->posicoes)->valor = posicao;
		((raiz)->posicoes)->prox = NULL;
		(raiz)->ultimo = (raiz)->posicoes;
		(raiz)->altura = 0;
	}
	else {
		if (strcmp((raiz)->hashtag, hashtag) > 0) {
			raiz->esq = _insere(raiz->esq, hashtag, posicao);
		}
		else {
			if (strcmp(raiz->hashtag, hashtag) == 0) {
				lista *l = (lista *)malloc(sizeof(lista));
				l->valor = posicao;
				l->prox = NULL;
				raiz->ultimo->prox = l;
				raiz->ultimo = l;			
			}
			else {
				raiz->dir = _insere(raiz->dir, hashtag, posicao);
			}
		}
	}
	
	raiz->altura = max(altura(raiz->esq), altura(raiz->dir)) + 1;
	int balanco = balanceamento(raiz);
	
	// Rotacao simples a direita
	if (balanco > 1 && strcmp((raiz)->esq->hashtag, hashtag) > 0)
		return rotacao_a_direita(raiz);

	// Rotacao simples a esquerda
	if (balanco < -1 && strcmp((raiz)->dir->hashtag, hashtag) < 0)
		return rotacao_a_esquerda(raiz);  

	// Rotacao dupla a direita
	if (balanco > 1 && strcmp((raiz)->esq->hashtag, hashtag) < 0) {
		raiz->esq = rotacao_a_esquerda(raiz->esq);
		return rotacao_a_direita(raiz);
	}

	// Rotacao dupla a esquerda
	if (balanco > 1 && strcmp((raiz)->dir->hashtag, hashtag) > 0) {
		raiz->dir = rotacao_a_direita(raiz->dir);
		return rotacao_a_esquerda(raiz);
	}

	// Retorna o ponteiro do nodo (caso nao tenha sofrido alteracao)
	return raiz;
}

void insere(nodo **lista, char hashtag[], int indice) {
	int pos = posicao(hashtag);
	
	lista[pos] = _insere(lista[pos], hashtag, indice);
}

void _lista_ordem(nodo **raiz) {
	if (*raiz == NULL) return;
	
	_lista_ordem(&(*raiz)->esq);
	
	printf("Nome: %s\n", (*raiz)->hashtag);
	if ((*raiz)->posicoes != NULL) {
		lista *l = (*raiz)->posicoes;
		
		while (l != NULL) {
			printf("    posicoes = %d\n", l->valor);
			
			l = l->prox;
		}
	}
	
	_lista_ordem(&(*raiz)->dir);
}

void lista_posicao_ordem(nodo **lista, char letra[]) {
	int pos = posicao(letra);
	
	_lista_ordem(&(lista[pos]));
}

void lista_todos_ordem(nodo **lista) {
	for (int i = 0; i < 26; i++) {
		_lista_ordem(&(lista[i]));
	}
}

int contaLinhas(char *arquivo) {
	FILE *f = NULL;
	char linha[800];
	int cont = 0;

	f = fopen(arquivo, "rb");

	if(f == NULL) {
		printf("Erro ao abrir o arquivo %s\n", arquivo);
		exit (0);
	} else {
		while (fscanf(f, "%[^\n]%*c", linha) == 1) {
			cont++;
		}
	}

	fclose(f);
	return(cont);
}

int contaLinhasIndice(char *arquivo) {
	FILE *f = NULL;
	indice i;
	int cont = 0;

	f = fopen(arquivo, "rb");

	if(f == NULL) {
		printf("Erro ao abrir o arquivo %s\n", arquivo);
		exit (0);
	} else {
		while (fread(&i, sizeof(indice), 1, f) == 1) {
			cont++;
		}
	}

	fclose(f);
	return(cont);
}

void _escreve_ordem(nodo **raiz, char* index) {
	FILE *fi = NULL;
	
	indice in;
	
	int i;
	char str[5];
	char posicoes[10000];
		
	if (*raiz == NULL) return;
	
	_escreve_ordem(&(*raiz)->esq, index);
		
	fi = fopen(index, "ab");	
	//fwrite(&(*raiz)->hashtag, sizeof((*raiz)->hashtag), 1, fi);
	
	strcpy(in.hashtag, (*raiz)->hashtag);
			
	if ((*raiz)->posicoes != NULL) {
		lista *l = (*raiz)->posicoes;		
		i = 0;
		strcpy(posicoes, "");
		
		while (l != NULL) {	
			itoa((l)->valor, str, 10);
			//fwrite(&str, sizeof(str), 1, fi);
			
			strcat(posicoes, str);
			
			if (l->prox != NULL)
				strcat(posicoes, ",");
			
			l = l->prox;
			i++;
		}
		
		//printf("%s\n", posicoes);
		
		strcpy(in.posicao, posicoes);
		
//		for (;i < 500; i++) {
//			fwrite(" ", sizeof(str), 1, fi);
//		}
	}
	
	fwrite(&in, sizeof(indice), 1, fi);
	
	fclose(fi);	
	_escreve_ordem(&(*raiz)->dir, index);
}

void escreve_todos_ordem(nodo **lista, char *index) {
	for (int i = 0; i < 26; i++) {
		_escreve_ordem(&(lista[i]), index);
	}
}

void criaIndice(char *registrosbin, char *index) {
	nodo *tags[26] = {};
	
	char *token;
	char hashtag[40];
	
	FILE *fa = NULL;
	FILE *fi = NULL;
	
	fa = fopen(registrosbin, "rb");
	
	fi = fopen(index, "wb");
	fclose(fi);
	
	registro r;
	
	int i = 0;
	
	if(fa == NULL) {
		printf("Erro ao abrir o arquivo %s\n", registrosbin);
		exit (0);
	}
		
	while(fread(&r, sizeof(registro), 1, fa) == 1) {		
		if (strcmp(r.hahstags, " ") != 0) {			
			token = strtok(r.hahstags, " ");
			while (token != NULL) {
				strcpy(hashtag, token);
				
				char c = hashtag[0];
				bool isDigit = (c >= '0' && c <= '9');

				if (isDigit == false) {
					insere(tags, (char*)toLowerCase(hashtag), i);
				}
				token = strtok(NULL, " ");
			}
		}
		i++;
	}
	
	escreve_todos_ordem(tags, index);
	
	fclose(fa);
}

int main() {
	setlocale(LC_ALL, "Portuguese");
	
	nodo *tags[26] = {};
	
	char *registrosarq = (char*)"tweets.txt";
	char *registrosbin = (char*)"tweets.bin";
	
	char *indicehashtagsbin = (char*)"hahstags.bin";	
	
	criaIndice(registrosbin, indicehashtagsbin);
	
	int linhas = contaLinhasIndice(indicehashtagsbin);
	
	FILE *fi = NULL;
	FILE *fp = NULL;
	fi = fopen(indicehashtagsbin, "rb");	
	
	indice in;
	registro r;
	int meio, pos;
	char procurada[40];
	bool encontrado;
	char *token;
	
	while (true) {
		printf("Insira uma hashtag para ver os Tweets: ");
		scanf("%s", &procurada);
		printf("\n");
		
		if (strcmp(procurada, "0") == 0) break;
		
		encontrado = false;
		
		int inf = 0;     // limite inferior (o primeiro índice de vetor em C é zero          )
		int sup = linhas-1; // limite superior (termina em um número a menos. 0 a 9 são 10 números)	
		
		while (inf <= sup) {
			meio = (inf + sup) / 2;
			
			fseek(fi, meio * sizeof(indice), SEEK_SET);
			fread(&in, sizeof(indice), 1, fi);
			
			if (strcmp(toLowerCase(procurada), toLowerCase(in.hashtag)) == 0) {
				encontrado = true;
				printf("\nO registro está na posição: %d;\n\n", meio);
				
				fp = fopen(registrosbin, "rb");
				
				token = strtok(in.posicao, ",");				
				while (token != NULL) {
					pos = atoi(token);
					//printf("pos %s\n", token);
					
					fseek(fp, pos * sizeof(registro), SEEK_SET);
					fread(&r, sizeof(registro), 1, fp);
								
					printf("ID: %llu\nUSUARIO: %llu\nTWEET: %s\nIDIOMA: %s\nDATA: %s\nHASHTAGS: %s\n\n", r.id, r.usuario, r.texto, r.lang, r.dia, r.hahstags);
					
					token = strtok(NULL, ",");
				}
				
				break;
			}
			
			fclose(fp);
						
			if (strcmp(toLowerCase(procurada), toLowerCase(in.hashtag)) < 0)
				sup = meio-1;
		    else
				inf = meio+1;
				
			printf("%d %s - %s\n", meio, procurada, in.hashtag);
		}
		
		if (encontrado == false)
			printf("\nRegistro não encontrado!\n\n");
	}

	fclose(fi);
}
