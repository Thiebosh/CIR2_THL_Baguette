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
\"[0-9A-Za-zéèàçâôùê_!?\'\+\-\*\/(\\(n|t)),;:_ ]*\"  { //OK car guillemets 
                                                      string read = strdup(yytext);
                                                      yylval.stringValeur = strdup(read.substr(1,read.size()-2).c_str());//retire les guillemets
                                                      return STRING_VALUE;
                                                    }

liste                       { return TAB; }
TAILLE                      { return SIZE; }
SUPPRIMER                   { return DELETE; }


AFFICHER	{ return DISPLAY;}
PAUSE  { return STOP; }//attendre pour delay

\+=   { return PLUS_CREMENT; }
\-=   { return MOINS_CREMENT; }
\*=   { return FOIS_CREMENT; }
\/=   { return DIVISE_CREMENT; }

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