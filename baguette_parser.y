%{
  #include <iostream>
  #include <math.h>
  #define FILENAME "program.choco"
  extern int yylex ();

  int yyerror(char *s);
  using namespace std;

%}

%code requires
  {
    #define YYSTYPE double
  }

%token NUM	
%token SIN
%token TAN
%left '+' '-'
%left '*' '/'

%%
program: /* empty */		
       | program line          
	   ;

line: '\n'			 
	| expr '\n' { cout << endl << "Result : " << $1 << endl; }	
	;

expr:
     NUM                 { $$ = $1; }		
     | expr '+' expr     { $$ = $1 + $3; cout << $1 << " + " << $3 << " = " << $$ << endl; }
     | expr '-' expr     { $$ = $1 - $3; cout << $1 << " - " << $3 << " = " << $$ << endl; }
     | expr '*' expr     { $$ = $1 * $3; cout << $1 << " * " << $3 << " = " << $$ << endl; }
     | '(' expr ')'      { $$ = $2; }
     | SIN '(' expr ')'  { $$ = sin($3); cout << "sin(" << $3 << ") = " << $$ << endl; }
     | TAN '(' expr ')'  { $$ = tan($3); cout << "tan(" << $3 << ") = " << $$ << endl; }
     ;
%%

int yyerror(char *s) {					
    cout << s << endl;
}


int main()
{
  //yyparse() retourne 0 si entrée valide en fonctionde la grammaire
  //yylex() retourne une valeur qui indique le type du token optenu
	auto yyin = fopen( FILENAME, "r" );
	cout << yyparse();
}