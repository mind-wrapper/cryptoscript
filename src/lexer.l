%{
#include <stdio.h>
#include <string.h>
#include "parser.tab.h"

int col = 1;
%}

%option noyywrap
%option yylineno
%x COMENTARIO

%%
   /* Palabras reservadas */
"mensaje"      { printf("<MENSAJE, linea %d, col %d>\n", yylineno, col); col += yyleng; return MENSAJE; }
"clave"        { printf("<CLAVE, linea %d, col %d>\n", yylineno, col); col += yyleng; return CLAVE; }
"cifrar"       { printf("<CIFRAR, linea %d, col %d>\n", yylineno, col); col += yyleng; return CIFRAR; }
"descifrar"    { printf("<DESCIFRAR, linea %d, col %d>\n", yylineno, col); col += yyleng; return DESCIFRAR; }
"cesar"        { printf("<CESAR, linea %d, col %d>\n", yylineno, col); col += yyleng; return CESAR; }
"vigenere"     { printf("<VIGENERE, linea %d, col %d>\n", yylineno, col); col += yyleng; return VIGENERE; }
"sustitucion"  { printf("<SUSTITUCION, linea %d, col %d>\n", yylineno, col); col += yyleng; return SUSTITUCION; }
"imprimir"     { printf("<IMPRIMIR, linea %d, col %d>\n", yylineno, col); col += yyleng; return IMPRIMIR; }
"si"           { printf("<SI, linea %d, col %d>\n", yylineno, col); col += yyleng; return SI; }
"sino"         { printf("<SINO, linea %d, col %d>\n", yylineno, col); col += yyleng; return SINO; }
"mientras"     { printf("<MIENTRAS, linea %d, col %d>\n", yylineno, col); col += yyleng; return MIENTRAS; }

   /* Identificadores */
[a-zA-Z_][a-zA-Z0-9_]*   {
    printf("<ID, '%s', linea %d, col %d>\n", yytext, yylineno, col);
    col += yyleng;
    yylval.str = strdup(yytext);
    return ID;
}

   /* Operadores aritméticos */
"+"      { printf("<PLUS, linea %d, col %d>\n", yylineno, col); col += yyleng; return PLUS; }
"-"      { printf("<MINUS, linea %d, col %d>\n", yylineno, col); col += yyleng; return MINUS; }
"*"      { printf("<MULT, linea %d, col %d>\n", yylineno, col); col += yyleng; return MULT; }
"/"      { printf("<DIV, linea %d, col %d>\n", yylineno, col); col += yyleng; return DIV; }

   /* Operadores relacionales */
"<="     { printf("<LESSEQ, linea %d, col %d>\n", yylineno, col); col += yyleng; return LESSEQ; }
">="     { printf("<GREATEREQ, linea %d, col %d>\n", yylineno, col); col += yyleng; return GREATEREQ; }
"=="     { printf("<EQEQ, linea %d, col %d>\n", yylineno, col); col += yyleng; return EQEQ; }
"!="     { printf("<NOTEQ, linea %d, col %d>\n", yylineno, col); col += yyleng; return NOTEQ; }
"<"      { printf("<LESS, linea %d, col %d>\n", yylineno, col); col += yyleng; return LESS; }
">"      { printf("<GREATER, linea %d, col %d>\n", yylineno, col); col += yyleng; return GREATER; }

   /* Asignación */
"="      { printf("<ASSIGN, linea %d, col %d>\n", yylineno, col); col += yyleng; return ASSIGN; }

   /* Delimitadores */
"("      { printf("<LPAREN, linea %d, col %d>\n", yylineno, col); col += yyleng; return LPAREN; }
")"      { printf("<RPAREN, linea %d, col %d>\n", yylineno, col); col += yyleng; return RPAREN; }
"{"      { printf("<LBRACE, linea %d, col %d>\n", yylineno, col); col += yyleng; return LBRACE; }
"}"      { printf("<RBRACE, linea %d, col %d>\n", yylineno, col); col += yyleng; return RBRACE; }
";"      { printf("<SEMICOLON, linea %d, col %d>\n", yylineno, col); col += yyleng; return SEMICOLON; }
","      { printf("<COMMA, linea %d, col %d>\n", yylineno, col); col += yyleng; return COMMA; }

   /* Números */
[0-9]+(\.[0-9]+)?   {
    printf("<NUMBER, %s, linea %d, col %d>\n", yytext, yylineno, col);
    col += yyleng;
    yylval.num = atof(yytext);
    return NUMBER;
}

   /* Cadenas */
\"(\\.|[^\"\\])*\"   {
    printf("<STRING, %s, linea %d, col %d>\n", yytext, yylineno, col);
    col += yyleng;
    yylval.str = strdup(yytext);
    return STRING;
}

   /* Comentario de línea */
"//".*    { /* ignorar */ }

   /* Comentario de bloque */
"/*"          { BEGIN(COMENTARIO); }
<COMENTARIO>{
    "*/"      { BEGIN(INITIAL); }
    [^*\n]+   { /* ignorar */ }
    "*"       { /* ignorar */ }
    \n        { col = 1; }
}

   /* Saltos de línea y espacios */
\n          { col = 1; }
[ \t]       { col += yyleng; }

   /* Error léxico */
.           {
    printf("Error léxico [línea %d, columna %d]: carácter inesperado '%c'\n",
           yylineno, col, yytext[0]);
    col++;
}
%%