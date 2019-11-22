/*fichier flex : scan*/
%option noyywrap
%{
  #include "parser.bison.cpp"
%}

%%

-?[0-9]+               { yylval.intValeur = atoi(yytext); return INT_VALUE; }
-?[0-9]+[\.][0-9]*     { yylval.doubleValeur = atof(yytext); return DOUBLE_VALUE; }

entier                      { return INT; }
virgule                     { return DOUBLE; }
texte                       { return STRING; }
"[A-Za-z_][0-9A-Za-z_]*"   	{ strcpy(yylval.stringValeur,yytext);    return STRING_VALUE; }//OK car guillemets + tester fputs(yytext, yyout) : https://stackoverflow.com/questions/48013348/bison-yacc-yyerror-doesnt-work?rq=1

liste                       { return TAB; }
TAILLE                      { return SIZE; }
SUPPRIMER                   { return DELETE; }

racine      {return SQRT ;}//bloc a conserver?
et          {return '&' ;}
ou          {return '|' ;}
non         {return '!' ;}
sin|sinus   {return SIN ;}
tan|tangente  {return TAN ;}


AFFICHER	{return DISPLAY;}



si 			{return IF;}
sinon	  {return ELSE;}
finSi	  {return END_IF;}

tantque 		{return WHILE;}
finTantque	{return END_WHILE;}

répéter { return REPEAT; }
finRépéter { return END_REPEAT; }

parcourir { return FOREACH; }
finParcourir { return END_FOREACH; }

TERMINER	{return END_PRGM;}


[A-Za-z_][0-9A-Za-z_]*   		{ strcpy(yylval.nom,yytext); return VARIABLE_NAME; }/*tester fputs(yytext, yyout) : https://stackoverflow.com/questions/48013348/bison-yacc-yyerror-doesnt-work?rq=1*/

\r\n|\r|\n  	{  return '\n'; }	
[ \t]					{ }
.							{  return yytext[0]; }

%%