%{
  #include "parser.bison.cpp"
%}

%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)? {  yylval.val = atof(yytext); return NUMBER; }

\n          {return '\n';}
plus        {return '+';}
fois        {return '*';}
moins       {return '-';}
divise      {return '/' ;}
puissance   {return '^' ;}
racine      {return SQRT ;}
et          {return '&' ;}
ou          {return '|' ;}
non         {return '!' ;}
Repeter     {return REPEAT ;}
" "         {}
un          {yylval.val = 1 ; return NUMBER ;}
deux        {yylval.val = 2 ; return NUMBER ;}
sin|sinus   {return SIN ;}
tan|tangente  {return TAN ;}
Si|if       {return IF ;}
Alors|then  {return THEN ;}
Sinon|else  {return ELSE ;}

[a-zA-Z_][0-9a-zA-Z_]* { strcpy(yylval.name,yytext); return IDENTIFIER; }

.      { return yytext[0]; }

%%

int yywrap(){ return 0; }
