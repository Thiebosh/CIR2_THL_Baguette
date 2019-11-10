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


  map<string,double> variables; // table de symboles

  vector<pair<int,double>> instructions;
  int currentInstruction = 0;   // compteur instruction 
  inline void addInstruct(int command, double decimal) { instructions.push_back(make_pair(command, decimal)); currentInstruction++; };

  // structure pour stocker les adresses pour les sauts condistionnels et autres...
  typedef struct adr {
    int currentInstruction_goto; 
    int currentInstruction_false;
  } t_adress;

  //association commande / code
  const map<string,int> commandList = {
		{"PRINT",0},
		{"JUMP",1},
    {"JUMP_IF_ZERO",2},
    {"+",3},
    {"-",4},
    {"*",5},
    {"/",6},
    {"NUMBER",7},
    {"GET_IDENTIFIER",8},
    {"SET_IDENTIFIER",9}
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
    expression { addInstruct(commandList.at("PRINT"),0);   /* imprimer le résultat de l'expression */  }

    | IF expression '\n'  { 
                            $1.currentInstruction_goto = currentInstruction;  
                            addInstruct(commandList.at("JUMP_IF_ZERO"),0);
                          }
      THEN '\n' bloc     { 
                            $1.currentInstruction_false = currentInstruction;
                            addInstruct(commandList.at("JUMP"),0);
                            instructions[$1.currentInstruction_goto].second = currentInstruction;  
                          }
      ELSE '\n' bloc     { instructions[$1.currentInstruction_false].second = currentInstruction; }
      END_IF               {   }

    | REPEAT '(' expression ')' expression { /* TO DO */ }

    | IDENTIFIER '=' expression { 
                                  variables[$1] = $3; 
                                  addInstruct(commandList.at("SET_IDENTIFIER"), $3); 
                                }
    |   /* Ligne vide*/
    ;

expression: 
    expression '+' expression     { addInstruct(commandList.at("+"), 0);}
    | expression '-' expression     { addInstruct(commandList.at("-"), 0);}
    | expression '*' expression     { addInstruct(commandList.at("*"), 0);}
    | expression '/' expression     { addInstruct(commandList.at("/"), 0);}
    | '(' expression ')'            { }
    | NUMBER                        { addInstruct(commandList.at("NUMBER"), $1);}
    | IDENTIFIER                    { addInstruct(commandList.at("GET_IDENTIFIER"), variables[$1]);}
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
  if (access(FOLDER, F_OK) == -1) {//le "/" assure que c'est un dossier
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
  for (auto instruct : instructions) {
    //displayCommand - imprimer le code généré de manière plus lisible 
    cout << "INSTRUCTION " << setw(1+(int)(log10(instructions.size()))) << i++ << " - ";

    //"faux" switch
    if (instruct.first == commandList.at("NUMBER")) cout << "GET NUM " << instruct.second;
    else if (instruct.first == commandList.at("SET_IDENTIFIER")) cout << "SET NUM " << instruct.second << " IN MEMORY";//instruct.second ne comprend que la première valeur de l'expression donnée à la variable
    else if (instruct.first == commandList.at("GET_IDENTIFIER")) cout << "GET NUM " << instruct.second << " IN MEMORY";//instruct.second ne comprend que la première valeur de l'expression donnée à la variable
    
    else if (instruct.first == commandList.at("JUMP_IF_ZERO"))  cout << "IF ZERO, JUMP TO INSTRUCTION " << instruct.second;   // Jump if not zero
    else if (instruct.first == commandList.at("JUMP"))              cout << "JUMP TO INSTRUCTION " << instruct.second;   // Unconditional Jump

    //2 précédents
    else if (instruct.first == commandList.at("+")) cout << "ADDITION OF TWO LAST NUM";
    else if (instruct.first == commandList.at("-")) cout << "SUBSTRACTION OF TWO LAST NUM";
    else if (instruct.first == commandList.at("*")) cout << "MULTIPLICATION OF TWO LAST NUM";
    else if (instruct.first == commandList.at("/")) cout << "DIVISION OF TWO LAST NUM";

    else if (instruct.first == commandList.at("PRINT")) cout << "PRINT RESULT";
    
    /*default*/ else cout << "UNKNOW COMMAND : " << instruct.first;

    cout << endl;
  }

  cout << "=====================" << endl;
}


double depiler(vector<double> &pile);
void executeGeneratedProgram() {//run program (similaire à de l'assembleur)
  vector<double> pile; 
  double x,y;

  cout << endl << "===== EXECUTION =====" << endl;
  currentInstruction = 0;
  while ( currentInstruction < instructions.size() ){
    auto instruct = instructions[currentInstruction];
    //cout << currentInstruction << '\t' << displayCommand(instruct.first) << "\t" << instruct.second << endl;
    
    {//"faux" switch
      if (instruct.first == commandList.at("NUMBER")) {
        pile.push_back(instruct.second);
        currentInstruction++;
      }
      else if (instruct.first == commandList.at("+")) {
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y+x);
        currentInstruction++;
      }
      else if (instruct.first == commandList.at("-")) {
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y-x);
        currentInstruction++;
      }
      else if (instruct.first == commandList.at("*")) {
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y*x);
        currentInstruction++;
      }
      else if (instruct.first == commandList.at("/")) {
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y/x);
        currentInstruction++;
      }

      else if (instruct.first == commandList.at("GET_IDENTIFIER")) {
        pile.push_back(instruct.second);
        currentInstruction++;
      }
      else if (instruct.first == commandList.at("SET_IDENTIFIER")) {
        //variables[setVariables[currentInstruction]] = depiler(pile);//recupère résultat
        currentInstruction++;
      }

      else if (instruct.first == commandList.at("JUMP_IF_ZERO")) {
        currentInstruction = (depiler(pile) ? currentInstruction + 1 : instruct.second);
      }
      else if (instruct.first == commandList.at("JUMP")) {
        currentInstruction = instruct.second;
      }

      else if (instruct.first == commandList.at("PRINT")) {
        cout << "Résultat : " << depiler(pile) << endl;
        currentInstruction++;
      }
      
      else {//default
        cout << "unknow command : " << instruct.first << endl;
        currentInstruction++;
      }
    }
  }
  cout << "=====================" << endl;
}
double depiler(vector<double> &pile) {
  double t = 0;
  if (pile.size() > 1) {
    t = pile[pile.size()-1];
    //cout << "Dépiler " << t << endl; 
    pile.pop_back();
  }
  return t;
}