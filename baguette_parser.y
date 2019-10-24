%{
  #include <math.h>
  #include <map>

  #include "fonctions.hpp"

  using namespace std;  

  extern int yylex ();
  int yyerror(char *s);

  map<string,double> variables; // table de symboles
  vector<pair<int,double>> instructionList;
%}

%union{
  double val;
  char name[100];
}


%token <val>NUMBER
%type <val>expression
%token <name>IDENTIFIER
%token SIN
%token TAN
%token IF
%token ELSE
%token THEN
%token SQRT
%token REPEAT
%left '+' '-'
%left '*' '/'

%%
program:
  program line
  | /* Epsilon */
	;

line: 
  line instruction ';'
  | '\n'
	;

instruction : 
  expression                              { }
  | REPEAT '(' expression ')' expression  { for (int i=0; i<$3; i++) cout << $5 << endl; }
  | IDENTIFIER '=' expression             {  variables[$1] = $3; cout << "affectation de " << $3 << " à " << $1 << endl; }
  ;

expression : 
  '(' expression ')'            { }
  | expression '+' expression   { instructionList.push_back(make_pair('+',0)); }
  | expression '-' expression   { instructionList.push_back(make_pair('-',0)); }
  | expression '*' expression   { instructionList.push_back(make_pair('*',0)); }
  | expression '/' expression   { instructionList.push_back(make_pair('/',0)); }
  | NUMBER                      { instructionList.push_back(make_pair(NUMBER,$1)); }
  | IDENTIFIER                  { instructionList.push_back(make_pair(IDENTIFIER,variables[$1])); }
  ;
%%

int yyerror(char *s) {
    cout << s << endl;
    return 0;
}


void print_program() {
  cout << "code généré" << endl;
  for (auto instruct : instructionList) cout << instruct.first << "\t" << instruct.second << endl;
  cout << "fin du code généré" << endl;
}

double depiler(vector<double> &pile) {
  double t = pile[pile.size()-1];
  cout << "Dépiler " << t << endl;
  pile.pop_back();
  return t;
}

void run_program() {
  vector<double> pile;
  double x,y;

  cout << "exécution" << endl;
  for (auto instruct : instructionList) {//remplacer par un while car ...
    switch(instruct.first) {
      case '+':
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(x+y);
        break;
      case '*':
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(x*y);
        break;
      case NUMBER:
        pile.push_back(instruct.second);
        break;
    }
  }
  cout << "résultat : " << depiler(pile) << endl;
  cout << "fin d'exécution" << endl;
}

int main(int argc, char **argv) {
  yyin = mainContent(argc, argv);
  yyparse();
}