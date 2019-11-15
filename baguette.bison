/*fichier bison : parse*/
%{
  //gestion fichiers et dossiers
  #include <unistd.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <dirent.h>

  #include <iostream>
  #include <iomanip>
  #include <string>
  #include <vector>
  #include <map>
  #include <cmath>

  #define FOLDER "programFiles/"
  #define EXTENSION ".choco"

  using namespace std;

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char *s) { printf("%s\n", s); return 0; }


  //liste de commandes
  enum class Command {
    _PRINT_,
		_JUMP_,
    _JUMP_IF_ZERO_,
    _PLUS_,
    _MOINS_,
    _FOIS_,
    _DIVISE_PAR_,
    _NUMBER_,
    _GET_IDENTIFIER_,
    _SET_IDENTIFIER_
  };


  //liste d'instructions (utilise liste de commandes)
  int currentInstruction = 0;   // compteur instruction 
  vector<pair<Command,double>> instructions;
  inline void addInstruct(Command command, double decimal = 0) { instructions.push_back(make_pair(command, decimal)); currentInstruction++; };

  // structure pour stocker les adresses pour les sauts condistionnels et autres...
  typedef struct adr {
    int currentInstruction_goto; 
    int currentInstruction_false;
  } t_adress;


  map<string,double> variables; // table de symboles


  //exécution liée aux commandes (utilise liste de commandes et currentInstruction)
  double depiler(vector<double> &pile) {
    double t = 0;
    if (pile.size() > 0) {
      t = pile[pile.size()-1];
      //cout << "Dépiler " << t << endl; 
      pile.pop_back();
    }
    return t;
  }

  typedef void (*functionPointer)(pair<Command, double> &instructionContent, vector<double> &pile);
  
  const map<Command, functionPointer> executeCommand = {
    {Command::_PRINT_,          
      [](pair<Command, double> &instructionContent, vector<double> &pile) { 
        cout << "Résultat : " << depiler(pile) << endl;
      }},

    {Command::_JUMP_,           
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        currentInstruction = (int)instructionContent.second;
      }},
    {Command::_JUMP_IF_ZERO_,   
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        currentInstruction = (depiler(pile) ? currentInstruction + 1 : (int)instructionContent.second);
      }},

    {Command::_PLUS_,           
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        pile.push_back(depiler(pile) + depiler(pile));
      }},
    {Command::_MOINS_,          
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        double tmp = depiler(pile);
        pile.push_back(depiler(pile) - tmp);
      }},
    {Command::_FOIS_,           
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        pile.push_back(depiler(pile) * depiler(pile));
      }},
    {Command::_DIVISE_PAR_,     
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        double tmp = depiler(pile);
        pile.push_back(depiler(pile) / tmp);
      }},

    {Command::_NUMBER_,         
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        pile.push_back(instructionContent.second);
      }},
    {Command::_GET_IDENTIFIER_, 
      [](pair<Command, double> &instructionContent, vector<double> &pile) { 
        pile.push_back(instructionContent.second);
      }},
    {Command::_SET_IDENTIFIER_, 
      [](pair<Command, double> &instructionContent, vector<double> &pile) {
        //variables[setVariables[currentInstruction]] = depiler(pile);
      }}
  };


%}

%union{
  double valeur;
  char nom[50];
  t_adress adresse;
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
                            $1.currentInstruction_goto = currentInstruction;  
                            addInstruct(Command::_JUMP_IF_ZERO_);
                          }
      THEN '\n' bloc     { 
                            $1.currentInstruction_false = currentInstruction;
                            addInstruct(Command::_JUMP_);
                            instructions[$1.currentInstruction_goto].second = currentInstruction;  
                          }
      ELSE '\n' bloc     { instructions[$1.currentInstruction_false].second = currentInstruction; }
      END_IF               {   }

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


bool folderExist();

bool programGeneration(int argc, char **argv);

void displayGeneratedProgram();

void executeGeneratedProgram();



int main(int argc, char **argv) {
  if (!folderExist()) exit(0);//ne peut pas fonctionner sans

  if (!programGeneration(argc, argv)) exit(0);//ne peut pas fonctionner sans

  displayGeneratedProgram();

  executeGeneratedProgram();
  
  return 0;
}



bool folderExist() {
  if (access(FOLDER, F_OK) == -1) {//le _DIVISE_PAR_ assure que c'est un dossier
    cout << "Dossier de programmes non trouvé : création en cours... ";

    if (mkdir(FOLDER, 0777)) {//échec de création
      cout << "Echec de création du dossier " << FOLDER << endl;
      return false;
    }
    else {
      cout << "fait" << endl << "Placez votre ou vos fichiers " << EXTENSION << " dans le répertoire créé" << endl;
    }
  }
  return true;
}


FILE* readOnlyFileFlow(string filename);
bool programGeneration(int argc, char **argv) {
  ++argv, --argc;
  if (argc) {
    yyin = readOnlyFileFlow(argv[0]);
  }
  else {
    int saisie;
    cout << "0 - Interprétation en fichier" << endl
        << "1 - Interprétation en console" << endl; 
    do cin >> saisie; while (saisie < 0 || saisie > 1);

    //initialisation de variables pour cas 0
    vector<string> fileList;
    DIR *fluxFolder;
    struct dirent *fileFolder;
    string filename;

    switch(saisie) {
      case 0 :
        //liste contenu du dossier
        cout << endl << "Contenu du dossier " << FOLDER << " :" << endl;
        fluxFolder = opendir(FOLDER);
        while (fileFolder = readdir(fluxFolder)) {
          filename = (string)fileFolder->d_name;
          if (filename.find(EXTENSION, filename.size() - ((string)EXTENSION).size()) !=  string::npos) {
            cout << fileList.size()+1 << " - \"" << filename << "\"" << endl;
            fileList.push_back(filename);
          }
        }
        closedir (fluxFolder);
        
        //choisit fichier à traiter
        cout << endl << "Votre sélection : ";
        do cin >> saisie; while (saisie <= 0 || saisie > fileList.size());

        yyin = readOnlyFileFlow(fileList[--saisie]);
        break;
      default ://case 1 :
        yyin = stdin;
        break;
    }
  }

  if (yyin == NULL) return false;
  yyparse();
  return true;
}
FILE* readOnlyFileFlow(string filename) {
  //ajoute extension si necessaire
  if (filename.find(EXTENSION, filename.size() - ((string)EXTENSION).size()) ==  string::npos) {
    filename += EXTENSION;
  }

  //exécute fichier s'il existe
  cout << filename << endl;
  if (access((FOLDER+filename).c_str(), F_OK) != -1) {
    return fopen((FOLDER+filename).c_str(),"r");
  }
  
  cout << "Echec d'accès au fichier" << endl;
  return NULL;
}


void displayGeneratedProgram() {
  cout << endl << "==== CODE GENERE ====" << endl;

  int i = 0;
  for (auto instructionContent : instructions) {
    //displayCommand - imprimer le code généré de manière plus lisible 
    cout << "INSTRUCTION " << setw(1+(int)(log10(instructions.size()))) << i++ << " - ";

    //"faux" switch
    if (instructionContent.first == Command::_NUMBER_) cout << "GET NUM " << instructionContent.second;
    else if (instructionContent.first == Command::_SET_IDENTIFIER_) cout << "SET NUM " << instructionContent.second << " IN MEMORY";//instructionContent.second ne comprend que la première valeur de l'expression donnée à la variable
    else if (instructionContent.first == Command::_GET_IDENTIFIER_) cout << "GET NUM " << instructionContent.second << " IN MEMORY";//instructionContent.second ne comprend que la première valeur de l'expression donnée à la variable
    
    else if (instructionContent.first == Command::_JUMP_IF_ZERO_)  cout << "IF ZERO, JUMP TO INSTRUCTION " << instructionContent.second;   // Jump if not zero
    else if (instructionContent.first == Command::_JUMP_)              cout << "JUMP TO INSTRUCTION " << instructionContent.second;   // Unconditional Jump

    //2 précédents
    else if (instructionContent.first == Command::_PLUS_) cout << "ADDITION OF TWO LAST NUM";
    else if (instructionContent.first == Command::_MOINS_) cout << "SUBSTRACTION OF TWO LAST NUM";
    else if (instructionContent.first == Command::_FOIS_) cout << "MULTIPLICATION OF TWO LAST NUM";
    else if (instructionContent.first == Command::_DIVISE_PAR_) cout << "DIVISION OF TWO LAST NUM";

    else if (instructionContent.first == Command::_PRINT_) cout << "PRINT RESULT";
    
    /*default*/ else cout << "UNKNOW COMMAND : " << (int)instructionContent.first;

    cout << endl;
  }

  cout << "=====================" << endl;
}

void executeGeneratedProgram() {//run program (similaire à de l'assembleur)
  vector<double> pile; 
  double x,y;

  cout << endl << "===== EXECUTION =====" << endl;
  currentInstruction = 0;
  while ( currentInstruction < instructions.size() ){
    auto instructionContent = instructions[currentInstruction];
    //cout << currentInstruction << '\t' << displayCommand(instructionContent.first) << "\t" << instructionContent.second << endl;
    
    currentInstruction++;
    if (executeCommand.find(instructionContent.first) != executeCommand.end()) {
      (*(executeCommand.at(instructionContent.first)))(instructionContent, pile);
    }
    else {
      cout << "unknow command : " << (int)instructionContent.first << endl;
    }
  }
  cout << "=====================" << endl;
}