%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "calculette.bison.hpp"
%}

%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)? {  yylval = atof(yytext); return NUM; }
\n    {  return '\n'; }
plus   { return '+'; }
fois   { return '*'; 
Si|if {return IF ;}
Alors|then {return THEN ;}
Sinon|else {return ELSE ;}
" "    {}
.      {  return yytext[0]; }
un     { yylval = 1 ; return NUM;}
deux   { yylval = 2 ; return NUM;}
sin    {return SIN;}
tan    {return TAN;}

%%

int yywrap(){}