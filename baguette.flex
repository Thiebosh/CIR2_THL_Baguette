/*fichier flex : scan*/
%option noyywrap
%{
  #include "parser.bison.cpp"
%}


%%

ouvrir_zone_memoire { return ADD_MEMORY_LAYER; }
fermer_zone_memoire { return REMOVE_MEMORY_LAYER; }

vrai    { yylval.intValeur = 1; return INT_VALUE; }
faux    { yylval.intValeur = 0; return INT_VALUE; }

-?[0-9]+               { yylval.intValeur = atoi(yytext); return INT_VALUE; }
-?[0-9]+[\.][0-9]*     { yylval.doubleValeur = atof(yytext); return DOUBLE_VALUE; }

entier                      { return INT; }
reel                        { return DOUBLE; }
texte                       { return STRING; }
\"[^\"\n]*\"  { //OK car guillemets 
                string read = strdup(yytext);
                yylval.stringValeur = strdup(read.substr(1,read.size()-2).c_str());//retire les guillemets
                return STRING_VALUE;
              }

liste       { return TAB; }
taille      { return SIZE; }
supprimer   { return DELETE; }

"++"  { return INCREMENT; }
"--"  { return DECREMENT; }
"+="  { return PLUS_CREMENT; }
"-="  { return MOINS_CREMENT; }
"*="  { return FOIS_CREMENT; }
"/="  { return DIVISE_CREMENT; }

ET    { return AND; }
OU    { return OR; }
"=="  { return EQUIV; }
"!="  { return DIFF; }
">="  { return INF_EGAL; }
"<="  { return SUP_EGAL; }

ecrire  	{ return WRITE;}
pause     { return STOP; }//attendre pour delay
lire      { return INPUT;}

si 			  { return IF; }
sinon	    { return ELSE; }
sinonsi	  { return ELIF; }
tantque 	{ return WHILE; }
faire 	  { return DO; }
pour      { return FOR; }
parcourir { return FOREACH; }
recette   { return FUNCTION; }

renvoyer	{ return RETURN; }
TERMINER(.|\n)*	{ return END_PRGM; }


[A-Za-z_][0-9A-Za-z_]*   		{ yylval.nom = strdup(yytext); return NAME; }

\r\n|\r|\n  	                { return '\n'; }
[ \t]|"//".*|"/*"(.|\n)*"*/"	{ }
.							                { return yytext[0]; }

%%