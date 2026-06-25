%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"     /* <-- añadir esta línea */

extern int yylineno;
int yylex(void);
extern FILE *yyin;

NodoAST *raiz = NULL;

void yyerror(const char *msg);
%}

%code requires { #include "ast.h" }

%union {
    char *str;
    double num;
    NodoAST *nodo;
}


/* Palabras reservadas */
%token MENSAJE CLAVE CIFRAR DESCIFRAR CESAR VIGENERE SUSTITUCION
%token IMPRIMIR SI SINO MIENTRAS

/* Operadores y delimitadores */
%token PLUS MINUS MULT DIV
%token LESS GREATER LESSEQ GREATEREQ EQEQ NOTEQ
%token ASSIGN LPAREN RPAREN LBRACE RBRACE SEMICOLON COMMA

%token <str> ID STRING
%token <num> NUMBER

%type <nodo> programa lista_sentencias sentencia bloque
%type <nodo> declaracion decl_mensaje decl_clave imprimir asignacion si_stmt mientras_stmt
%type <nodo> expresion llamada_cifrado metodo

%left EQEQ NOTEQ
%left LESS GREATER LESSEQ GREATEREQ
%left PLUS MINUS
%left MULT DIV
%right UNARY_MINUS

%%
programa:
    lista_sentencias    { raiz = crearNodo(NODO_PROGRAMA); raiz->cuerpo = $1; }
;

lista_sentencias:
    sentencia                   { $$ = $1; }
  | lista_sentencias sentencia  {
        NodoAST *n = $1;
        while (n->sig) n = n->sig;
        n->sig = $2;
        $$ = $1;
    }
;

sentencia:
    declaracion
  | imprimir
  | asignacion
  | si_stmt
  | mientras_stmt
  | expresion SEMICOLON   { $$ = $1; }
;

declaracion:
    decl_mensaje
  | decl_clave
;

decl_mensaje:
    MENSAJE ID ASSIGN expresion SEMICOLON {
        NodoAST *n = crearNodo(NODO_DECL_MENSAJE);
        n->nombre = $2;
        n->der = $4;
        $$ = n;
    }
;

decl_clave:
    CLAVE ID ASSIGN expresion SEMICOLON {
        NodoAST *n = crearNodo(NODO_DECL_CLAVE);
        n->nombre = $2;
        n->der = $4;
        $$ = n;
    }
;

imprimir:
    IMPRIMIR expresion SEMICOLON {
        NodoAST *n = crearNodo(NODO_IMPRIMIR);
        n->der = $2;
        $$ = n;
    }
;

asignacion:
    ID ASSIGN expresion SEMICOLON {
        NodoAST *n = crearNodo(NODO_ASIGNACION);
        n->nombre = $1;
        n->der = $3;
        $$ = n;
    }
;

si_stmt:
    SI LPAREN expresion RPAREN bloque {
        NodoAST *n = crearNodo(NODO_SI);
        n->cond = $3;
        n->cuerpo = $5;
        $$ = n;
    }
  | SI LPAREN expresion RPAREN bloque SINO bloque {
        NodoAST *n = crearNodo(NODO_SI);
        n->cond = $3;
        n->cuerpo = $5;
        n->sino = $7;
        $$ = n;
    }
;

mientras_stmt:
    MIENTRAS LPAREN expresion RPAREN bloque {
        NodoAST *n = crearNodo(NODO_MIENTRAS);
        n->cond = $3;
        n->cuerpo = $5;
        $$ = n;
    }
;

bloque:
    LBRACE lista_sentencias RBRACE  { $$ = $2; }
  | LBRACE RBRACE                  { $$ = NULL; }
;

expresion:
    expresion EQEQ expresion       { $$ = crearNodoBinario(OP_EQEQ, $1, $3); }
  | expresion NOTEQ expresion      { $$ = crearNodoBinario(OP_NOTEQ, $1, $3); }
  | expresion LESS expresion       { $$ = crearNodoBinario(OP_LESS, $1, $3); }
  | expresion GREATER expresion    { $$ = crearNodoBinario(OP_GREATER, $1, $3); }
  | expresion LESSEQ expresion     { $$ = crearNodoBinario(OP_LESSEQ, $1, $3); }
  | expresion GREATEREQ expresion  { $$ = crearNodoBinario(OP_GREATEREQ, $1, $3); }
  | expresion PLUS expresion       { $$ = crearNodoBinario(OP_PLUS, $1, $3); }
  | expresion MINUS expresion      { $$ = crearNodoBinario(OP_MINUS, $1, $3); }
  | expresion MULT expresion       { $$ = crearNodoBinario(OP_MULT, $1, $3); }
  | expresion DIV expresion        { $$ = crearNodoBinario(OP_DIV, $1, $3); }
  | MINUS expresion %prec UNARY_MINUS  { $$ = crearNodoUnario(OP_NEG, $2); }
  | LPAREN expresion RPAREN        { $$ = $2; }
  | llamada_cifrado
  | NUMBER                         { $$ = crearNodoNum($1); }
  | STRING                         { $$ = crearNodoCad($1); }
  | ID                             { $$ = crearNodoID($1); }
;

llamada_cifrado:
    CIFRAR metodo LPAREN expresion COMMA expresion RPAREN {
        NodoAST *n = crearNodoCifrado("cifrar", $2->nombre, $4, $6);
        free($2);
        $$ = n;
    }
  | DESCIFRAR metodo LPAREN expresion COMMA expresion RPAREN {
        NodoAST *n = crearNodoCifrado("descifrar", $2->nombre, $4, $6);
        free($2);
        $$ = n;
    }
;

metodo:
    CESAR        { $$ = crearNodoID("cesar"); }
  | VIGENERE     { $$ = crearNodoID("vigenere"); }
  | SUSTITUCION  { $$ = crearNodoID("sustitucion"); }
;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Error sintáctico [línea %d]: %s\n", yylineno, msg);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) { perror(argv[1]); return 1; }
    } else {
        yyin = stdin;
    }
    if (yyparse() == 0) {
        printf("\nAnálisis sintáctico completado con éxito.\n");
        printf("AST generado:\n");
        imprimirAST(raiz, 0);

        /* Análisis semántico */
        analizarSemantico(raiz);
    }
    return 0;
}