%option noyywrap
%{
#include <string>

// structure pour stocker les adresses pour les sauts condistionnels et autres...
typedef struct adr {
    int ic_goto; 
    int ic_false;
} t_adresse;

#include "projet.bison.hpp" 

using namespace std;

%}
%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)?	{ yylval.valeur = atof(yytext); return NUMBER; }

Répéter|Réitérer { return REPEAT; }

si|SI|Si 			{return SI;}
Alors|alors|ALORS 	{return ALORS;}
Sinon|sinon|SINON	{return SINON;}
FinSi|finsi|FINSI	{return FINSI;}

[A-Za-z_][0-9A-Za-z_]*   		{ strcpy(yylval.nom,yytext); return IDENTIFIER; }

\r\n  							{  return '\n'; }	
\r								{  return '\n'; }
\n								{  return '\n'; }

[ \t]							{ }

.								{  return yytext[0]; }

%%

