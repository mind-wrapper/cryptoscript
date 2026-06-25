#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

/* Pila de ámbitos: el ámbito actual es el último creado */
static Ambito *ambitoActual = NULL;

/* Contador de errores semánticos */
static int erroresSemanticos = 0;

/* ---------- Funciones de ámbito ---------- */

Ambito *crearAmbito(Ambito *padre) {
    Ambito *a = malloc(sizeof(Ambito));
    a->primerSimbolo = NULL;
    a->padre = padre;
    return a;
}

void destruirAmbito(Ambito *ambito) {
    Simbolo *s = ambito->primerSimbolo;
    while (s) {
        Simbolo *tmp = s;
        s = s->sig;
        free(tmp->nombre);
        free(tmp);
    }
    free(ambito);
}

/* Busca un símbolo en el ámbito actual y en todos los superiores */
Simbolo *buscarSimbolo(Ambito *ambito, const char *nombre) {
    while (ambito) {
        Simbolo *s = ambito->primerSimbolo;
        while (s) {
            if (strcmp(s->nombre, nombre) == 0)
                return s;
            s = s->sig;
        }
        ambito = ambito->padre;
    }
    return NULL;
}

/* Busca solo en el ámbito dado (sin subir) */
Simbolo *buscarSimboloLocal(Ambito *ambito, const char *nombre) {
    Simbolo *s = ambito->primerSimbolo;
    while (s) {
        if (strcmp(s->nombre, nombre) == 0)
            return s;
        s = s->sig;
    }
    return NULL;
}

/* Inserta un nuevo símbolo. Retorna 0 si éxito, 1 si ya existe */
int insertarSimbolo(Ambito *ambito, const char *nombre, Tipo tipo, int linea) {
    if (buscarSimboloLocal(ambito, nombre)) {
        printf("Error semantico [linea %d]: variable '%s' redeclarada en el mismo ambito.\n", linea, nombre);
        erroresSemanticos++;
        return 1;
    }
    Simbolo *nuevo = malloc(sizeof(Simbolo));
    nuevo->nombre = strdup(nombre);
    nuevo->tipo = tipo;
    nuevo->inicializado = 0;
    nuevo->linea = linea;
    nuevo->sig = ambito->primerSimbolo;
    ambito->primerSimbolo = nuevo;
    return 0;
}

/* ---------- Funciones auxiliares para el análisis ---------- */

static Tipo tipoDeNodo(NodoAST *nodo);
static void analizarNodo(NodoAST *nodo);

/* Determina el tipo resultante de una expresión */
static Tipo tipoDeNodo(NodoAST *nodo) {
    if (!nodo) return TIPO_ERROR;
    switch (nodo->tipo) {
        case NODO_LITERAL_NUM:
            return TIPO_NUMERO;
        case NODO_LITERAL_CAD:
            return TIPO_CADENA;
        case NODO_IDENTIFICADOR: {
            Simbolo *s = buscarSimbolo(ambitoActual, nodo->nombre);
            if (!s) {
                printf("Error semantico: variable '%s' no declarada.\n", nodo->nombre);
                erroresSemanticos++;
                return TIPO_ERROR;
            }
            if (!s->inicializado) {
                printf("Error semantico: variable '%s' usada antes de ser inicializada.\n", nodo->nombre);
                erroresSemanticos++;
            }
            return s->tipo;
        }
        case NODO_BINARIO: {
            Tipo ti = tipoDeNodo(nodo->izq);
            Tipo td = tipoDeNodo(nodo->der);
            Operador op = nodo->op;
            if (op >= OP_PLUS && op <= OP_DIV) {
                if (ti != TIPO_NUMERO || td != TIPO_NUMERO) {
                    printf("Error semantico: operador aritmetico requiere operandos numericos.\n");
                    erroresSemanticos++;
                    return TIPO_ERROR;
                }
                return TIPO_NUMERO;
            }
            if (op >= OP_LESS && op <= OP_NOTEQ) {
                if (ti != TIPO_NUMERO || td != TIPO_NUMERO) {
                    printf("Error semantico: operador relacional requiere operandos numericos.\n");
                    erroresSemanticos++;
                    return TIPO_ERROR;
                }
                return TIPO_BOOLEANO;
            }
            return TIPO_ERROR;
        }
        case NODO_UNARIO: {
            Tipo t = tipoDeNodo(nodo->izq);
            if (nodo->op == OP_NEG) {
                if (t != TIPO_NUMERO) {
                    printf("Error semantico: el operador unario '-' requiere un operando numerico.\n");
                    erroresSemanticos++;
                    return TIPO_ERROR;
                }
                return TIPO_NUMERO;
            }
            return TIPO_ERROR;
        }
        case NODO_LLAMADA_CIFRADO:
            return TIPO_CADENA;
        default:
            return TIPO_ERROR;
    }
}

/* Analiza recursivamente un nodo del AST */
static void analizarNodo(NodoAST *nodo) {
    if (!nodo) return;

    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            analizarNodo(nodo->cuerpo);
            break;

        case NODO_DECL_MENSAJE: {
            Tipo t = tipoDeNodo(nodo->der);
            if (t != TIPO_CADENA) {
                printf("Error semantico: un mensaje debe inicializarse con una cadena.\n");
                erroresSemanticos++;
            }
            insertarSimbolo(ambitoActual, nodo->nombre, TIPO_CADENA, 0);
            Simbolo *s = buscarSimbolo(ambitoActual, nodo->nombre);
            if (s) s->inicializado = 1;
            /* Regla propia: mensaje no vacío si es literal */
            if (nodo->der->tipo == NODO_LITERAL_CAD) {
                if (strlen(nodo->der->nombre) <= 2) {
                    printf("Error semantico: el mensaje no puede ser una cadena vacia.\n");
                    erroresSemanticos++;
                }
            }
            analizarNodo(nodo->der);   // para revisar llamadas a cifrado dentro
            break;
        }

        case NODO_DECL_CLAVE: {
            Tipo t = tipoDeNodo(nodo->der);
            if (t != TIPO_NUMERO && t != TIPO_CADENA) {
                printf("Error semantico: una clave debe ser numero o cadena.\n");
                erroresSemanticos++;
            }
            insertarSimbolo(ambitoActual, nodo->nombre, t, 0);
            Simbolo *s = buscarSimbolo(ambitoActual, nodo->nombre);
            if (s) s->inicializado = 1;
            analizarNodo(nodo->der);
            break;
        }

        case NODO_ASIGNACION: {
            Tipo t = tipoDeNodo(nodo->der);
            Simbolo *s = buscarSimbolo(ambitoActual, nodo->nombre);
            if (s) {
                if (s->tipo != t) {
                    printf("Error semantico: tipos incompatibles en asignacion a '%s'.\n", nodo->nombre);
                    erroresSemanticos++;
                }
                s->inicializado = 1;
            }
            analizarNodo(nodo->der);
            break;
        }

        case NODO_IMPRIMIR:
            analizarNodo(nodo->der);
            break;

        case NODO_SI: {
            Tipo cond = tipoDeNodo(nodo->cond);
            if (cond != TIPO_BOOLEANO) {
                printf("Error semantico: la condicion del 'si' debe ser booleana.\n");
                erroresSemanticos++;
            }
            analizarNodo(nodo->cond);
            Ambito *ambitoSi = crearAmbito(ambitoActual);
            ambitoActual = ambitoSi;
            analizarNodo(nodo->cuerpo);
            ambitoActual = ambitoSi->padre;
            destruirAmbito(ambitoSi);
            if (nodo->sino) {
                Ambito *ambitoSino = crearAmbito(ambitoActual);
                ambitoActual = ambitoSino;
                analizarNodo(nodo->sino);
                ambitoActual = ambitoSino->padre;
                destruirAmbito(ambitoSino);
            }
            break;
        }

        case NODO_MIENTRAS: {
            Tipo cond = tipoDeNodo(nodo->cond);
            if (cond != TIPO_BOOLEANO) {
                printf("Error semantico: la condicion del 'mientras' debe ser booleana.\n");
                erroresSemanticos++;
            }
            analizarNodo(nodo->cond);
            Ambito *ambitoWhile = crearAmbito(ambitoActual);
            ambitoActual = ambitoWhile;
            analizarNodo(nodo->cuerpo);
            ambitoActual = ambitoWhile->padre;
            destruirAmbito(ambitoWhile);
            break;
        }

        case NODO_LLAMADA_CIFRADO: {
            Tipo tipoMsg = tipoDeNodo(nodo->arg1);
            if (tipoMsg != TIPO_CADENA) {
                printf("Error semantico: el primer argumento de cifrar/descifrar debe ser un mensaje (cadena).\n");
                erroresSemanticos++;
            }
            Tipo tipoClave = tipoDeNodo(nodo->arg2);
            if (strcmp(nodo->metodo, "cesar") == 0) {
                if (tipoClave != TIPO_NUMERO) {
                    printf("Error semantico: el cifrado 'cesar' requiere una clave numerica.\n");
                    erroresSemanticos++;
                } else {
                    if (nodo->arg2->tipo == NODO_LITERAL_NUM) {
                        double val = nodo->arg2->valorNumerico;
                        if (val < 1 || val > 25) {
                            printf("Error semantico: la clave Cesar debe estar entre 1 y 25, se encontro %g.\n", val);
                            erroresSemanticos++;
                        }
                    }
                }
            } else if (strcmp(nodo->metodo, "vigenere") == 0 ||
                       strcmp(nodo->metodo, "sustitucion") == 0) {
                if (tipoClave != TIPO_CADENA) {
                    printf("Error semantico: el cifrado '%s' requiere una clave de tipo cadena.\n", nodo->metodo);
                    erroresSemanticos++;
                } else {
                    if (nodo->arg2->tipo == NODO_LITERAL_CAD) {
                        if (strlen(nodo->arg2->nombre) <= 2) {
                            printf("Error semantico: la clave para '%s' no puede ser una cadena vacia.\n", nodo->metodo);
                            erroresSemanticos++;
                        }
                    }
                }
            }
            break;
        }

        case NODO_BINARIO:
            analizarNodo(nodo->izq);
            analizarNodo(nodo->der);
            break;

        case NODO_UNARIO:
            analizarNodo(nodo->izq);
            break;

        case NODO_LITERAL_NUM:
        case NODO_LITERAL_CAD:
        case NODO_IDENTIFICADOR:
            /* ya procesados por tipoDeNodo cuando se necesitó */
            break;

        default:
            break;
    }

    if (nodo->sig)
        analizarNodo(nodo->sig);
}

/* Punto de entrada del análisis semántico */
void analizarSemantico(NodoAST *raiz) {
    ambitoActual = crearAmbito(NULL);   /* ámbito global */
    erroresSemanticos = 0;
    analizarNodo(raiz);
    if (erroresSemanticos == 0) {
        printf("\nAnálisis semántico completado sin errores.\n");
    } else {
        printf("\nSe encontraron %d errores semánticos.\n", erroresSemanticos);
    }
    destruirAmbito(ambitoActual);
    ambitoActual = NULL;
}