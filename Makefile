compile_keep:
	rm -rf baguette
	bison -d baguette.bison -o parser.bison.cpp
	flex -o scanner.lex.cpp baguette.flex
	g++ scanner.lex.cpp fonctions.cpp -o baguette

compile:
	make compile_keep
	rm -rf scanner.lex.cpp parser.bison.cpp parser.bison.hpp

run:
	./baguette

all:
	make compile
	make run
