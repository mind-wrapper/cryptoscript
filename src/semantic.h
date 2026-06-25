#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/* Tipos de datos que maneja el lenguaje */
typedef enum {
    TIPO_NUMERO,
    TIPO_CADENA,
    TIPO_BOOLEANO,
    TIPO_ERROR
} Tipo;

/* Entrada en la tabla de símbolos */
typedef struct Simbolo {
    char *nombre;
    Tipo tipo;
    int inicializado;       /* 1 si ya se le asignó un valor */
    int linea;              /* línea donde se declaró */
    struct Simbolo *sig;    /* siguiente símbolo en el mismo ámbito */
} Simbolo;

/* Un ámbito: contiene una lista de símbolos y un puntero al ámbito padre */
typedef struct Ambito {
    Simbolo *primerSimbolo;
    struct Ambito *padre;
} Ambito;

/* Funciones */
Ambito *crearAmbito(Ambito *padre);
void destruirAmbito(Ambito *ambito);
Simbolo *buscarSimbolo(Ambito *ambito, const char *nombre);
Simbolo *buscarSimboloLocal(Ambito *ambito, const char *nombre);
int insertarSimbolo(Ambito *ambito, const char *nombre, Tipo tipo, int linea);
void analizarSemantico(NodoAST *raiz);

#endif