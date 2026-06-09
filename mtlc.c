/*
 * Auteur: Ascensa Mens;
 * Nature: Code source;
 * Date: Samedi 03 juin;
 * Langage: Metallvm;
 * Version: O;
 * But: Compilation des logiciels autonomes.
 */

#include "mtlc.h"

bool quot = false;
char c, word[30];
int i, ii;
long mtl_size;
FILE *mtl, *bin;
WORD code[300];

/* Analyse lexicale */
void lex(void) {
	c = fgetc(mtl);
	if(isalnum(c) || c == '_' || quot) {
		if(c == '\"') quot = false;
		word[i] = c;
		i++;
	} else if(issym(c) || isdelim(c)) {
		if(c == '\"') quot = true;
		// Dans le cas d'un symbole réservé, il faut vérifier si un mot n'existe pas avant.
		if(word[0] != 0) {
			strcpy(code[ii].name, word);
			ii++;
			for(int n = 0; n < 30; n++) word[n] = 0;
		}
		word[0] = c;
		if(quot) i++;
		else {
			word[0] = c;
			strcpy(code[ii].name, word);
			ii++;
			for(int n = 0; n < 30; n++) word[n] = 0;
			i = 0;
		}
	} else if(isspe(c)) {
		if(word[0] != 0) {
			strcpy(code[ii].name, word);
			ii++;
			for(int n = 0; n < 30; n++) word[n] = 0;
			i = 0;
		}
		if(c == '\n' || c == ';') {
			// Prend en compte les symboles terminant une instruction dans la liste.
			word[0] = c;
			strcpy(code[ii].name, word);
			ii++;
			for(int n = 0; n < 30; n++) word[n] = 0;
			i = 0;
		}
	} else if(!isspe(c) && !feof(mtl)) {
		// Refuse tout caractère spécial.
		printf("%c = %d", c, c);
		exit(2);
	}
}


/* Analyse Syntaxique */
void syn(int index) {
	// Définition des natures des mots.
	if(isalpha(code[index].name[0])) code[index].nature = NAME;
	else if(isdigit(code[index].name[0]) || (code[index].name[0] == '\"' && code[index].name[1] != 0)) code[index].nature = VALUE;
	else if(issym(code[index].name[0])) code[index].nature = OPERATOR;
	else if(isdelim(code[index].name[0])) code[index].nature = DELIMITOR;
	else if(code[index].name[0] == '\n' || code[index].name[0] == ';') code[index].nature = INSTRUCTION;
	else exit(3);
}

/* Analyse Sémantique */
void sem(void) {
	if(!strcmp())
}


int main(void) {
	// Seulement un fichier peut être lu par le compilateur.
	mtl = fopen("codex.mtl", "r");
	bin = fopen("codex.bin", "r");
	fseek(mtl, 0, SEEK_END);
	mtl_size = ftell(mtl);
	fseek(mtl, 0, SEEK_SET);
	for(long cnt = 0; cnt < mtl_size; cnt++) lex();
	for(int n = 0; n < ii; n++) {
		syn(n);
		printf(code[n].name);
		switch(code[n].nature) {
			case 0:
				printf(" = NAME\n");
				break;
			case 1:
				printf(" = VALUE\n");
				break;             
			case 2:
				printf(" = OPERATOR\n");
				break;
			case 3:
				printf(" = DELIMITOR\n");
				break;
			case 4:
				printf("INSTRUCTION\n\n");
				break;
		}
	}
	fclose(mtl);
}