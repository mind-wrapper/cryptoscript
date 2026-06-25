all: compilador

compilador: lex.yy.c parser.tab.c ast.o semantic.o
	gcc -Isrc -o compilador lex.yy.c parser.tab.c ast.o semantic.o -lfl

lex.yy.c: src/lexer.l parser.tab.h
	flex src/lexer.l

parser.tab.c parser.tab.h: src/parser.y
	bison -d src/parser.y

ast.o: src/ast.c src/ast.h
	gcc -Isrc -c src/ast.c -o ast.o

semantic.o: src/semantic.c src/semantic.h src/ast.h
	gcc -Isrc -c src/semantic.c -o semantic.o

clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h ast.o semantic.o compilador