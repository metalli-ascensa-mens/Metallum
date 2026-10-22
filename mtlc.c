/*
 * Auteur: Ascensa Mens;
 * Nature: Code source;
 * Date: Samedi 03 juin 2026;
 * Langage: Metallvm;
 * Version: O;
 * But: Compilation des logiciels autonomes.
 */

#include "mtlc.h"

bool quot = false, is_writing = false;
char c, word[30];
int i, ii, oi, si, mi;
int block;
long mtl_size;
FILE *mtl, *bin;
WORD code[300], ocode[300], stack[300];
/* VARIABLES & FUNCTIONS */
int size;
MEM dict[300];
/* CODE MACHINE */
char mch[300];

/* Analyse lexicale */
void lex(void) {
	c = fgetc(mtl);
	if(isalnum(c) || c == '_' || quot) {
		if(c == '\"') quot = false;
		word[i] = c;
		i++;
	} else if(iscomposedsym(code[ii-1].name[0], c)) { 
		// Dans le cas d'un digraphe, il faut vérifier que ce dernier est valide.
		if(code[ii-1].name[1] != 0) exit(2);
		strcat(code[ii-1].name, &c);
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

/* Analyse syntaxique */
void syn(int index) {
	// Définition des natures des mots et de la priorité des opérations.
	if(isalpha(code[index].name[0])) {
		if(code[index+1].name[0] != '[') code[index].nature = NAME;
		else {
			code[index].nature = FUNCTION;
			code[index].sense = block;
		}
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
	else if(iscomposedsym(code[index].name[0], code[index].name[1])) {
		code[index].nature = OPERATOR;
		if(code[index].name[1] == ':') code[index].sense = block+1;
		else if(!strcmp(code[index].name, "++") || !strcmp(code[index].name, "--")) code[index].sense = block+9;
		else if(!strcmp(code[index].name, "**") || !strcmp(code[index].name, "//")) code[index].sense = block+9;
		else if(!strcmp(code[index].name, "<<") || !strcmp(code[index].name, ">>")) code[index].sense = block+7;
	}
	else if(issym(code[index].name[0])) {
		code[index].nature = OPERATOR;
		switch(code[index].name[0]) {
			case '+':
				code[index].sense = block+7;
				break;
			case '-':
				code[index].sense = block+7;
				break;
			case '*':
				code[index].sense = block+8;
				break;
			case '/':
				code[index].sense = block+8;
				break;
			case '^':
				code[index].sense = block+3;
				break;
			case '&':
				code[index].sense = block+4;
				break;
			case '|':
				code[index].sense = block+2;
				break;
			case '!':
				code[index].sense = block+9;
				break;
			case ':':
				code[index].sense = block+1;
				break;
			case '=':
				code[index].sense = block+5;
				break;
		}
	}
	else if(isdelim(code[index].name[0])) {
		code[index].nature = DELIMITOR;
		switch(code[index].name[0]) {
			case '(':
				block++;
				break;
			case ')':
				block--;
				break;
			case '[':
				block++;
				break;
			case ']':
				block--;
				break;
			case '{':
				block++;
				break;
			case '}':
				block--;
				break;
		}
	}
	else if(code[index].name[0] == '\n' || code[index].name[0] == ';') code[index].nature = INSTRUCTION;
	else exit(3);
	switch(code[index].nature) {
		case NAME:
			strcpy(ocode[oi].name, code[index].name);
			ocode[oi].sense = code[index].sense;
			ocode[oi].nature = code[index].nature;
			oi++;
			break;
		case VALUE:
			strcpy(ocode[oi].name, code[index].name);
			ocode[oi].sense = code[index].sense;
			ocode[oi].nature = code[index].nature;
			oi++;
			break;
		case INSTRUCTION:
			strcpy(ocode[oi].name, code[index].name);
			ocode[oi].sense = code[index].sense;
			ocode[oi].nature = code[index].nature;
			oi++;
			break;
		case OPERATOR:
			while(stack[si].sense >= code[index].sense && si > 0) {
				strcpy(ocode[oi].name, stack[si].name);
				ocode[oi].sense = stack[si].sense;
				ocode[oi].nature = stack[si].nature;
				oi++;
				strcpy(stack[si].name, "");
				stack[si].nature = NOTHING;
				stack[si].sense = 0;
				si--;
			}
			si++;
			strcpy(stack[si].name, code[index].name);
			stack[si].sense = code[index].sense;
			stack[si].nature = code[index].nature;
			break;
		case FUNCTION:
			while(stack[si].sense >= code[index].sense && si > 0) {
				strcpy(ocode[oi].name, stack[si].name);
				ocode[oi].sense = stack[si].sense;
				ocode[oi].nature = stack[si].nature;
				oi++;
				strcpy(stack[si].name, "");
				stack[si].sense = 0;
				si--;
			}
			si++;
			strcpy(stack[si].name, code[index].name);
			stack[si].sense = code[index].sense;
			stack[si].nature = code[index].nature;
			break;
		case DELIMITOR:
			break;
		default:
			exit(3);
			break;
	}
}

/* Analyse sémantique */
void sem(int index) {
	char fname[36];
	switch(ocode[index].nature) {
		case NAME:
			
			break;
		case FUNCTION:
			strcpy(fname, code[index].name);
			strcat(fname, ".mtlc");
			FILE *mcode = fopen(fname, "r");
			fclose(mcode);
			break;
	}
}

/* Compilation */
void cmp(int *index) {
	if(!strcmp(code[*index].name, "scribo") && code[(*index)+1].name[0] == '[' && code[(*index)+2].nature == VALUE && code[(*index)+3].name[0] == ']') {
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
	} else exit(4);
}

int main(int argc, char **argv) {
	mtl = fopen("codex.mtl", "r");
	bin = fopen("codex.bin", "w");
	for(long cnt = 0; !feof(mtl); cnt++) lex();
	for(int n = 0; n < ii; n++) {
		syn(n);
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
	while(si > 0) {
		strcpy(ocode[oi].name, stack[si].name);
		ocode[oi].sense = stack[si].sense;
		ocode[oi].nature = stack[si].nature;
		oi++;
		strcpy(stack[si].name, "");
		stack[si].sense = 0;
		si--;
	}
	si = 0;
	for(int n = 0; n < oi; n++) {
		for(int k = 0; k < si; k++) {
			strcpy(stack[k].name, "");
			stack[k].nature = NOTHING;
			stack[k].sense = 0;
		}
		sem(n);
	}for(int ic = 0; ic < ii; ic++) {
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
	system("C:\\Users\\LEO.DESHAYES\\qemu\\qemu.exe -hda \"C:\\Users\\LEO.DESHAYES\\Moi\\Projets\\Metallvm O\\Bin\\codex.bin\"");
}
