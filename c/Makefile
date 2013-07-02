default:
	flex lexer.l
	bison parser.y
	gcc -Wall -Wno-unused-function -o ../lambda *.c
