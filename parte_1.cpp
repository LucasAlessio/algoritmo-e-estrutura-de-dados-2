//teste
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <locale.h>

typedef struct {
	unsigned long long int id;
	unsigned long long int usuario;
	char texto[280];
	char dia[10];
	char lang[5];
	char hahstags[200];
} registro;

typedef struct {
	unsigned long long int id;
	int posicao;
} indice;

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

registro *arrayRegistro(char *arquivo, int linhas) {
	FILE *f = NULL;
	char linha[800];
	registro *registros = (registro *)malloc(sizeof(registro) * linhas);
	int cont = 0;
	char *token;

	f = fopen(arquivo, "r");

	if (f == NULL) {
		printf("Erro ao abrir o arquivo %s\n", arquivo);
		exit(0);
	}
	else {
		long int id;
		long int usuario;
		char tweet[280];
		char lang[2];
		char hahstags[200];
		
		while (fscanf(f, "%[^\n]%*c", linha) == 1) {
			token = strtok(linha, "|||");
			registros[cont].id = atoll(token);

			token = strtok(NULL, "|||");
			registros[cont].usuario = atoll(token);

			token = strtok(NULL, "|||");
			strcpy(registros[cont].texto, token);
			
			token = strtok(NULL, "|||");
			strcpy(registros[cont].dia, token);
			
			token = strtok(NULL, "|||");
			strcpy(registros[cont].lang, token);

			token = strtok(NULL, "|||");
			strcpy(registros[cont].hahstags, token);

			cont++;
		}
	}
	
	fclose(f);
	return registros;
}

void criaBinario(registro *registros, int linhas, char *arquivo) {
	FILE *fp;
	fp = fopen(arquivo, "wb");
	
	for (int i = 0; i < linhas; i++) {
		fwrite(&registros[i], sizeof(registro), 1, fp);
	}
	
	fclose(fp);
}

void criaIndice(char *arquivo, char *index) {
	FILE *fa = NULL;
	FILE *fi = NULL;
	
	registro r;
	indice in;
	
	int cont = 0;
	
	fa = fopen(arquivo, "rb");
	fi = fopen(index, "wb");

	if(fa == NULL) {
		printf("Erro ao abrir o arquivo %s\n", arquivo);
		exit (0);
	} else {
		while (fread(&r, sizeof(registro), 1, fa)) {
			in.id = r.id;
			in.posicao = cont;
			
			fwrite(&in, sizeof(indice), 1, fi);
			
			cont++;
		}
	}
	
	fclose(fi);
	fclose(fa);
}

int main() {
	setlocale(LC_ALL, "Portuguese");
	
	char *registrosarq = (char*)"tweets.txt";
	char *registrosbin = (char*)"tweets.bin";
	char *registrosind = (char*)"indice.bin";
	
	int linhas = contaLinhas(registrosarq);
	
	printf("%d\n", linhas);
	
	registro *registros = arrayRegistro(registrosarq, linhas);	
	criaBinario(registros, linhas, registrosbin);
	criaIndice(registrosbin, registrosind);	
	
	// ---------------------------------------------------
	
	FILE *fi = NULL;
	fi = fopen("indice.bin", "rb");
	FILE *fp = NULL;
	
	unsigned long long int procurado;
			
	int meio;
	registro r;
	indice in;
	bool encontrado;
	
	while (true) {
		printf("Insira o registro que deseja procurar (id): ");
		scanf("%llu", &procurado);
		printf("\n");
		
		if (procurado == 0) break;
		
		encontrado = false;
		
		int inf = 0;     // limite inferior (o primeiro índice de vetor em C é zero          )
		int sup = linhas-1; // limite superior (termina em um número a menos. 0 a 9 são 10 números)	
		
		while (inf <= sup) {
			meio = (inf + sup) / 2;
			
			fseek(fi, meio * sizeof(indice), SEEK_SET);
			fread(&in, sizeof(indice), 1, fi);
			
			if (procurado == in.id) {
				encontrado = true;
				printf("\nO registro está na posição: %d;\n\n", in.posicao);
				
				fp = fopen(registrosbin, "rb");
				fseek(fp, in.posicao * sizeof(registro), SEEK_SET);
				fread(&r, sizeof(registro), 1, fp);
								
				printf("ID: %llu\nUSUARIO: %llu\nTWEET: %s\nIDIOMA: %s\nDATA: %s\nHASHTAGS: %s\n\n", r.id, r.usuario, r.texto, r.lang, r.dia, r.hahstags);
				
				fclose(fp);
				
				break;
			}
						
			if (procurado < in.id)
				sup = meio-1;
		    else
				inf = meio+1;
				
			printf("%d %llu - %llu\n", meio, procurado, in.id);
		}
		
		if (encontrado == false)
			printf("\nRegistro não encontrado!\n\n");
	}

	fclose(fi);
	

	return 0;
}
