#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

typedef enum {
    TIPO_NUMERO,
    TIPO_CADENA,
    TIPO_BOOLEANO,
    TIPO_ERROR
} Tipo;

typedef struct Simbolo {
    char *nombre;
    Tipo tipo;
    int inicializado;
    int linea;
    int columna;          /* añadido */
    struct Simbolo *sig;
} Simbolo;

typedef struct Ambito {
    Simbolo *primerSimbolo;
    struct Ambito *padre;
} Ambito;

Ambito *crearAmbito(Ambito *padre);
void destruirAmbito(Ambito *ambito);
Simbolo *buscarSimbolo(Ambito *ambito, const char *nombre);
Simbolo *buscarSimboloLocal(Ambito *ambito, const char *nombre);
int insertarSimbolo(Ambito *ambito, const char *nombre, Tipo tipo, int linea, int columna);
void analizarSemantico(NodoAST *raiz);

#endif