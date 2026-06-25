#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

NodoAST *crearNodo(TipoNodo tipo) {
    NodoAST *n = malloc(sizeof(NodoAST));
    n->tipo = tipo;
    n->nombre = NULL;
    n->valorNumerico = 0.0;
    n->izq = n->der = n->cond = n->cuerpo = n->sino = n->arg1 = n->arg2 = n->sig = NULL;
    n->operCifrado = n->metodo = NULL;
    return n;
}

NodoAST *crearNodoID(char *nombre) {
    NodoAST *n = crearNodo(NODO_IDENTIFICADOR);
    n->nombre = strdup(nombre);
    return n;
}

NodoAST *crearNodoNum(double val) {
    NodoAST *n = crearNodo(NODO_LITERAL_NUM);
    n->valorNumerico = val;
    return n;
}

NodoAST *crearNodoCad(char *cad) {
    NodoAST *n = crearNodo(NODO_LITERAL_CAD);
    n->nombre = strdup(cad);
    return n;
}

NodoAST *crearNodoBinario(Operador op, NodoAST *izq, NodoAST *der) {
    NodoAST *n = crearNodo(NODO_BINARIO);
    n->op = op;
    n->izq = izq;
    n->der = der;
    return n;
}

NodoAST *crearNodoUnario(Operador op, NodoAST *expr) {
    NodoAST *n = crearNodo(NODO_UNARIO);
    n->op = op;
    n->izq = expr;
    return n;
}

NodoAST *crearNodoCifrado(char *oper, char *metodo, NodoAST *arg1, NodoAST *arg2) {
    NodoAST *n = crearNodo(NODO_LLAMADA_CIFRADO);
    n->operCifrado = strdup(oper);
    n->metodo = strdup(metodo);
    n->arg1 = arg1;
    n->arg2 = arg2;
    return n;
}

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
            printf("DeclMensaje: %s\n", nodo->nombre);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_DECL_CLAVE:
            printf("DeclClave: %s\n", nodo->nombre);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_ASIGNACION:
            printf("Asignacion: %s\n", nodo->nombre);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_IMPRIMIR:
            printf("Imprimir:\n");
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_SI:
            printf("Si:\n");
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
            printf("Mientras:\n");
            imprimirSangria(nivel + 1); printf("Cond:\n");
            if (nodo->cond) imprimirAST(nodo->cond, nivel + 2);
            imprimirSangria(nivel + 1); printf("Cuerpo:\n");
            if (nodo->cuerpo) imprimirAST(nodo->cuerpo, nivel + 2);
            break;
        case NODO_BINARIO:
            printf("BinOp: %d\n", nodo->op);
            if (nodo->izq) imprimirAST(nodo->izq, nivel + 1);
            if (nodo->der) imprimirAST(nodo->der, nivel + 1);
            break;
        case NODO_UNARIO:
            printf("UnOp: %d\n", nodo->op);
            if (nodo->izq) imprimirAST(nodo->izq, nivel + 1);
            break;
        case NODO_LITERAL_NUM:
            printf("Num: %g\n", nodo->valorNumerico);
            break;
        case NODO_LITERAL_CAD:
            printf("Cad: %s\n", nodo->nombre);
            break;
        case NODO_IDENTIFICADOR:
            printf("ID: %s\n", nodo->nombre);
            break;
        case NODO_LLAMADA_CIFRADO:
            printf("Cifrado: %s %s\n", nodo->operCifrado, nodo->metodo);
            if (nodo->arg1) imprimirAST(nodo->arg1, nivel + 1);
            if (nodo->arg2) imprimirAST(nodo->arg2, nivel + 1);
            break;
    }
    if (nodo->sig)
        imprimirAST(nodo->sig, nivel);
}