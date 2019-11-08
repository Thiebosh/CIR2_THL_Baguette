%{
  #include <math.h>
  #include <map>

  #include "fonctions.hpp"

  using namespace std;  

  extern int yylex ();
  int yyerror(char *s);

  map<string,double> variables; // table de symboles
  vector<pair<int,double>> instructions;
%}

%union{
  double val;
  char name[100];
}


%token <val>NUMBER
%type <val>expr
%token <name>IDENTIFIER
%token REPEAT
%token SIN
%token TAN
%token IF
%token ELSE
%token THEN
%token SQRT
%left '+' '-'
%left '*' '/'

%%
program: /* empty */		
  | program line      
	;

line: '\n'
	| expr ';' { cout << endl << "Result : " << $1 << endl; }
  | IDENTIFIER '=' expr ';' { variables[$1] = $3; cout << $1 << " = " << $3 << endl;}
	;

expr:
  NUMBER                   { $$ = $1; }
    | IDENTIFIER              { $$ = variables.at($1); }
    | expr '+' expr     { $$ = $1 + $3; cout << $1 << " + " << $3 << " = " << $$ << endl; }
    | expr '-' expr     { $$ = $1 - $3; cout << $1 << " - " << $3 << " = " << $$ << endl; }
    | expr '*' expr     { $$ = $1 * $3; cout << $1 << " * " << $3 << " = " << $$ << endl; }
    | '(' expr ')'      { $$ = $2; }
    | SIN '(' expr ')'  { $$ = sin($3); cout << "sin(" << $3 << ") = " << $$ << endl; }
    | TAN '(' expr ')'  { $$ = tan($3); cout << "tan(" << $3 << ") = " << $$ << endl; }
    | SQRT '(' expr ')' { $$ = sqrt($3); cout << "sqrt(" << $3 << ") = " << $$ << endl;}
    ;
%%

int yyerror(char *s) {
    cout << s << endl;
    return 0;
}

int main(int argc, char **argv) {
  yyin = mainContent(argc, argv);
  yyparse();
}
