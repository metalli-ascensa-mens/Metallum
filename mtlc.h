/*
 * Auteur: Ascensa Mens;
 * Nature: Bibliothèque;
 * Date: Samedi 03 juin;
 * Langage: Metallvm;
 * Version: O;
 * But: Compilation des logiciels autonomes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/*** Définitions des éléments liguistiques ***/
typedef enum tag_nature {
	NOTHING = 0,
	NAME = 1,
	FUNCTION = 2,
	TYPE = 3,
	VALUE = 4,
	OPERATOR = 5,
	DELIMITOR = 6,
	INSTRUCTION = 7,
}NAT;

/* Définition d'un lexème (mot dans une phrase) */
typedef struct tag_lexeme {
	char name[30];
	NAT nature;
	long sense;
}WORD;

/** Définition d'une variable/fonction **/
typedef struct tag_memory {
	char name[30];
	char address;
}MEM;

/*** Fonctions de reconnaissance de caractères ***/
bool issym(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|') return true;
	else if(c == '^' || c == '!' || c == '%' || c == ':' || c == '@' || c == '$') return true;
	else if(c == '<' || c == '=' || c == '>' || c == '.' || c == '\\' || c == ',') return true;
	return false;
}

bool iscomposedsym(char c1, char c2) {
	if((c1 == '+' || c1 == '-' || c1 == '*' || c1 == '/' || c1 == '&' || c1 == '|' || c1 == '~' || c1 == '%') && c2 == ':') return true;
	else if((c1 == '+' && c2 == c1) || (c1 == '-' && c2 == c1) || (c1 == '*' && c2 == c1) || (c1 == '/' && c2 == c1)) return true;
	else if((c1 == '<' && c2 == c1) || (c1 == '>' && c2 == c1) || (c1 == '~' && c2 == c1) || (c1 == '#' && c2 == c1)) return true;
	else if((c1 == '<' && c2 == '=') || (c1 == '>' && c2 == '=') || (c1 == '!' && c2 == '=')) return true;
	else if((c1 == '!' && c2 == '?') || (c1 == '?' && c2 == '!')) return true;
	return false;
}

bool isdelim(char c) {
	if(c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}' || c == '\"' || c == '\'') return true;
	return false;
}

bool isspe(char c) {
	if(c == 6 || c == '\n' || c == '	' || c == ' ' || c == ';') return true;
	return false;
}
