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
%token END

%token END_PRGM

%left AND OR     /* associativité à gauche */
%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */

%%
program : bloc END_PRGM { addInstruct(command::_EXIT_BLOCK_); };

bloc : instruction '\n' bloc | /*Epsilon*/ ;

instruction : 
    IO
    | operation
    | affectation
    | structure
    
    | DELETE VARIABLE_NAME'['INT_VALUE']' { 
                                            addInstruct(command::_EMPILE_VALUE_,(int)$4);//index tab
                                            addInstruct(command::_REMOVE_TABLE_ELEMENT_,$2);//nom tab
                                          }
    |   /* Ligne vide*/
    ;

IO :
    DISPLAY output
    | STOP         { addInstruct(command::_STOP_); }
    ;

output : 
    value { addInstruct(command::_PRINT_); } output_inter;
output_inter : ',' output | /*Epsilon*/ ;

var_tab_update : // l un ou l autre
      /*Epsilon*/     { addInstruct(command::_UPDATE_VARIABLE_); }
    | '['INT_VALUE']' { 
                        addInstruct(command::_EMPILE_VALUE_,(int)$2);//index tab
                        addInstruct(command::_UPDATE_TABLE_ELEMENT_);
                      }
    ;

var_type :
      var_tab_update
    | '['']'          { addInstruct(command::_ADD_TABLE_ELEMENT_); }

    | INT         { addInstruct(command::_CREATE_VARIABLE_,(int)1);/*type var*/ }
    | DOUBLE      { addInstruct(command::_CREATE_VARIABLE_,(double)1);/*type var*/ }
    | STRING      { addInstruct(command::_CREATE_VARIABLE_,(string)"");/*type var*/ }

    | TAB INT         { addInstruct(command::_CREATE_TABLE_,(int)1);/*type var*/ }
    | TAB DOUBLE      { addInstruct(command::_CREATE_TABLE_,(double)1);/*type var*/ }
    | TAB STRING      { addInstruct(command::_CREATE_TABLE_,(string)"");/*type var*/ }
    ;

crementation :
      '+''+' VARIABLE_NAME var_tab_update    { addInstruct(command::_INCREMENT_,$3); }
    | '-''-' VARIABLE_NAME var_tab_update    { addInstruct(command::_DECREMENT_,$3); }
    ;

affectation : affectation_simple | affectation_crement ;

affectation_simple : value '=' VARIABLE_NAME { addInstruct(command::_EMPILE_VALUE_,$3); } var_type;

affectation_crement :
      value PLUS_CREMENT     VARIABLE_NAME var_tab_update  { addInstruct(command::_PLUS_CREMENT_,  $3); }
    | value MOINS_CREMENT    VARIABLE_NAME var_tab_update  { addInstruct(command::_MOINS_CREMENT_, $3); }
    | value FOIS_CREMENT     VARIABLE_NAME var_tab_update  { addInstruct(command::_FOIS_CREMENT_,  $3); }
    | value DIVISE_CREMENT   VARIABLE_NAME var_tab_update  { addInstruct(command::_DIVISE_CREMENT_,$3); }
    ;

value : operation | logic_test ;

operation :
      '(' operation ')'   { }

    | INT_VALUE     { addInstruct(command::_EMPILE_VALUE_,(int)$1); }
    | DOUBLE_VALUE  { addInstruct(command::_EMPILE_VALUE_,(double)$1); }
    | STRING_VALUE  { addInstruct(command::_EMPILE_VALUE_,(string)$1); }

    | operation '+' operation   { addInstruct(command::_PLUS_);}
    | operation '-' operation   { addInstruct(command::_MOINS_);}
    | operation '*' operation   { addInstruct(command::_FOIS_);}
    | operation '/' operation   { addInstruct(command::_DIVISE_PAR_);}

    | VARIABLE_NAME                 { addInstruct(command::_EMPILE_VARIABLE_,$1); }
    | VARIABLE_NAME'['INT_VALUE']'  { 
                                      addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                      addInstruct(command::_EMPILE_TABLE_ELEMENT_,$1);//nom tab
                                    }
    | SIZE VARIABLE_NAME            { addInstruct(command::_EMPILE_TABLE_SIZE_,$2); }

    | crementation
    ;

    
logic_test :
      '(' logic_test ')'   { }
    | logic_test AND logic_test { addInstruct(command::_AND_);}
    | logic_test OR  logic_test { addInstruct(command::_OR_);}
    | comparaison
    ;


comparaison :
      operation                     { 
                                      addInstruct(command::_EMPILE_VALUE_,(int)0);
                                      addInstruct(command::_DIFF_);
                                    }
    | '!' operation                 {
                                      addInstruct(command::_EMPILE_VALUE_,(int)0);
                                      addInstruct(command::_EQUIV_);
                                    }
    | operation EQUIV    operation  { addInstruct(command::_EQUIV_);}
    | operation DIFF     operation  { addInstruct(command::_DIFF_);}
    | operation '>'      operation  { addInstruct(command::_INFERIEUR_);}
    | operation '<'      operation  { addInstruct(command::_SUPERIEUR_);}
    | operation INF_EGAL operation  { addInstruct(command::_INF_EGAL_);}
    | operation SUP_EGAL operation  { addInstruct(command::_SUP_EGAL_);}
    ;


structure :
      IF             { addInstruct(command::_ENTER_BLOCK_); }
      logic_test '\n'{
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
      bloc_else END   { 
                        instructionList[$1.refInstruct].second.intVal = instructionList.size();//<adresse fin else / debut end_if>
                        addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/
                      }

    | WHILE           { 
                        addInstruct(command::_ENTER_BLOCK_);
                        $1.refInstruct = instructionList.size();//<adresse test>
                      }
      logic_test '\n'{
                            //apres interpretation de operation :
                        $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                        addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                      }
      bloc END        {
                            //apres interpretation de bloc :
                        addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                        instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                        instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                      
                        addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/
                      }

    | DO                          {
                                    addInstruct(command::_ENTER_BLOCK_);
                                    $1.refInstruct = instructionList.size();//<adresse debut>
                                  }
      bloc END_WHILE logic_test {//apres interpretation de operation :
                                    addInstruct(command::_GOTO_TEST_INV_);/*testnot0*///realise cette instruction (sauter à <adresse debut> si vrai, quitter sinon)
                                    instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse debut>
                                    
                                    addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/
                                  }

    | FOR                           { addInstruct(command::_ENTER_BLOCK_); }
      VARIABLE_NAME { addInstruct(command::_EMPILE_VALUE_,$3); } var_type            { $1.refInstruct = instructionList.size();/*<adresse test>*/ }
      ':' logic_test                { //FIN
                                      $1.refInstructTest = instructionList.size();//quand arrive à ce numero d'instruction :
                                      addInstruct(command::_GOTO_TEST_);//realise cette instruction (si vrai : continuer dans bloc, sinon sauter à <adresse end while>)
                                    } 
      ':' operation '\n' bloc END {/pas
                                      addInstruct(command::_GOTO_);//realise cette instruction (sauter à <adresse test>)
                                      instructionList[instructionList.size()-1].second.intVal = $1.refInstruct;//<adresse test>

                                      instructionList[$1.refInstructTest].second.intVal = instructionList.size();//<adresse end while>
                                    
                                      addInstruct(command::_EXIT_BLOCK_);/*garbage collector*/
                                    }
    ;

bloc_else : ELSE '\n' bloc | /* Epsilon */ ;

for_step : affectation | crementation ;
%%


int main(int argc, char **argv) {
  if (!folderExist()) exit(0);//ne peut pas fonctionner sans

  if ((yyin = programGeneration(argc, argv)) == NULL) exit(0);//ne peut pas fonctionner sans
  yyparse();

  displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}
