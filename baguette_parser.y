%{
  #include <iostream>
  #include <math.h>
  #include <map>
  #include <string>
  #define FILENAME "program.choco"
  extern int yylex ();

  int yyerror(char *s);
  using namespace std;
  map <string,double> variables; // table de symboles
%}

%union{
  double val;
  char name[100];
}


%token <val>NUM	
%type <val>expr
%token <name>ID
%token SIN
%token TAN
%token IF
%token ELSE
%token THEN
%token SQRT
%token LOAD_FILE
%token END_OF_FILE
%left '+' '-'
%left '*' '/'

%%
program: /* empty */		
       | program line      
	   ;

line: '\n'
	| expr ';' { cout << endl << "Result : " << $1 << endl; }
  | ID '=' expr ';' { variables[$1] = $3; cout << $1 << " = " << $3 << endl;}
	;

expr:
     NUM                 { $$ = $1; }
     | ID                { $$ = variables.at($1); }
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
}