/*
 * Auteur: Ascensa Mens;
 * Nature: Code source;
 * Date: Samedi 03 juin;
 * Langage: Metallvm;
 * Version: O;
 * But: Compilation des logiciels autonomes.
 */

#include "mtlc.h"

bool quot = false, is_writing = false;
char c, word[30], mch[300];
int i, ii, mi;
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
		if((c == '\n' || c == ';') && !isspe(code[ii-1].name[0])) {
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


/* Analyse Linguistique */
void lng(int index) {
	// Définition des natures des mots.
	if(isalpha(code[index].name[0])) {
		if(index+1 < ii) code[index].nature = NAME;
		else if(code[index+1].name[0] == '[') code[index].nature = FUNCTION;
		else code[index].nature = NAME;
	}
	else if(isdigit(code[index].name[0]) || code[index].name[0] == '\"' && code[index].name[1] != 0) {
		code[index].nature = VALUE;
		if(code[index].name[0] == '\"') {
			for(int n = 1; code[index].name[n-1] != 0; ++n) {
				if(code[index].name[n+1] == 0) code[index].name[n-1] = 0;
				else code[index].name[n-1] = code[index].name[n];
				
			}
		}
	}
	else if(issym(code[index].name[0])) code[index].nature = OPERATOR;
	else if(isdelim(code[index].name[0])) code[index].nature = DELIMITOR;
	else if(code[index].name[0] == '\n' || code[index].name[0] == ';') code[index].nature = INSTRUCTION;
	else exit(3);
}

/* Compilation */
void cmp(int *index) {
	if(!strcmp(code[*index].name, "scribo") && code[(*index)+1].name[0] == '[' && code[(*index)+2].nature == VALUE && code[(*index)+3].name[0] == ']' && code[(*index)+3].name[0] == ']') {
		if(!is_writing) {
			mch[mi] = 0xB4;
			mch[mi+1] = 0x0E;
			mi += 2;
			is_writing = false;
		}
		for(int mw = 0; code[(*index)+2].name[mw] != 0; ++mw) {
			mch[mi] = 0xB0;
			mch[mi+1] = code[(*index)+2].name[mw];
			mch[mi+2] = 0xCD;
			mch[mi+3] = 0x10;
			mi += 4;
		}
		*index += 4;
	}
}


int main(void) {
	// Seulement un fichier peut être lu par le compilateur. Un seul autre peut être écrit.
	mtl = fopen("codex.mtl", "r");
	bin = fopen("codex.bin", "w");

	fseek(mtl, 0, SEEK_END);
	mtl_size = ftell(mtl);
	fseek(mtl, 0, SEEK_SET);
	for(long cnt = 0; cnt < mtl_size; cnt++) lex();
	for(int n = 0; n < ii; n++) {
		lng(n);
		printf(code[n].name);
		switch(code[n].nature) {
			case NAME:
				printf(" = NAME\n");
				break;
			case FUNCTION:
				printf(" = FUNCTION\n");
				break;
			case VALUE:
				printf(" = VALUE\n");
				break;             
			case OPERATOR:
				printf(" = OPERATOR\n");
				break;
			case DELIMITOR:
				printf(" = DELIMITOR\n");
				break;
			case INSTRUCTION:
				printf("INSTRUCTION\n\n");
				break;
		}
	}
	for(int ic = 0; ic < ii; ic++) {
		cmp(&ic);
	}
	mch[mi] = 0xEB;
	mch[mi+1] = 0xFE;
	mi += 2;
	for(int num16 = 0; num16 < 512; num16++) {
		if(num16 < mi) fputc(mch[num16], bin);
		else if(num16 == 510) {
			fputc(0x55, bin);
			fputc(0xAA, bin);
			num16++;
		} else fputc(0x0, bin);
	}

	fclose(mtl);
	fclose(bin);

	// Simulation du fichier compilé.
	system("C:\\Users\\LEO.DESHAYES\\qemu\\qemu.exe -hda \"C:\\Users\\LEO.DESHAYES\\Projets\\Metallvm O\\Bin\\codex.bin\"");
	while(1) ;
}
