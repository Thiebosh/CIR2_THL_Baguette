/*fichier flex : scan*/
%option noyywrap
%{
  #include "parser.bison.cpp"
%}

%%

[0-9]+                      { yylval.intValeur = atoi(yytext); return INT_VALUE; }
[0-9]+[,.][0-9]*              { yylval.doubleValeur = atof(yytext); return DOUBLE_VALUE; }

entier                      { return INT; }
virgule                     { return DOUBLE; }
texte                       { return STRING; }
"[A-Za-z_][0-9A-Za-z_]*"   	{ strcpy(yylval.stringValeur,yytext);    return STRING_VALUE; }//OK car guillemets + tester fputs(yytext, yyout) : https://stackoverflow.com/questions/48013348/bison-yacc-yyerror-doesnt-work?rq=1


plus        {return '+';}//bloc a conserver?
fois        {return '*';}
moins       {return '-';}
divise      {return '/' ;}
puissance   {return '^' ;}
racine      {return SQRT ;}
et          {return '&' ;}
ou          {return '|' ;}
non         {return '!' ;}
sin|sinus   {return SIN ;}
tan|tangente  {return TAN ;}


AFFICHER	{return DISPLAY;}

si|SI|Si 			{return IF;}
Alors|alors|ALORS 	{return THEN;}
Sinon|sinon|SINON	{return ELSE;}
FinSi|finsi|FINSI	{return END_IF;}

Répéter|Réitérer { return REPEAT; }

TERMINER	{return END_PRGM;}


[A-Za-z_][0-9A-Za-z_]*   		{ strcpy(yylval.nom,yytext); return VARIABLE_NAME; }/*tester fputs(yytext, yyout) : https://stackoverflow.com/questions/48013348/bison-yacc-yyerror-doesnt-work?rq=1*/

\r\n|\r|\n  	{  return '\n'; }	
[ \t]					{ }
.							{  return yytext[0]; }

%%