#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <cmath>

//gestion fichiers et dossiers
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#define FOLDER "programFiles/"
#define EXTENSION ".choco"

using namespace std;


//I. Instructions et mémoire

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
void addInstruct(Command command, double decimal = 0) { 
    instructions.push_back(make_pair(command, decimal)); currentInstruction++;
};

// structure pour stocker les adresses pour les sauts condistionnels et autres...
typedef struct adr {
    int currentInstruction_goto; 
    int currentInstruction_false;
} t_adress;


map<string,double> variables; // table de symboles



//II. exécution des commandes

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



//III. fonctions du main

//fonction 1
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


//fonction 2
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

FILE* programGeneration(int argc, char **argv) {
    ++argv, --argc;
    if (argc) {
        return readOnlyFileFlow(argv[0]);
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

                return readOnlyFileFlow(fileList[--saisie]);
                break;
            default ://case 1 :
                return stdin;
                break;
        }
    }

    return NULL;
}


//fonction 3
void displayGeneratedProgram() {
    cout << endl << "==== CODE GENERE ====" << endl;

    int i = 0;
    for (auto instructionContent : instructions) {
        cout << "INSTRUCTION " << setw(1+(int)(log10(instructions.size()))) << i++ << " - ";

        switch(instructionContent.first) {
            case Command::_NUMBER_ :
                cout << "GET NUM " << instructionContent.second;
                break;
            case  Command::_SET_IDENTIFIER_ :
                cout << "SET NUM " << instructionContent.second << " IN MEMORY";//instructionContent.second ne comprend que la première valeur de l'expression donnée à la variable
                break;
            case  Command::_GET_IDENTIFIER_ :
                cout << "GET NUM " << instructionContent.second << " IN MEMORY";//instructionContent.second ne comprend que la première valeur de l'expression donnée à la variable
                break;

            case  Command::_JUMP_IF_ZERO_ :
                cout << "IF ZERO, JUMP TO INSTRUCTION " << instructionContent.second;   // Jump if not zero
                break;
            case  Command::_JUMP_ :
                cout << "JUMP TO INSTRUCTION " << instructionContent.second;   // Unconditional Jump
                break;

            case  Command::_PLUS_ :
                cout << "ADDITION OF TWO LAST NUM";
                break;
            case  Command::_MOINS_ :
                cout << "SUBSTRACTION OF TWO LAST NUM";
                break;
            case  Command::_FOIS_ :
                cout << "MULTIPLICATION OF TWO LAST NUM";
                break;
            case  Command::_DIVISE_PAR_ :
                cout << "DIVISION OF TWO LAST NUM";
                break;

            case  Command::_PRINT_ :
                cout << "PRINT RESULT";
                break;
        
            default : 
                cout << "UNKNOW COMMAND : " << (int)instructionContent.first;
        }
        cout << endl;
    }

    cout << "=====================" << endl;
}


//fonction 4
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
