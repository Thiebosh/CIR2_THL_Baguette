%{
  #include <iostream>
  #include <string>
  #include "parser.bison.hpp"

  using namespace std;
%}

%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)? {  yylval.val = atof(yytext); return NUM; }

\n     {  return '\n'; }
plus   { return '+'; }
fois   { return '*'; }
moins  { return '-';}
divise { return '/' ;}
puissance { return '^' ;}
racine { return SQRT ;}
et     { return '&&' ;}
ou     { return '||' ;}
non    { return '!' ;}
" "    {}
un     { yylval.val = 1 ; return NUM;}
deux   { yylval.val = 2 ; return NUM;}
sin    {return SIN;}
tan    {return TAN;}
Si|if {return IF ;}
Alors|then {return THEN ;}
Sinon|else {return ELSE ;}

[a-zA-Z_][0-9a-zA-Z_]* { strcpy(yylval.name,yytext); return ID; }

.      { return yytext[0]; }

%%

int yywrap(){}

int main(int argc, char **argv) {
  ++argv, --argc;

  if (!argc) {
    cout << "0 - Interpréter la console" << endl 
        << "1 - Interpréter un fichier" << endl;
    int saisie;
    cin >> saisie;

    if (!saisie) yyin = stdin;
    else {
      cout << "Nom du fichier .choco : ";
      string filename;
      cin >> filename;
      if (filename.find(".choco") ==  string::npos) {
        //faire un substring des derniers caractères puis comparer
        filename += ".choco";
      }
      yyin = fopen(filename.c_str(),"r");
    }
    cout << endl;
  }
  else yyin = fopen(argv[0],"r");

  yyparse();
}