#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

static Ambito *ambitoActual = NULL;
static int erroresSemanticos = 0;

/* ---------- Ámbitos ---------- */

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

Simbolo *buscarSimboloLocal(Ambito *ambito, const char *nombre) {
    Simbolo *s = ambito->primerSimbolo;
    while (s) {
        if (strcmp(s->nombre, nombre) == 0)
            return s;
        s = s->sig;
    }
    return NULL;
}

int insertarSimbolo(Ambito *ambito, const char *nombre, Tipo tipo, int linea, int columna) {
    if (buscarSimboloLocal(ambito, nombre)) {
        printf("Error semántico [línea %d, columna %d]: variable '%s' redeclarada en el mismo ámbito.\n",
               linea, columna, nombre);
        erroresSemanticos++;
        return 1;
    }
    Simbolo *nuevo = malloc(sizeof(Simbolo));
    nuevo->nombre = strdup(nombre);
    nuevo->tipo = tipo;
    nuevo->inicializado = 0;
    nuevo->linea = linea;
    nuevo->columna = columna;
    nuevo->sig = ambito->primerSimbolo;
    ambito->primerSimbolo = nuevo;
    return 0;
}

/* ---------- Funciones auxiliares ---------- */

static Tipo tipoDeNodo(NodoAST *nodo);
static void analizarNodo(NodoAST *nodo);

static Tipo tipoDeNodo(NodoAST *nodo) {
    if (!nodo) return TIPO_ERROR;
    switch (nodo->tipo) {
        case NODO_LITERAL_NUM: return TIPO_NUMERO;
        case NODO_LITERAL_CAD: return TIPO_CADENA;
        case NODO_IDENTIFICADOR: {
            Simbolo *s = buscarSimbolo(ambitoActual, nodo->nombre);
            if (!s) {
                printf("Error semántico [línea %d, columna %d]: variable '%s' no declarada.\n",
                       nodo->linea, nodo->columna, nodo->nombre ? nodo->nombre : "???");
                erroresSemanticos++;
                return TIPO_ERROR;
            }
            if (!s->inicializado) {
                printf("Error semántico [línea %d, columna %d]: variable '%s' usada antes de ser inicializada.\n",
                       nodo->linea, nodo->columna, nodo->nombre);
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
                    printf("Error semántico [línea %d, columna %d]: operador aritmético requiere operandos numéricos.\n",
                           nodo->linea, nodo->columna);
                    erroresSemanticos++;
                    return TIPO_ERROR;
                }
                return TIPO_NUMERO;
            }
            if (op >= OP_LESS && op <= OP_NOTEQ) {
                if (ti != TIPO_NUMERO || td != TIPO_NUMERO) {
                    printf("Error semántico [línea %d, columna %d]: operador relacional requiere operandos numéricos.\n",
                           nodo->linea, nodo->columna);
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
                    printf("Error semántico [línea %d, columna %d]: el operador unario '-' requiere un operando numérico.\n",
                           nodo->linea, nodo->columna);
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

static void analizarNodo(NodoAST *nodo) {
    if (!nodo) return;

    switch (nodo->tipo) {
        case NODO_PROGRAMA:
            analizarNodo(nodo->cuerpo);
            break;

        case NODO_DECL_MENSAJE: {
            Tipo t = tipoDeNodo(nodo->der);
            if (t != TIPO_CADENA) {
                printf("Error semántico [línea %d, columna %d]: un mensaje debe inicializarse con una cadena.\n",
                       nodo->linea, nodo->columna);
                erroresSemanticos++;
            }
            insertarSimbolo(ambitoActual, nodo->nombre, TIPO_CADENA, nodo->linea, nodo->columna);
            Simbolo *s = buscarSimbolo(ambitoActual, nodo->nombre);
            if (s) s->inicializado = 1;
            if (nodo->der->tipo == NODO_LITERAL_CAD && strlen(nodo->der->nombre) <= 2) {
                printf("Error semántico [línea %d, columna %d]: el mensaje no puede ser una cadena vacía.\n",
                       nodo->der->linea, nodo->der->columna);
                erroresSemanticos++;
            }
            analizarNodo(nodo->der);
            break;
        }

        case NODO_DECL_CLAVE: {
            Tipo t = tipoDeNodo(nodo->der);
            if (t != TIPO_NUMERO && t != TIPO_CADENA) {
                printf("Error semántico [línea %d, columna %d]: una clave debe ser número o cadena.\n",
                       nodo->linea, nodo->columna);
                erroresSemanticos++;
            }
            insertarSimbolo(ambitoActual, nodo->nombre, t, nodo->linea, nodo->columna);
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
                    printf("Error semántico [línea %d, columna %d]: tipos incompatibles en asignación a '%s'.\n",
                           nodo->linea, nodo->columna, nodo->nombre);
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
                printf("Error semántico [línea %d, columna %d]: la condición del 'si' debe ser booleana.\n",
                       nodo->linea, nodo->columna);
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
                printf("Error semántico [línea %d, columna %d]: la condición del 'mientras' debe ser booleana.\n",
                       nodo->linea, nodo->columna);
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
                printf("Error semántico [línea %d, columna %d]: el primer argumento de cifrar/descifrar debe ser un mensaje (cadena).\n",
                       nodo->linea, nodo->columna);
                erroresSemanticos++;
            }
            Tipo tipoClave = tipoDeNodo(nodo->arg2);
            if (strcmp(nodo->metodo, "cesar") == 0) {
                if (tipoClave != TIPO_NUMERO) {
                    printf("Error semántico [línea %d, columna %d]: el cifrado 'cesar' requiere una clave numérica.\n",
                           nodo->linea, nodo->columna);
                    erroresSemanticos++;
                } else if (nodo->arg2->tipo == NODO_LITERAL_NUM) {
                    double val = nodo->arg2->valorNumerico;
                    if (val < 1 || val > 25) {
                        printf("Error semántico [línea %d, columna %d]: la clave César debe estar entre 1 y 25, se encontró %g.\n",
                               nodo->arg2->linea, nodo->arg2->columna, val);
                        erroresSemanticos++;
                    }
                }
            } else if (strcmp(nodo->metodo, "vigenere") == 0 ||
                       strcmp(nodo->metodo, "sustitucion") == 0) {
                if (tipoClave != TIPO_CADENA) {
                    printf("Error semántico [línea %d, columna %d]: el cifrado '%s' requiere una clave de tipo cadena.\n",
                           nodo->linea, nodo->columna, nodo->metodo);
                    erroresSemanticos++;
                } else if (nodo->arg2->tipo == NODO_LITERAL_CAD && strlen(nodo->arg2->nombre) <= 2) {
                    printf("Error semántico [línea %d, columna %d]: la clave para '%s' no puede ser una cadena vacía.\n",
                           nodo->arg2->linea, nodo->arg2->columna, nodo->metodo);
                    erroresSemanticos++;
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
            break;

        default:
            break;
    }

    if (nodo->sig)
        analizarNodo(nodo->sig);
}

void analizarSemantico(NodoAST *raiz) {
    ambitoActual = crearAmbito(NULL);
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