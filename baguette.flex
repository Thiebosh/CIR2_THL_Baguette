/*fichier flex : scan*/
%option noyywrap
%{
  #include "parser.bison.cpp"
%}

%%

-?[0-9]+               { yylval.intValeur = atoi(yytext); return INT_VALUE; }
-?[0-9]+[\.][0-9]*     { yylval.doubleValeur = atof(yytext); return DOUBLE_VALUE; }

entier                      { return INT; }//futur : binaire return bool
reel                        { return DOUBLE; }
texte                       { return STRING; }
\"[A-Za-z_][0-9A-Za-z_]*\"  { /*to do : retirer les "*/yylval.stringValeur = strdup(yytext); return STRING_VALUE; }//OK car guillemets 

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


si 			  { return IF; }
sinon	    { return ELSE; }
tantque 	{ return WHILE; }
faire 	  { return DO; }
répéter   { return REPEAT; }
parcourir { return FOREACH; }
fin	      { return END; }

TERMINER(.|\n)*	{ return END_PRGM; }


[A-Za-z_][0-9A-Za-z_]*   		{ yylval.nom = strdup(yytext); return VARIABLE_NAME; }

\r\n|\r|\n  	                { return '\n'; }
[ \t]|\/\/.*|\/\*(.|\n)*\*\/	{ }
.							                { return yytext[0]; }

%%