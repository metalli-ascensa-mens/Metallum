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
	VARIABLE = 0,
	POINTER = 1,
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

/*** Fonctions de reconnaissance de caractères ***/
bool issym(char c) {
	if(c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|') return true;
	else if(c == '^' || c == '!' || c == '%' || c == ':' || c == '@' || c == '$') return true;
	else if(c == '<' || c == '=' || c == '>' || c == '.' || c == '\\' || c == ',') return true;
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