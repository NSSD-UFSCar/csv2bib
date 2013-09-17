//Evelin Soares

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 350

typedef struct article {
	char label[MAX/2];
	char author[MAX];
	char title[MAX];
	char journal[MAX];
	char year[5];
} Article;

typedef struct {
        char name[MAX];
        int label;
} Author;

int MakeBibTex (char *Arq, Article ar); // returns 1 if there is a problem to create a file 
char *firstAuthor(char *authors);

void CleanMemory(FILE *arq, Author *autor, char *aux);
void FreeVec(void *ptr);

int main (int argc, char *argv[]) {
	char c, delim, name_bib[256], string[MAX];
	char aux[MAX] = "", label[MAX/2], *p, *p_author = NULL;
	int cont = 0, tab_cont = 0, numArticles = 0, i;
	
	Article ar;
	Author *authors = NULL;
	FILE *Arq;
	
	if ( argc >= 4 ) {		
		if ( ( Arq = fopen(argv[1], "r") ) == NULL ) {
			printf("\nthe file .csv didn't found!\nClosing program...\n");
			return 1;
		}
		
		//It numerates the lines before, to know how many articles will be necessary and then allocattes sufficient positions to authors[] 
		while ( ! feof(Arq) ) {
			c = fgetc(Arq);
			if ( c == '\n' ) {
				numArticles++;
			}
		}
		rewind(Arq);
		
		if ( ( authors = calloc(numArticles, sizeof(Author) ) ) == NULL ) {
			printf("\nInsufficient memory to continue the execution of the program!\n");
			CleanMemory(Arq, authors, aux);
			exit(4);
		}
		numArticles = 0;
        		
		strcpy(name_bib, argv[2]);
		
		if ( ! ( strcmp(argv[3], "\t") ) )
			delim = '\t';
		else
			delim = argv[3][0];
	} else {
		printf("\nError!\nUse of the program: %s \"arq_origin.csv\" \"arq_destiny.bib\" delimiter(\";\" \"\\t\")\n", argv[0]);
		return 1;
	}
	    
	while ( ! feof(Arq) ) {
		c = fgetc( Arq );
		if ( c != delim && c != '\n' ) {
			string[cont++] = c;
		} else {
			string[cont] = '\0';
			tab_cont++;
			cont = 0;
			
			switch (tab_cont) {
				case 1:
					strcpy(ar.title, string);
					break;
				case 2: 
					strcpy(ar.author, string);
					break;
				case 3:
					strcpy(ar.year, string);
					break;
				case 4:
					strcpy(ar.journal, string);
					tab_cont = 0;
					p = NULL;
					numArticles++;
					
					if ( strlen(ar.author) > 0 ) {
						p_author = firstAuthor(ar.author);
						
						int achou = 0;
						for ( i = 0; i < numArticles - 1; i++ ) {
							if ( strstr(authors[i].name, p_author) != NULL ) {
								achou = 1;
								break;
							}
						}
						if ( achou ) {
							authors[i].label++;
						} else {
							i = numArticles - 1;
							strcpy(authors[i].name, p_author);
							authors[i].label = 1;	
						}
						
						strcpy(aux, authors[i].name);
						p = strtok(aux, ", "); //p = NULL before the start of the new article, avoid problems with adress  
						sprintf(label, "%s%d", p, authors[i].label); // printf in string
						strcpy(ar.label, label);
						
						printf("\nTitle: %s\nAuthors: %s\nYear: %s\nPublication: %s", ar.title, ar.author, ar.year, ar.journal);

						//MakeBibTex(name od the file bib, struct article with data of the processed article);
						if ( MakeBibTex(name_bib, ar) ) { // retorna 1 se houve erro!
							printf("\n It's not possible to create the file .bib! Closing program...\n");
							CleanMemory(Arq, authors, aux);
							FreeVec(p_author);
							return 1;
						}
						FreeVec(p_author); // p_author is allocated dynamically in char*firstAuthor
						
					}
					// After of identification of all fields of the article, it will read semicolon or space (.csv format)
					// and it will get out of the loop when it finds a different caracter
					// The first different caracter found will be "\n", then it get out of the loop and on the next read
					// on the first line after of "principal while" will be a caracter to record on the next title artigo
					while ( ! feof(Arq) && ( ( c = fgetc(Arq) ) == delim || c == ' ') );
					break;
			}
		}
	}
	
	printf("\nWe have analyzed %d articles\n", numArticles);
	
	system ("pause");
	
	CleanMemory(Arq, authors, aux);
	
	return 0;
}

int MakeBibTex (char *Arq, Article ar ) {
	FILE *a;
	char inproceedings = 0;

	// It will the INPROCEEDINGD when find on the read of the string: proceedins, acta, conferece, symposium. Otherwise, it will be ARTICLE.
     
	if ( ( strstr(ar.journal, "Inproceedings") != NULL ) || ( strstr(ar.journal, "Conference") != NULL ) || ( strstr(ar.journal, "Symposium") != NULL ) || ( strstr(ar.journal, " Acta ") != NULL ) ) { 
		inproceedings = 1;
	}
          
	if ( ( a = fopen(Arq, "a+") ) != NULL ) {
		if ( inproceedings ) 
			fprintf(a, "@inproceedings {%s\n", ar.label);
		else 
			fprintf(a, "@article {%s\n", ar.label);
		fprintf(a, "author = {\"%s\"},\n", ar.author);
		fprintf(a, "title = {\"%s\"},\n", ar.title);
		fprintf(a, "journal = {\"%s\"},\n", ar.journal);
		fprintf(a, "year = {\"%s\"}\n}\n\n", ar.year);
	} else {
		fclose(a);
		return 1;
	}
     
	fclose(a); 
	return 0;
}

char *firstAuthor(char *authors) {
	char *s, *name;
	int tam;
	
	if ( ( s = strstr(authors, " and") ) != NULL ) {
		tam = (strlen(authors) - strlen(s)) + 1;
		name = (char *) malloc(tam * sizeof(char));
		strncpy(name, authors, s - authors);
		name[tam-1] = '\0';
		return name;
	} else {
		if ( ( tam = strlen(authors) ) > 0 ) {
			tam++;
			name = (char *) malloc(tam * sizeof(char) );
			strcpy(name, authors);
			return name;
		}
	}
	
	return NULL;
}

void CleanMemory(FILE *arq, Author *autor, char *aux) {
	fclose(arq);
	FreeVec(autor);
	FreeVec(aux);
}

void FreeVec(void *ptr) {
     free(ptr);
     ptr = NULL; //  to avoid "dangling pointers"
}
