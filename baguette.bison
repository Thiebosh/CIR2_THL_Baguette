/*fichier bison : parse*/
%{
  #include "storage.cpp"
  #include "processing.cpp"

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char const *s) { fprintf (stderr, "%s\n", s); return 1; }//preciser erreurs? https://www.gnu.org/software/bison/manual/html_node/Error-Reporting.html
%}

%union{//variables
  int intValeur;
  double doubleValeur;
  char* stringValeur;
  char nom[50];
  idInstruct adresse;
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

%token JUMP 
%token JUMP_IF_ZERO

%token <adresse> IF
%token THEN
%token ELSE
%token END_IF

%token REPEAT

%token END_PRGM

%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */


%%
programme :
    bloc '\n'
    | END_PRGM            { 
                            addInstruct(command::_EXIT_BLOCK_);
                            exit(0);
                          }
    | /* vide */
    ;

bloc :
    bloc instruction '\n' //pas dans l'autre sens? 
    |    /* Epsilon */
    ;

instruction : 
    INT    VARIABLE_NAME '=' expression { 
                                            //var name forcement string donc passe en type l'expression (type de var) pour savoir quel tableau checker
                                            addInstruct(command::_CREATE_VARIABLE_, { valType::_int_,   (int)stringList.size() });
                                            stringList.push_back($2);
                                          }
    | DOUBLE VARIABLE_NAME '=' expression { 
                                            addInstruct(command::_CREATE_VARIABLE_, { valType::_double_,(int)stringList.size() }); 
                                            stringList.push_back($2); 
                                          }
    | STRING VARIABLE_NAME '=' expression { 
                                            addInstruct(command::_CREATE_VARIABLE_, { valType::_string_,(int)stringList.size() }); 
                                            stringList.push_back($2); 
                                          }
    
    | VARIABLE_NAME '=' expression  { 
                                      addInstruct(command::_UPDATE_VARIABLE_, { valType::_string_,(int)stringList.size() }); 
                                      stringList.push_back($1); 
                                    }
    
    | TAB INT     VARIABLE_NAME   '=' expression  { 
                                                    addInstruct(command::_CREATE_VAR_TABLE_, { valType::_int_,(int)stringList.size() });
                                                    stringList.push_back($3);
                                                  }
    | TAB DOUBLE  VARIABLE_NAME   '=' expression  { 
                                                    addInstruct(command::_CREATE_VAR_TABLE_, { valType::_double_,(int)stringList.size() });
                                                    stringList.push_back($3);
                                                  }
    | TAB STRING  VARIABLE_NAME   '=' expression  { 
                                                    addInstruct(command::_CREATE_VAR_TABLE_, { valType::_string_,(int)stringList.size() });
                                                    stringList.push_back($3);
                                                  }
    
    | VARIABLE_NAME'['INT_VALUE']' '=' expression  { 
                                                    //index tab
                                                    addInstruct(command::_EMPILE_VALUE_, { valType::_int_,   (int)intList.size() });
                                                    intList.push_back($3);

                                                    //nom var + contenu expression (index dispo dans pile)
                                                    addInstruct(command::_UPDATE_TABLE_ELEMENT_, { valType::_string_,(int)stringList.size() }); 
                                                    stringList.push_back($1);
                                                  }
    
    | DELETE VARIABLE_NAME'['INT_VALUE']'         { 
                                                    //index tab
                                                    addInstruct(command::_EMPILE_VALUE_, { valType::_int_,   (int)intList.size() });
                                                    intList.push_back($4);

                                                    //nom var
                                                    addInstruct(command::_REMOVE_TABLE_ELEMENT_, { valType::_string_,(int)stringList.size() });
                                                    stringList.push_back($2);
                                                  }

    | DISPLAY expression { addInstruct(command::_PRINT_);   /* imprimer le résultat de l'expression */  }

    | IF expression '\n'  { //refaire clairement (ajouter comparaison?)
                            addInstruct(command::_ENTER_BLOCK_);

                            $1.jumpToInstruct = indexInstruction; //enregistre position dans le IF
                            addInstruct(command::_JUMP_IF_ZERO_); //saute si prochaine valeur empilee (expression) vaut 0
                          }
      THEN '\n' bloc      { 
                            $1.jumpToInstructIfFalse = indexInstruction; // toujours le IF
                            addInstruct(command::_JUMP_);
                            instructionList[$1.jumpToInstruct].second = { valType::_int_,(int)intList.size() }; /*enregistre position*/
                            intList.push_back(indexInstruction);
                          }
      ELSE '\n' bloc      { 
                            instructionList[$1.jumpToInstructIfFalse].second = { valType::_int_,(int)intList.size() }; /*enregistre position*/
                            intList.push_back(indexInstruction);
                          }
      END_IF              { addInstruct(command::_EXIT_BLOCK_); }

    | REPEAT '(' expression ')' bloc { /* TO DO */ }

    |   /* Ligne vide*/
    ;

expression :
    '(' expression ')'   { } //reduit expression

    | expression '+' expression     { addInstruct(command::_PLUS_);}
    | expression '-' expression     { addInstruct(command::_MOINS_);}
    | expression '*' expression     { addInstruct(command::_FOIS_);}
    | expression '/' expression     { addInstruct(command::_DIVISE_PAR_);}
    
    | INT_VALUE       { 
                        addInstruct(command::_EMPILE_VALUE_, { valType::_int_,   (int)intList.size() });
                        intList.push_back($1);
                      }
    | DOUBLE_VALUE    { 
                        addInstruct(command::_EMPILE_VALUE_, { valType::_double_,(int)doubleList.size() }); 
                        doubleList.push_back($1); 
                      }
    | STRING_VALUE    { 
                        addInstruct(command::_EMPILE_VALUE_, { valType::_string_,(int)stringList.size() }); 
                        stringList.push_back($1); 
                      }
    
    | VARIABLE_NAME   { 
                        addInstruct(command::_EMPILE_VARIABLE_, { valType::_string_,(int)stringList.size() });
                        stringList.push_back($1); 
                      }
    
    | SIZE VARIABLE_NAME    { 
                              //nom var
                              addInstruct(command::_EMPILE_TABLE_SIZE_, { valType::_string_,(int)stringList.size() });
                              stringList.push_back($2);
                            }
    | VARIABLE_NAME'['INT_VALUE']'    { 
                                        //index tab
                                        addInstruct(command::_EMPILE_VALUE_, { valType::_int_,   (int)intList.size() });
                                        intList.push_back($3);

                                        //nom var
                                        addInstruct(command::_EMPILE_TABLE_ELEMENT_, { valType::_string_,(int)stringList.size() });
                                        stringList.push_back($1);
                                      }

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

  //displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}
