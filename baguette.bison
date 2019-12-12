/*fichier bison : parse*/
%{
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
%token ERASE

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
%token DELAY

%token <adresse> IF
%token ELSE
%token <adresse> WHILE
%token <adresse> DO
%token <adresse> FOR
%token <adresse> FUNCTION

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
    | function_declare
    | function_use
    | function_return
    ;

memoryBloc :
    ADD_MEMORY_LAYER    { addInstruct(command::_ENTER_BLOCK_); }
    instructBloc
    REMOVE_MEMORY_LAYER {addInstruct(command::_EXIT_BLOCK_); } //garbage collector
    ;

IO :
      WRITE output
    | STOP              { addInstruct(command::_STOP_); }
    | INPUT NAME        { addInstruct(command::_READ_,$2); }
    | DELAY INT_VALUE   { addInstruct(command::_DELAY_,$2); }
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
    | ERASE NAME                    { addInstruct(command::_CLEAR_TABLE_,$2); }//nom tab
    ;

value :
      '(' value ')'
    
    | INT_VALUE     { addInstruct(command::_EMPILE_VALUE_,(int)$1); }
    | DOUBLE_VALUE  { addInstruct(command::_EMPILE_VALUE_,(double)$1); }
    | STRING_VALUE  { addInstruct(command::_EMPILE_VALUE_,(string)$1); }
    
    | SIZE NAME     { addInstruct(command::_EMPILE_TABLE_SIZE_,$2); }

    | value '+' value   { addInstruct(command::_PLUS_);}
    | value '-' value   { addInstruct(command::_MOINS_);}
    | value '*' value   { addInstruct(command::_FOIS_);}
    | value '/' value   { addInstruct(command::_DIVISE_PAR_);}
    
    | NAME                 { addInstruct(command::_EMPILE_VARIABLE_,$1); }
    | NAME'['INT_VALUE']'  { 
                                addInstruct(command::_EMPILE_VALUE_,(int)$3);//index tab
                                addInstruct(command::_EMPILE_TABLE_ELEMENT_,$1);//nom tab
                            }

    | oneCrement

    | function_use
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

function_type : type | VOID ;

function_declare :
    FUNCTION NAME function_type     { addInstruct(command::_EMPILE_VALUE_,(int)-1); }//guette -1 pour fin de declaration des parametres
      '(' argument_declare ')' '{'  {
                                        addInstruct(command::_EMPILE_VALUE_,(int)instructionList.size() + 3);//adresse debut fonction
                                        addInstruct(command::_CREATE_FUNCTION_,$2);//nom de fonction
                                        
                                        $1.refInstruct = instructionList.size();//quand arrive à ce numero d'instruction : saute
                                        addInstruct(command::_GOTO_);

                                        addInstruct(command::_ENTER_FUNCTION_,$2);
                                    }
      instructBloc '}'              { 
                                        addInstruct(command::_END_FUNCTION_);//cas void, pas necessairement de return
                                        instructionList[$1.refInstruct].second.intVal = instructionList.size();
                                    }
    ;

function_use :
    NAME                { addInstruct(command::_EMPILE_VALUE_,(int)-1); } //guette -1 pour fin de declaration des parametres
      '(' argument_use ')'  { addInstruct(command::_CALL_FUNCTION_,$1); }
    ;

function_return : RETURN value { addInstruct(command::_EXIT_FUNCTION_); } ;

argument_declare : /*Epsilon*/ | NAME type { addInstruct(command::_EMPILE_VALUE_,$1); } argument_declare_inter ;
argument_declare_inter : /*Epsilon*/ | ',' argument_declare ;

argument_use : /*Epsilon*/ | value argument_use_inter ;
argument_use_inter : /*Epsilon*/ | ',' argument_use ;
    
%%

int main(int argc, char **argv) {
    ++argv, --argc;

    string folderName = DEFAULT_FOLDER;
    if (argc) {
        folderName = argv[0];//si donne autre dossier, remplace
        if (folderName[folderName.size() - 1] != '/') folderName += "/";//le / assure que c'est un dossier
    }


    do {
        //initialise varriables
		instructionList.clear();
        globalVariables allVariables = {};


        //teste acces au dossier (ne peut pas fonctionner sans)
        if (access(folderName.c_str(), F_OK) == -1) {
            cout << "Dossier de stockage " << folderName << " non trouvé : création en cours... ";

            if (mkdir(folderName.c_str(), 0777)) error(allVariables, errorCode::folderCreationFailed);
            else pauseProcess("Placez votre ou vos fichiers " + (string)PROGRAM_EXTENSION + " et " + (string)COMPILED_EXTENSION + " dans le répertoire créé");
        }


        //recupere contenu dossier
        vector<string> programList;
        vector<string> compiledList;

        DIR *fluxFolder = opendir(folderName.c_str());
        while (struct dirent *fileFolder = readdir(fluxFolder)) {
            string filename = (string)fileFolder->d_name;

            if      (filename.find(PROGRAM_EXTENSION,  filename.size() - ((string)PROGRAM_EXTENSION).size())  !=  string::npos) {
                programList.push_back(filename);
            }
            else if (filename.find(COMPILED_EXTENSION, filename.size() - ((string)COMPILED_EXTENSION).size()) !=  string::npos) {
                compiledList.push_back(filename);
            }
        }
        closedir (fluxFolder);
    

        //affiche contenu dossier
        int i = 0;
	    streamsize digit = 1 + log10(programList.size() + compiledList.size());
        cout << endl << "Dossier " << folderName << " : Fichiers programmes (compiler)" << endl;
        for (auto file : programList)  cout << setw(digit) << ++i << " - \"" << file << "\"" << endl;

        cout << endl << "Dossier " << folderName << " : Programmes compilés (exécuter)" << endl;
        for (auto file : compiledList) cout << setw(digit) << ++i << " - \"" << file << "\"" << endl;


        //choisit fichier à traiter
        int saisie;
        cout << endl << "Votre sélection (0 pour quitter) : ";
        do cin >> saisie; while (saisie < -1 || saisie > programList.size() + compiledList.size());

        string programName = "", operation = "";
        if (--saisie < 0) break;
        else if (saisie < programList.size()) {
            programName = programList[saisie];
            cout << programName << endl;
            operation = "Compilation";

            //execute fichier s'il existe
            if (access((folderName + programName).c_str(), F_OK) != -1) {
                if (yyin = fopen((folderName + programName).c_str(),"r")) {
                    yyparse();
                    pauseProcess("Acquisition de Bison terminée");

                    displayGeneratedProgram(allVariables);
                    
                    //enregistre version compilee
                    programName = programName.substr(0,programName.size() - ((string)PROGRAM_EXTENSION).size());
                    ofstream file((folderName + programName + COMPILED_EXTENSION).c_str());
                    if (file) {
                        for (instruction instructContent : instructionList) {
                            file << endl;
                            file << (int)instructContent.first << " "; 
                            file << (int)instructContent.second.type << " ";
                            file << instructContent.second.intVal << " ";
                            file << instructContent.second.doubleVal << " ";
                            file << "\"" + instructContent.second.stringVal + "\"";
                        }
                        file.close();
                    }

                    //saveCommandProgramFile(folderName, programName);//debug
                }
            }
            else cout << "Echec d'accès au fichier" << endl;
        }
        else {
            programName = compiledList[saisie - programList.size()];
            cout << programName << endl;
            operation = "Acquisition";

            //recupere version compilee
            ifstream file((folderName + programName).c_str());
            if (file) {
                while (file.good()) {//verifie eof, failbit et badbit
                    int getCommand;
                    int getType;
                    int getInt;
                    double getDouble;
                    string getString;

                    file >> getCommand >> getType >> getInt >> getDouble;
                    file.ignore();
                    getline(file,getString);
                    getString = getString.substr(1,getString.size()-2);

                    instructionList.push_back({(command)getCommand,{(valType)getType,getInt,getDouble,getString}});
                }
                file.close();
            }

            //execute version compilee
            cout << endl << "===== EXECUTION =====" << endl;
            while (allVariables.indexInstruction < instructionList.size()) {
                instruction instructContent = instructionList[allVariables.indexInstruction++];
                if (executeCommand.find(instructContent.first) != executeCommand.end()) {
                    (*(executeCommand.at(instructContent.first))) (instructContent.second, allVariables);
                }
                else error(allVariables, errorCode::unknowCommand);
            }
            cout << endl << "=====================" << endl;
        }

        pauseProcess(operation + " terminée");
    } while (true);

    return 0;
}
