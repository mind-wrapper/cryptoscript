#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* Constructor base */
NodoAST *crearNodo(TipoNodo tipo, int linea, int columna) {
    NodoAST *n = malloc(sizeof(NodoAST));
    n->tipo = tipo;
    n->linea = linea;
    n->columna = columna;
    n->nombre = NULL;
    n->valorNumerico = 0.0;
    n->izq = n->der = n->cond = n->cuerpo = n->sino = n->arg1 = n->arg2 = n->sig = NULL;
    n->operCifrado = n->metodo = NULL;
    return n;
}

NodoAST *crearNodoID(char *nombre, int linea, int columna) {
    NodoAST *n = crearNodo(NODO_IDENTIFICADOR, linea, columna);
    n->nombre = strdup(nombre);
    return n;
}

NodoAST *crearNodoNum(double val, int linea, int columna) {
    NodoAST *n = crearNodo(NODO_LITERAL_NUM, linea, columna);
    n->valorNumerico = val;
    return n;
}

NodoAST *crearNodoCad(char *cad, int linea, int columna) {
    NodoAST *n = crearNodo(NODO_LITERAL_CAD, linea, columna);
    n->nombre = strdup(cad);
    return n;
}

NodoAST *crearNodoBinario(Operador op, NodoAST *izq, NodoAST *der, int linea, int columna) {
    NodoAST *n = crearNodo(NODO_BINARIO, linea, columna);
    n->op = op;
    n->izq = izq;
    n->der = der;
    return n;
}

NodoAST *crearNodoUnario(Operador op, NodoAST *expr, int linea, int columna) {
    NodoAST *n = crearNodo(NODO_UNARIO, linea, columna);
    n->op = op;
    n->izq = expr;
    return n;
}

NodoAST *crearNodoCifrado(char *oper, char *metodo, NodoAST *arg1, NodoAST *arg2, int linea, int columna) {
    NodoAST *n = crearNodo(NODO_LLAMADA_CIFRADO, linea, columna);
    n->operCifrado = strdup(oper);
    n->metodo = strdup(metodo);
    n->arg1 = arg1;
    n->arg2 = arg2;
    return n;
}

/* Impresión con sangría para depuración */
static void imprimirSangria(int nivel) {
    for (int i = 0; i < nivel; i++) printf("  ");
}

void imprimirAST(NodoAST *nodo, int nivel) {
    if (!nodo) return;
    imprimirSangria(nivel);
    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            printf("Programa:\n");
            if (nodo->cuerpo) imprimirAST(nodo->cuerpo, nivel + 1);
            break;
        case NODO_DECL_MENSAJE:
            printf("DeclMensaje: %s", nodo->nombre);
            printf(" (línea %d, col %d)\n", nodo->linea, nodo->columna);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_DECL_CLAVE:
            printf("DeclClave: %s", nodo->nombre);
            printf(" (línea %d, col %d)\n", nodo->linea, nodo->columna);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_ASIGNACION:
            printf("Asignacion: %s", nodo->nombre);
            printf(" (línea %d, col %d)\n", nodo->linea, nodo->columna);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_IMPRIMIR:
            printf("Imprimir:\n");
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_SI:
            printf("Si (línea %d, col %d):\n", nodo->linea, nodo->columna);
            imprimirSangria(nivel + 1); printf("Cond:\n");
            if (nodo->cond) imprimirAST(nodo->cond, nivel + 2);
            imprimirSangria(nivel + 1); printf("Cuerpo:\n");
            if (nodo->cuerpo) imprimirAST(nodo->cuerpo, nivel + 2);
            if (nodo->sino) {
                imprimirSangria(nivel + 1); printf("Sino:\n");
                imprimirAST(nodo->sino, nivel + 2);
            }
            break;
        case NODO_MIENTRAS:
            printf("Mientras (línea %d, col %d):\n", nodo->linea, nodo->columna);
            imprimirSangria(nivel + 1); printf("Cond:\n");
            if (nodo->cond) imprimirAST(nodo->cond, nivel + 2);
            imprimirSangria(nivel + 1); printf("Cuerpo:\n");
            if (nodo->cuerpo) imprimirAST(nodo->cuerpo, nivel + 2);
            break;
        case NODO_BINARIO:
            printf("BinOp: %d (línea %d, col %d)\n", nodo->op, nodo->linea, nodo->columna);
            if (nodo->izq) imprimirAST(nodo->izq, nivel + 1);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_UNARIO:
            printf("UnOp: %d (línea %d, col %d)\n", nodo->op, nodo->linea, nodo->columna);
            if (nodo->izq) imprimirAST(nodo->izq, nivel + 1);
            break;
        case NODO_LITERAL_NUM:
            printf("Num: %g (línea %d, col %d)\n", nodo->valorNumerico, nodo->linea, nodo->columna);
            break;
        case NODO_LITERAL_CAD:
            printf("Cad: %s (línea %d, col %d)\n", nodo->nombre, nodo->linea, nodo->columna);
            break;
        case NODO_IDENTIFICADOR:
            printf("ID: %s (línea %d, col %d)\n", nodo->nombre, nodo->linea, nodo->columna);
            break;
        case NODO_LLAMADA_CIFRADO:
            printf("Cifrado: %s %s (línea %d, col %d)\n", nodo->operCifrado, nodo->metodo, nodo->linea, nodo->columna);
            if (nodo->arg1) imprimirAST(nodo->arg1, nivel + 1);
            if (nodo->arg2) imprimirAST(nodo->arg2, nivel + 1);
            break;
    }
    if (nodo->sig)
        imprimirAST(nodo->sig, nivel);
}