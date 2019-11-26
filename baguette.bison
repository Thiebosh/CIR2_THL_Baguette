/*fichier bison : parse*/
%{
  #include "storage.cpp"
  #include "processing.cpp"

  //sauts conditionnels
  typedef struct {
    int refInstruct;
    int refInstructTest;
  } instructAdress;

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char const *s) { fprintf (stderr, "%s\n", s); return 1; }//preciser erreurs? https://www.gnu.org/software/bison/manual/html_node/Error-Reporting.html
%}

%union{//variables
  int intValeur;
  double doubleValeur;
  char* stringValeur;
  char* nom;
  instructAdress adresse;
}

%token <intValeur>    INT_VALUE
%token <doubleValeur> DOUBLE_VALUE
%token <stringValeur> STRING_VALUE

%token INT
%token DOUBLE
%token STRING
%token TAB
%token <nom> VARIABLE_NAME

%token SIZE
%token DELETE

//conserver?
%token SIN
%token TAN
%token SQRT
//fin conserver ?

%token DISPLAY

%token <adresse> IF
%token ELSE
%token <adresse> WHILE
%token <adresse> DO
%token <adresse> REPEAT
%token <adresse> FOREACH
%token END

%token END_PRGM

%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */

%%

program : bloc END_PRGM { addInstruct(command::_EXIT_BLOCK_); };

bloc :
    bloc instruction '\n'
    |    /* Epsilon */
    ;

instruction : 
    operation
    | affectation
    | structure
    
    | DELETE VARIABLE_NAME'['INT_VALUE']' { 
                                            addInstruct(command::_EMPILE_VALUE_,$4);//index tab
                                            addInstruct(command::_REMOVE_TABLE_ELEMENT_,$2);//nom tab
                                          }

    | DISPLAY input

    |   /* Ligne vide*/
    ;
  
input :
    operation { addInstruct(command::_PRINT_); }
    | ','
    ;

operation :
    '(' operation ')'   { } //reduit operation

    /*
      | SIN '(' expr ')'  { $$ = sin($3); cout << "sin(" << $3 << ") = " << $$ << endl; }
      | TAN '(' expr ')'  { $$ = tan($3); cout << "tan(" << $3 << ") = " << $$ << endl; }
      | SQRT '(' expr ')' { $$ = sqrt($3); cout << "sqrt(" << $3 << ") = " << $$ << endl;}
    */

    | operation '+' operation     { addInstruct(command::_PLUS_);}
    | operation '-' operation     { addInstruct(command::_MOINS_);}
    | operation '*' operation     { addInstruct(command::_FOIS_);}
    | operation '/' operation     { addInstruct(command::_DIVISE_PAR_);}
    
    | INT_VALUE       { addInstruct(command::_EMPILE_VALUE_,$1); }
    | DOUBLE_VALUE    { addInstruct(command::_EMPILE_VALUE_,$1); }
    | STRING_VALUE    { addInstruct(command::_EMPILE_VALUE_,$1); }
    
    | VARIABLE_NAME   { addInstruct(command::_EMPILE_VARIABLE_,$1); }
    | VARIABLE_NAME'['INT_VALUE']'    { 
                                        addInstruct(command::_EMPILE_VALUE_,$3);//index tab
                                        addInstruct(command::_EMPILE_TABLE_ELEMENT_,$1);//nom tab
                                      }
    | SIZE VARIABLE_NAME    { addInstruct(command::_EMPILE_TABLE_SIZE_,$2); }
    ;

affectation :
    VARIABLE_NAME INT '=' operation       {
                                            addInstruct(command::_EMPILE_VALUE_,(int)1);//type var
                                            addInstruct(command::_CREATE_VARIABLE_,$1);//nom var
                                          }
    | VARIABLE_NAME DOUBLE '=' operation  { 
                                            addInstruct(command::_EMPILE_VALUE_,(double)1);//type var
                                            addInstruct(command::_CREATE_VARIABLE_,$1);//nom var
                                          }
    | VARIABLE_NAME STRING '=' operation  { 
                                            addInstruct(command::_EMPILE_VALUE_,"");//type var
                                            addInstruct(command::_CREATE_VARIABLE_,$1);//nom var
                                          }
    | VARIABLE_NAME '=' operation         { addInstruct(command::_UPDATE_VARIABLE_,$1);/*nom var*/ }

    | VARIABLE_NAME TAB INT '=' operation     { 
                                                addInstruct(command::_EMPILE_VALUE_,$1);//nom tab
                                                addInstruct(command::_CREATE_TABLE_,(int)1);//type var
                                              }
    | VARIABLE_NAME TAB DOUBLE '=' operation  { 
                                                addInstruct(command::_EMPILE_VALUE_,$1);//nom tab
                                                addInstruct(command::_CREATE_TABLE_,(double)1);//type var
                                              }
    | VARIABLE_NAME TAB STRING '=' operation  { 
                                                addInstruct(command::_EMPILE_VALUE_,$1);//nom tab
                                                addInstruct(command::_CREATE_TABLE_,"");//type var
                                              }
    | VARIABLE_NAME'['']' '=' operation           { addInstruct(command::_ADD_TABLE_ELEMENT_,$1);/*nom tab*/ }
    | VARIABLE_NAME'['INT_VALUE']' '=' operation  { 
                                                    addInstruct(command::_EMPILE_VALUE_,$3);//index tab
                                                    addInstruct(command::_UPDATE_TABLE_ELEMENT_,$1);//nom tab
                                                  }
    ;

structure :
    IF                { addInstruct(command::_ENTER_BLOCK_); }
      operation '\n'  {//ajouter comparaison empilant 0 ou 1
                            //apres interpretation de operation :
                        $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                        addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans then, sinon sauter à <adresse fin then / debut else>)
                      }
      bloc            { 
                            //apres interpretation de bloc :
                        $1.refInstruct = instructionList.size();//quand arrive à ce numero d'instruction :
                        addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse fin else / debut end_if>)

                        instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse fin then / debut else>
                      }
      bloc_else       { instructionList[$1.refInstruct].second.intVal = instructionList.size(); }//<adresse fin else / debut end_if>
      END             { addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/ }

    | WHILE           { 
                        addInstruct(command::_ENTER_BLOCK_);
                        $1.refInstruct = instructionList.size();//<adresse test>
                      }
      operation '\n'  { //ajouter comparaison empilant 0 ou 1
                            //apres interpretation de operation :
                        $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                        addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                      }
      bloc            {
                            //apres interpretation de bloc :
                        addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                        instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                        instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                      }
      END             { addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/ }

    | DO                    {
                              addInstruct(command::_ENTER_BLOCK_);
                              //<adresse debut>
                            }
      bloc WHILE operation  { //ajouter comparaison empilant 0 ou 1
                                  //apres interpretation de operation :
                              //si vrai, saut <adresse debut>
                              //sinon
                              addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/ }

//    | REPEAT '(' operation ')' bloc { /* TO DO */ }
    ;

bloc_else : ELSE '\n' bloc | /* Epsilon */ ;
%%


int main(int argc, char **argv) {
  if (!folderExist()) exit(0);//ne peut pas fonctionner sans

  if ((yyin = programGeneration(argc, argv)) == NULL) exit(0);//ne peut pas fonctionner sans
  yyparse();

  displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}
