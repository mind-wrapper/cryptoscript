#ifndef AST_H
#define AST_H

typedef enum {
    NODO_PROGRAMA,
    NODO_DECL_MENSAJE,
    NODO_DECL_CLAVE,
    NODO_ASIGNACION,
    NODO_IMPRIMIR,
    NODO_SI,
    NODO_MIENTRAS,
    NODO_BINARIO,
    NODO_UNARIO,
    NODO_LITERAL_NUM,
    NODO_LITERAL_CAD,
    NODO_IDENTIFICADOR,
    NODO_LLAMADA_CIFRADO
} TipoNodo;

typedef enum {
    OP_PLUS, OP_MINUS, OP_MULT, OP_DIV,
    OP_LESS, OP_GREATER, OP_LESSEQ, OP_GREATEREQ, OP_EQEQ, OP_NOTEQ,
    OP_NEG
} Operador;

typedef struct NodoAST {
    TipoNodo tipo;
    int linea;            // <-- nuevo
    int columna;          // <-- nuevo
    char *nombre;
    double valorNumerico;
    Operador op;
    struct NodoAST *izq;
    struct NodoAST *der;
    struct NodoAST *cond;
    struct NodoAST *cuerpo;
    struct NodoAST *sino;
    char *operCifrado;
    char *metodo;
    struct NodoAST *arg1;
    struct NodoAST *arg2;
    struct NodoAST *sig;
} NodoAST;

NodoAST *crearNodo(TipoNodo tipo, int linea, int columna);
NodoAST *crearNodoID(char *nombre, int linea, int columna);
NodoAST *crearNodoNum(double val, int linea, int columna);
NodoAST *crearNodoCad(char *cad, int linea, int columna);
NodoAST *crearNodoBinario(Operador op, NodoAST *izq, NodoAST *der, int linea, int columna);
NodoAST *crearNodoUnario(Operador op, NodoAST *expr, int linea, int columna);
NodoAST *crearNodoCifrado(char *oper, char *metodo, NodoAST *arg1, NodoAST *arg2, int linea, int columna);
void imprimirAST(NodoAST *nodo, int nivel);

#endif