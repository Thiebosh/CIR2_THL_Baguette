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
  int     intValeur;
  double  doubleValeur;
  char*   stringValeur;
  char*   nom;
  instructAdress adresse;
  instructAdress declareFonction;
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

%token INCREMENT
%token DECREMENT
%token PLUS_CREMENT
%token MOINS_CREMENT
%token FOIS_CREMENT
%token DIVISE_CREMENT

%token AND
%token OR
%token EQUIV
%token DIFF
%token INF_EGAL
%token SUP_EGAL

%token DISPLAY
%token STOP
%token GET_NUMBER
%token GET_TEXT

%token <adresse> IF
%token ELSE
%token <adresse> WHILE
%token <adresse> DO
%token END_WHILE
%token <adresse> FOR
%token <adresse> FOREACH
%token <adresse> FUNCTION //refinstruct : instruction de debut de la fonction
%token END

%token ADD_MEMORY_LAYER
%token REMOVE_MEMORY_LAYER
%token END_PRGM

%left AND OR     /* associativité à gauche */
%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */

%%
program : instructBloc END_PRGM { addInstruct(command::_EXIT_BLOCK_); };

instructBloc : /*Epsilon*/ | instruction '\n' instructBloc ;

instruction :
    /* Ligne vide*/
    | memoryBloc
    | affectVar
    | IO
    | structure
    ;

memoryBloc :
    ADD_MEMORY_LAYER { addInstruct(command::_ENTER_BLOCK_); }
    instructBloc
    REMOVE_MEMORY_LAYER {addInstruct(command::_EXIT_BLOCK_); } //garbage collector
    ;

IO :
      DISPLAY output
    | STOP         { addInstruct(command::_STOP_); }
    ;

output : value { addInstruct(command::_PRINT_); } output_inter ;
output_inter : /*Epsilon*/ | ',' output ;

oneCrement :
      INCREMENT VARIABLE_NAME   { 
                                    addInstruct(command::_EMPILE_VALUE_,(int)1);
                                    addInstruct(command::_PLUS_CREMENT_,$2);
                                }
    | DECREMENT VARIABLE_NAME   { 
                                    addInstruct(command::_EMPILE_VALUE_,(int)1);
                                    addInstruct(command::_MOINS_CREMENT_,$2);
                                }
    
    | INCREMENT VARIABLE_NAME '['INT_VALUE']'   { 
                                                    addInstruct(command::_EMPILE_VALUE_,(int)1);
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                                    addInstruct(command::_PLUS_CREMENT_,$2); 
                                                }
    | DECREMENT VARIABLE_NAME '['INT_VALUE']'   {
                                                    addInstruct(command::_EMPILE_VALUE_,(int)1);
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                                    addInstruct(command::_MOINS_CREMENT_,$2); 
                                                }
    ;

valCrement :
      VARIABLE_NAME PLUS_CREMENT    value { addInstruct(command::_PLUS_CREMENT_,$1); }
    | VARIABLE_NAME MOINS_CREMENT   value { addInstruct(command::_MOINS_CREMENT_,$1); }
    | VARIABLE_NAME FOIS_CREMENT    value { addInstruct(command::_FOIS_CREMENT_,$1); }
    | VARIABLE_NAME DIVISE_CREMENT  value { addInstruct(command::_DIVISE_CREMENT_,$1); }

    | VARIABLE_NAME'['INT_VALUE']' PLUS_CREMENT    value    { 
                                                                addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                                                addInstruct(command::_PLUS_CREMENT_,$1);//ajouter verif : si pas var, cherche dans tab et recupere indice tab
                                                            }
    | VARIABLE_NAME'['INT_VALUE']' MOINS_CREMENT   value    { 
                                                                addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                                                addInstruct(command::_MOINS_CREMENT_,$1); 
                                                            }
    | VARIABLE_NAME'['INT_VALUE']' FOIS_CREMENT    value    { 
                                                                addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                                                addInstruct(command::_FOIS_CREMENT_,$1);
                                                            }
    | VARIABLE_NAME'['INT_VALUE']' DIVISE_CREMENT  value    { 
                                                                addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                                                addInstruct(command::_DIVISE_CREMENT_,$1);
                                                            }
    ;

varCrement : oneCrement | valCrement ;

affectVar :
    VARIABLE_NAME INT '=' value         {
                                            addInstruct(command::_EMPILE_VALUE_,(int)1);//type var
                                            addInstruct(command::_CREATE_VARIABLE_,$1);//nom var
                                        }
    | VARIABLE_NAME DOUBLE '=' value    { 
                                            addInstruct(command::_EMPILE_VALUE_,(double)1);//type var
                                            addInstruct(command::_CREATE_VARIABLE_,$1);//nom var
                                        }
    | VARIABLE_NAME STRING '=' value    { 
                                            addInstruct(command::_EMPILE_VALUE_,(string)"");//type var
                                            addInstruct(command::_CREATE_VARIABLE_,$1);//nom var
                                        }
    | VARIABLE_NAME '=' value           { addInstruct(command::_UPDATE_VARIABLE_,$1); } //nom var
    
    | valCrement

    | VARIABLE_NAME TAB INT '=' value           { 
                                                    addInstruct(command::_EMPILE_VALUE_,(string)$1);//nom tab
                                                    addInstruct(command::_CREATE_TABLE_,(int)1);//type var
                                                }
    | VARIABLE_NAME TAB DOUBLE '=' value        { 
                                                    addInstruct(command::_EMPILE_VALUE_,(string)$1);//nom tab
                                                    addInstruct(command::_CREATE_TABLE_,(double)1);//type var
                                                }
    | VARIABLE_NAME TAB STRING '=' value        { 
                                                    addInstruct(command::_EMPILE_VALUE_,(string)$1);//nom tab
                                                    addInstruct(command::_CREATE_TABLE_,"");//type var
                                                }
    | VARIABLE_NAME'['']' '=' value             { addInstruct(command::_ADD_TABLE_ELEMENT_,$1); } //nom tab
    | VARIABLE_NAME'['INT_VALUE']' '=' value    { 
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                                    addInstruct(command::_UPDATE_TABLE_ELEMENT_,$1);//nom tab
                                                  }

    | DELETE VARIABLE_NAME'['INT_VALUE']'   { 
                                                addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                                addInstruct(command::_REMOVE_TABLE_ELEMENT_,$2);//nom tab
                                            }
    ;

value :
      '(' value ')'
    
    | INT_VALUE       { addInstruct(command::_EMPILE_VALUE_,(int)$1); }
    | DOUBLE_VALUE    { addInstruct(command::_EMPILE_VALUE_,(double)$1); }
    | STRING_VALUE    { addInstruct(command::_EMPILE_VALUE_,(string)$1); }
    
    | SIZE VARIABLE_NAME            { addInstruct(command::_EMPILE_TABLE_SIZE_,$2); }

    | value '+' value     { addInstruct(command::_PLUS_);}
    | value '-' value     { addInstruct(command::_MOINS_);}
    | value '*' value     { addInstruct(command::_FOIS_);}
    | value '/' value     { addInstruct(command::_DIVISE_PAR_);}
    
    | VARIABLE_NAME                 { addInstruct(command::_EMPILE_VARIABLE_,$1); }
    | VARIABLE_NAME'['INT_VALUE']'  { 
                                      addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                      addInstruct(command::_EMPILE_TABLE_ELEMENT_,$1);//nom tab
                                    }

    | oneCrement
    ;

logic_test :
      '[' logic_test ']'
    | logic_test AND logic_test { addInstruct(command::_AND_);}
    | logic_test OR logic_test  { addInstruct(command::_OR_);}

    | value                 { 
                                addInstruct(command::_EMPILE_VALUE_,(int)0);
                                addInstruct(command::_DIFF_);
                            }
    | '!' value             {
                                addInstruct(command::_EMPILE_VALUE_,(int)0);
                                addInstruct(command::_EQUIV_);
                            }
    | value EQUIV value     { addInstruct(command::_EQUIV_);}
    | value DIFF value      { addInstruct(command::_DIFF_);}
    | value '>' value       { addInstruct(command::_INFERIEUR_);}
    | value '<' value       { addInstruct(command::_SUPERIEUR_);}
    | value INF_EGAL value  { addInstruct(command::_INF_EGAL_);}
    | value SUP_EGAL value  { addInstruct(command::_SUP_EGAL_);}
    ;


structure :
    IF                  { addInstruct(command::_ENTER_BLOCK_); }
      logic_test '\n'   {
                                //apres interpretation de operation :
                            $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans then, sinon sauter à <adresse fin then / debut else>)
                        }
      instructBloc      { 
                                //apres interpretation de bloc :
                            $1.refInstruct = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse fin else / debut end_if>)

                            instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse fin then / debut else>
                        }
      bloc_else END     { 
                            instructionList[$1.refInstruct].second.intVal = instructionList.size();//<adresse fin else / debut end_if>
                            addInstruct(command::_EXIT_BLOCK_);
                        }

    | WHILE             { 
                            addInstruct(command::_ENTER_BLOCK_);
                            $1.refInstruct = instructionList.size();//<adresse test>
                        }
      logic_test '\n'   {
                                //apres interpretation de operation :
                            $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                        }
      instructBloc END  {
                                //apres interpretation de bloc :
                            addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                            instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                            instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                        
                            addInstruct(command::_EXIT_BLOCK_);
                        }

    | DO                                {
                                            addInstruct(command::_ENTER_BLOCK_);
                                            $1.refInstruct = instructionList.size();//<adresse debut>
                                        }
      instructBloc END_WHILE logic_test {//apres interpretation de operation :
                                            addInstruct(command::_EMPILE_VALUE_,(int)0);
                                            addInstruct(command::_EQUIV_);
                                            addInstruct(command::_GOTO_TEST_);/*testnot0*///realise cette instruction (sauter à <adresse debut> si vrai, quitter sinon)
                                            instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse debut>
                                            
                                            addInstruct(command::_EXIT_BLOCK_);
                                        }

    | FOR                                   { addInstruct(command::_ENTER_BLOCK_); }
      affectVar                             { $1.refInstruct = instructionList.size();/*<adresse test>*/ }
      ':' logic_test                        { 
                                                $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                                                addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                                            } 
      ':' varCrement '\n' instructBloc END   { 
                                                addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                                                instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                                                instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                                                
                                                addInstruct(command::_EXIT_BLOCK_);
                                            }
    ;

bloc_else : /* Epsilon */ | ELSE '\n' instructBloc ;
%%
/*
functions :
    FUNCTION type {createFunction}//se charge d'enregistrer instruct ref
      '(' arguments ')'
      instructBloc
      END

    | FUNCTION '(' valeurs ')' {call function} //se charge d'ajouter couche mémoire
        END { exit function} //se charge de retirer couche memoire
      */

int main(int argc, char **argv) {
  if (!folderExist()) exit(0);//ne peut pas fonctionner sans

  if ((yyin = programGeneration(argc, argv)) == NULL) exit(0);//ne peut pas fonctionner sans
  yyparse();

  displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}
