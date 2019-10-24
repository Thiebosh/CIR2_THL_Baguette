%{
  #include <iostream>
  #include <math.h>
  #include <map>
  #include <string>

  #include "fonctions.hpp"
  
  using namespace std;

  extern int yylex ();
  int yyerror(char *s);

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

    return 0;
}

int main(int argc, char **argv) {
  string folder = "programFiles/", filename, extension = ".choco";

  //teste existence de dossier
  bool getFolder = folderExist(folder, extension);

  //exécute routine
  ++argv, --argc;
  if (!argc) {
    switch(menuPrincipal()) {
      case 0 :
        yyin = stdin;
        break;
      case 1 :
        if (!getFolder) exit(0);

        vector<string> fileList = menuFichier(folder, filename, extension);
        
        int saisie;
        cout << endl << "Votre sélection : ";
        do cin >> saisie; while (saisie <= 0 || saisie > fileList.size());

        saisie--;

        //exécute fichier s'il existe
        filename = fileList[saisie];
        cout << filename << endl;
        if (access((folder+filename).c_str(), F_OK) != -1) {
          yyin = fopen((folder+filename).c_str(),"r");
        }
        else {
          cout << "Echec d'accès au fichier" << endl;
          exit(0);
        }
        break;
    }
    cout << endl;
  }
  else {
    if (!getFolder) exit(0);

    filename = argv[0];
    if (filename.find(extension, filename.size() - extension.size()) ==  string::npos) {
      filename += extension;
    }
    if (access((folder+filename).c_str(), F_OK) != -1) {
      yyin = fopen((folder+filename).c_str(),"r");
    }
    else {
      cout << "Ce fichier n'existe pas" << endl;
      exit(0);
    }
  }
  
  yyparse();
}