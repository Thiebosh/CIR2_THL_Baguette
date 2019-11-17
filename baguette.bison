/*fichier bison : parse*/
%{
  #include "processing.cpp"

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char *s) { printf("%s\n", s); return 0; }
%}

%union{
  double valeur;
  char nom[50];
  idInstruct adresse;
}

%token <valeur> NUMBER
%token <nom> IDENTIFIER
%type <valeur> expression
%token <adresse> IF
%token THEN
%token ELSE
%token END_IF
%token REPEAT
%token JUMP 
%token JUMP_IF_ZERO
%token PRINT
%token SIN
%token TAN
%token SQRT

%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */


%%
bloc :
    bloc instruction '\n' 
    | bloc instruction
    |    /* Epsilon */
    ;

instruction : 
    expression { addInstruct(Command::_PRINT_);   /* imprimer le résultat de l'expression */  }

    | IF expression '\n'  { 
                            $1.jumpToInstruct = indexInstruction;/*enregistre position*/
                            addInstruct(Command::_JUMP_IF_ZERO_);
                          }
      THEN '\n' bloc      { 
                            $1.jumpToInstructIfFalse = indexInstruction;
                            addInstruct(Command::_JUMP_);
                            instructionList[$1.jumpToInstruct].second = indexInstruction;/*enregistre position*/
                          }
      ELSE '\n' bloc      { instructionList[$1.jumpToInstructIfFalse].second = indexInstruction;/*enregistre position*/ }
      END_IF              {   }

    | REPEAT '(' expression ')' expression { /* TO DO */ }

    | IDENTIFIER '=' expression { 
                                  variables[$1] = $3; 
                                  addInstruct(Command::_SET_IDENTIFIER_, $3); 
                                }
    |   /* Ligne vide*/
    ;

expression: 
    expression '+' expression     { addInstruct(Command::_PLUS_);}
    | expression '-' expression     { addInstruct(Command::_MOINS_);}
    | expression '*' expression     { addInstruct(Command::_FOIS_);}
    | expression '/' expression     { addInstruct(Command::_DIVISE_PAR_);}
    | '(' expression ')'            { }
    | NUMBER                        { addInstruct(Command::_NUMBER_, $1);}
    | IDENTIFIER                    { addInstruct(Command::_GET_IDENTIFIER_, variables[$1]);}
  /*
    | SIN '(' expr ')'  { $$ = sin($3); cout << "sin(" << $3 << ") = " << $$ << endl; }
    | TAN '(' expr ')'  { $$ = tan($3); cout << "tan(" << $3 << ") = " << $$ << endl; }
    | SQRT '(' expr ')' { $$ = sqrt($3); cout << "sqrt(" << $3 << ") = " << $$ << endl;}
  */
    ;
%%


int main(int argc, char **argv) {
  if (!folderExist()) exit(0);//ne peut pas fonctionner sans

  if ((yyin = programGeneration(argc, argv)) == NULL) exit(0);//ne peut pas fonctionner sans
  yyparse();

  displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}
