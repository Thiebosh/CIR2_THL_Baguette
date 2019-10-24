%{
  #include <iostream>
  #include <string>
  #include "parser.bison.hpp"

  using namespace std;
%}

%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)? {  yylval = atof(yytext); return NUM; }
\n     {  return '\n'; }
plus   { return '+'; }
fois   { return '*'; }
" "    {}
.      {  return yytext[0]; }
un     { yylval = 1 ; return NUM;}
deux   { yylval = 2 ; return NUM;}
sin    {return SIN;}
tan    {return TAN;}

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