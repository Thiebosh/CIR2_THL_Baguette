/*fichier flex : scan*/
%option noyywrap
%{
  #include "parser.bison.cpp"
%}

%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)? {  yylval.valeur = atof(yytext); return NUMBER; }

Répéter|Réitérer { return REPEAT; }

si|SI|Si 			{return IF;}
Alors|alors|ALORS 	{return THEN;}
Sinon|sinon|SINON	{return ELSE;}
FinSi|finsi|FINSI	{return END_IF;}


plus        {return '+';}
fois        {return '*';}
moins       {return '-';}
divise      {return '/' ;}
puissance   {return '^' ;}
racine      {return SQRT ;}
et          {return '&' ;}
ou          {return '|' ;}
non         {return '!' ;}
un          {yylval.valeur = 1 ; return NUMBER ;}
deux        {yylval.valeur = 2 ; return NUMBER ;}
sin|sinus   {return SIN ;}
tan|tangente  {return TAN ;}


[A-Za-z_][0-9A-Za-z_]*   		{ strcpy(yylval.nom,yytext); return IDENTIFIER; }/*tester fputs(yytext, yyout) : https://stackoverflow.com/questions/48013348/bison-yacc-yyerror-doesnt-work?rq=1*/

\r\n  							{  return '\n'; }	
\r								{  return '\n'; }
\n								{  return '\n'; }

[ \t]							{ }

.								{  return yytext[0]; }

%%