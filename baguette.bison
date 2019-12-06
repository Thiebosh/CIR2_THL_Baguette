/*fichier bison : parse*/
%{
  #include "files.cpp"

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

%token ADD_MEMORY_LAYER
%token REMOVE_MEMORY_LAYER

%token <intValeur>    INT_VALUE
%token <doubleValeur> DOUBLE_VALUE
%token <stringValeur> STRING_VALUE

%token VOID
%token INT
%token DOUBLE
%token STRING
%token TAB
%token <nom> NAME

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

%token WRITE
%token STOP
%token INPUT

%token <adresse> IF
%token <adresse> ELIF
%token ELSE
%token <adresse> WHILE
%token <adresse> DO
%token <adresse> FOR
%token <adresse> FOREACH

%token RETURN
%token END_PRGM

%left AND OR     /* associativité à gauche */
%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */

%%
program : { addInstruct(command::_ENTER_FUNCTION_); } instructBloc END_PRGM { addInstruct(command::_EXIT_FUNCTION_); };

endline : '\n' | ';' ;

instructBloc : /*Epsilon*/ | instruction endline instructBloc ;

instruction :
    /* Ligne vide*/
    | memoryBloc
    | affectVar
    | IO
    | condition
    | boucle
    | function
    ;

memoryBloc :
    ADD_MEMORY_LAYER    { addInstruct(command::_ENTER_BLOCK_); }
    instructBloc
    REMOVE_MEMORY_LAYER {addInstruct(command::_EXIT_BLOCK_); } //garbage collector
    ;

IO :
      WRITE output
    | STOP          { addInstruct(command::_STOP_); }
    | INPUT NAME    { addInstruct(command::_READ_,$2); }
    ;

output : value { addInstruct(command::_WRITE_); } output_inter ;
output_inter : /*Epsilon*/ | ',' output ;

oneCrement :
      INCREMENT NAME   { 
                            addInstruct(command::_EMPILE_VALUE_,(int)1);
                            addInstruct(command::_PLUS_CREMENT_,$2);
                        }
    | DECREMENT NAME   { 
                            addInstruct(command::_EMPILE_VALUE_,(int)1);
                            addInstruct(command::_MOINS_CREMENT_,$2);
                        }
    
    | INCREMENT NAME '['INT_VALUE']'   { 
                                            addInstruct(command::_EMPILE_VALUE_,(int)1);
                                            addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                            addInstruct(command::_PLUS_CREMENT_,$2); 
                                        }
    | DECREMENT NAME '['INT_VALUE']'   {
                                            addInstruct(command::_EMPILE_VALUE_,(int)1);
                                            addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                            addInstruct(command::_MOINS_CREMENT_,$2); 
                                        }
    ;

valCrement :
      value PLUS_CREMENT    NAME { addInstruct(command::_PLUS_CREMENT_,$3); }
    | value MOINS_CREMENT   NAME { addInstruct(command::_MOINS_CREMENT_,$3); }
    | value FOIS_CREMENT    NAME { addInstruct(command::_FOIS_CREMENT_,$3); }
    | value DIVISE_CREMENT  NAME { addInstruct(command::_DIVISE_CREMENT_,$3); }

    | value PLUS_CREMENT NAME'['INT_VALUE']'    { 
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$5);//index tab
                                                    addInstruct(command::_PLUS_CREMENT_,$3);//ajouter verif : si pas var, cherche dans tab et recupere indice tab
                                                }
    | value MOINS_CREMENT NAME'['INT_VALUE']'   { 
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$5);//index tab
                                                    addInstruct(command::_MOINS_CREMENT_,$3); 
                                                }
    | value FOIS_CREMENT NAME'['INT_VALUE']'    { 
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$5);//index tab
                                                    addInstruct(command::_FOIS_CREMENT_,$3);
                                                }
    | value DIVISE_CREMENT NAME'['INT_VALUE']'  { 
                                                    addInstruct(command::_EMPILE_VALUE_,(int)$5);//index tab
                                                    addInstruct(command::_DIVISE_CREMENT_,$3);
                                                }
    ;

varCrement : oneCrement | valCrement ;

type :
      INT       { addInstruct(command::_EMPILE_VALUE_,(int)1); }
    | DOUBLE    { addInstruct(command::_EMPILE_VALUE_,(double)1); }
    | STRING    { addInstruct(command::_EMPILE_VALUE_,""); }
    ;

affectVar :
    value '=' type NAME { addInstruct(command::_CREATE_VARIABLE_,$4); }
    | value '=' NAME    { addInstruct(command::_UPDATE_VARIABLE_,$3); } //nom var

    | valCrement

    | value '=' TAB type NAME       { addInstruct(command::_CREATE_TABLE_,$5); }//nom tab
    | value '=' NAME'['']'          { addInstruct(command::_ADD_TABLE_ELEMENT_,$3); } //nom tab
    | value '=' NAME'['INT_VALUE']' {
                                        addInstruct(command::_EMPILE_VALUE_,(int)$5);//index tab
                                        addInstruct(command::_UPDATE_TABLE_ELEMENT_,$3);//nom tab
                                    }
    | DELETE NAME'['INT_VALUE']'    {
                                        addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                        addInstruct(command::_REMOVE_TABLE_ELEMENT_,$2);//nom tab
                                    }
    ;

value :
      '(' value ')'
    
    | INT_VALUE       { addInstruct(command::_EMPILE_VALUE_,(int)$1); }
    | DOUBLE_VALUE    { addInstruct(command::_EMPILE_VALUE_,(double)$1); }
    | STRING_VALUE    { addInstruct(command::_EMPILE_VALUE_,(string)$1); }
    
    | SIZE NAME            { addInstruct(command::_EMPILE_TABLE_SIZE_,$2); }

    | value '+' value     { addInstruct(command::_PLUS_);}
    | value '-' value     { addInstruct(command::_MOINS_);}
    | value '*' value     { addInstruct(command::_FOIS_);}
    | value '/' value     { addInstruct(command::_DIVISE_PAR_);}
    
    | NAME                 { addInstruct(command::_EMPILE_VARIABLE_,$1); }
    | NAME'['INT_VALUE']'  { 
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


condition :
    IF              { addInstruct(command::_ENTER_BLOCK_); }
    logic_test '{' {
                        $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                        addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans then, sinon sauter à <adresse fin then / debut else>)
                    }
    instructBloc    { 
                        $1.refInstruct = instructionList.size();//quand arrive à ce numero d'instruction :
                        addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse fin else / debut end_if>)

                        instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse fin then / debut else>
                    }
    bloc_else '}'   {
                        instructionList[$1.refInstruct].second.intVal = instructionList.size();//<adresse fin else / debut end_if>
                        addInstruct(command::_EXIT_BLOCK_);
                    }
    ;//ELIF ... ?
bloc_else : /* Epsilon */ | ELSE '\n' instructBloc ;

boucle :
    WHILE               { 
                            addInstruct(command::_ENTER_BLOCK_);
                            $1.refInstruct = instructionList.size();//<adresse test>
                        }
      logic_test '{'    {
                                //apres interpretation de operation :
                            $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                            addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                        }
      instructBloc '}'  {
                                //apres interpretation de bloc :
                            addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                            instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                            instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                        
                            addInstruct(command::_EXIT_BLOCK_);
                        }

    | DO '{'                        {
                                        addInstruct(command::_ENTER_BLOCK_);
                                        $1.refInstruct = instructionList.size();//<adresse debut>
                                    }
      instructBloc '}' logic_test   {
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
      ':' varCrement '{' instructBloc '}'   { 
                                                addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                                                instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                                                instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                                                
                                                addInstruct(command::_EXIT_BLOCK_);
                                            }
    ;


function :
    NAME type               { addInstruct(command::_EMPILE_VALUE_,(int)-1); }//guette -1 pour fin de declaration des parametres
      '(' argument ')' '{'  {
                                addInstruct(command::_EMPILE_VALUE_,(int)instructionList.size() + 2);//adresse debut fonction
                                addInstruct(command::_CREATE_FUNCTION_,$1);//nom de fonction,todo
                                addInstruct(command::_ENTER_FUNCTION_,$1);
                            }
      instructBloc '}'      { /*cas void, pas necessairement de return : addInstruct(command::_EXIT_FUNCTION_);*/ }

    | NAME              { addInstruct(command::_EMPILE_VALUE_,(int)-1); } //guette -1 pour fin de declaration des parametres
      '(' argument ')'  { addInstruct(command::_CALL_FUNCTION_,$1); }

    | RETURN value      { addInstruct(command::_EXIT_FUNCTION_); }
    ;

argument : /*Epsilon*/ | NAME type argument_inter { addInstruct(command::_CREATE_VARIABLE_,$1); };
argument_inter : /*Epsilon*/ | ',' argument ;
    
%%

int main(int argc, char **argv) {
    //commence par verifier si argument (dossier)
    if (!folderExist()) exit(0);//ne peut pas fonctionner sans

    if ((yyin = programGeneration(argc, argv)) == NULL) exit(0);//ne peut pas fonctionner sans
    yyparse();

    displayGeneratedProgram();

    saveCommandProgramFile();//debug

    executeGeneratedProgram();

    return 0;
}
