compile:
	rm -rf baguette
	bison -d baguette_parser.y -o parser.bison.cpp
	flex -o scanner.lex.cpp baguette_scanner.l
	g++ scanner.lex.cpp fonctions.cpp -o baguette
	rm -rf scanner.lex.cpp parser.bison.cpp parser.bison.hpp


ex:
	rm -rf baguette
	bison -d baguette_parser_ex.y -o parser.bison.cpp
	flex -o scanner.lex.cpp baguette_scanner.l
	g++ scanner.lex.cpp fonctions.cpp -o baguette
	rm -rf scanner.lex.cpp parser.bison.cpp parser.bison.hpp
	./baguette


run:
	./baguette

all:
	make compile
	make run
