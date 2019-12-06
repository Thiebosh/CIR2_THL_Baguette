#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <fstream>

#include "processing.cpp"

#define DEFAULT_FOLDER "programFiles/"
#define PROGRAM_EXTENSION ".choco"
#define COMMAND_EXTENSION ".command"
#define COMPILED_EXTENSION ".chocapic"

string folderName = DEFAULT_FOLDER;
string programName = "";

bool folderExist() {
    if (access(folderName.c_str(), F_OK) == -1) {//le / assure que c'est un dossier
        cout << "Dossier de programmes non trouvé : création en cours... ";

        if (mkdir(folderName.c_str(), 0777)) {//échec de création
            cout << "Echec de création du dossier " << folderName << endl;
            return false;
        }
        else {
            cout << "fait" << endl << "Placez votre ou vos fichiers " << PROGRAM_EXTENSION << " dans le répertoire créé" << endl;
        }
    }
    return true;
}


FILE* readOnlyFileFlow(string filename) {
    //ajoute extension si necessaire
    if (filename.find(PROGRAM_EXTENSION, filename.size() - ((string)PROGRAM_EXTENSION).size()) ==  string::npos) {
        filename += PROGRAM_EXTENSION;
    }

    //exécute fichier s'il existe
    cout << filename << endl;
    if (access((folderName + filename).c_str(), F_OK) != -1) {
        return fopen((folderName + filename).c_str(),"r");
    }
    
    cout << "Echec d'accès au fichier" << endl;
    return NULL;
}

FILE* programGeneration(int argc, char** argv) {
    ++argv, --argc;
    if (argc) {//remplacer par dossier de fichiers
        programName = argv[0];
        return readOnlyFileFlow(argv[0]);
    }
    
    vector<string> programList;
    vector<string> compiledList;

    DIR *fluxFolder = opendir(folderName.c_str());
    while (struct dirent *fileFolder = readdir(fluxFolder)) {
        string filename = (string)fileFolder->d_name;
        if (filename.find(PROGRAM_EXTENSION, filename.size() - ((string)PROGRAM_EXTENSION).size()) !=  string::npos) {
            programList.push_back(filename);
        }
        else if (filename.find(COMPILED_EXTENSION, filename.size() - ((string)COMPILED_EXTENSION).size()) !=  string::npos) {
            compiledList.push_back(filename);
        }
    }
    closedir (fluxFolder);
    
    int i = 0;
    cout << endl << "Dossier " << folderName << " : Fichiers programmes" << endl;//a compiler
    for (auto file : programList) cout << ++i << " - \"" << file << "\"" << endl;

    cout << endl << "Dossier " << folderName << " : Programmes compilés (inaccessible : wip)" << endl;//a executer
    for (auto file : compiledList) cout << ++i << " - \"" << file << "\"" << endl;

    //choisit fichier à traiter
    int saisie;
    cout << endl << "Votre sélection (programme à compiler, compilé à exécuter ou 0 pour quitter) : ";
    do cin >> saisie; while (saisie < -1 || saisie > programList.size() /*+ compiledList.size()*/);

    if (--saisie < 0) exit(0);
    else if (saisie < programList.size()) {
        programName = programList[saisie].substr(0, programList[saisie].size() - ((string)PROGRAM_EXTENSION).size());
        return readOnlyFileFlow(programList[saisie]);
    }
    else {
        saisie -= programList.size();
        programName = compiledList[saisie].substr(0, compiledList[saisie].size() - ((string)COMPILED_EXTENSION).size());
        return readOnlyFileFlow(compiledList[saisie]);
    }
}


void saveCommandProgramFile() {
    ofstream file((folderName + programName + COMMAND_EXTENSION).c_str());

    if (file) {
        file << endl << "===== EXECUTION =====" << endl;
        for (instruction instructContent : instructionList) {
            string arguments = "command::";
            switch (instructContent.first) {
                //MEMOIRE - ok
                case command::_ENTER_BLOCK_: 
                    arguments += "_ENTER_BLOCK_"; 
                    break;
                case command::_EXIT_BLOCK_: 
                    arguments += "_EXIT_BLOCK_"; 
                    break;

                //EMPILEMENT - ok
                case command::_EMPILE_VALUE_:
                    arguments += "_EMPILE_VALUE_,"; 
                    switch (instructContent.second.type) {
                        case valType::_int_:
                            arguments += to_string(instructContent.second.intVal);
                            break;
                        case valType::_double_:
                            arguments += to_string(instructContent.second.doubleVal);
                            break;
                        case valType::_string_:
                            arguments += "\"" + instructContent.second.stringVal + "\"";
                            break;
                    }
                    break;
                case command::_EMPILE_VARIABLE_:
                    arguments += "_EMPILE_VARIABLE_,\"" + instructContent.second.stringVal + "\""; 
                    break;

                //OPERATIONS (var to var) - ok
                case command::_PLUS_CREMENT_:
                    arguments += "_PLUS_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_MOINS_CREMENT_:
                    arguments += "_MOINS_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_FOIS_CREMENT_:
                    arguments += "_FOIS_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_DIVISE_CREMENT_:
                    arguments += "_DIVISE_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_PLUS_:
                    arguments += "_PLUS_"; 
                    break;
                case command::_MOINS_:
                    arguments += "_MOINS_"; 
                    break;
                case command::_FOIS_:
                    arguments += "_FOIS_"; 
                    break;
                case command::_DIVISE_PAR_:
                    arguments += "_DIVISE_PAR_"; 
                    break;

                //COMPARAISON - ok
                case command::_AND_:
                    arguments += "_AND_"; 
                    break;
                case command::_OR_:
                    arguments += "_OR_"; 
                    break;
                case command::_EQUIV_:
                    arguments += "_EQUIV_"; 
                    break;
                case command::_DIFF_:
                    arguments += "_DIFF_"; 
                    break;
                case command::_SUPERIEUR_:
                    arguments += "_SUPERIEUR_"; 
                    break;
                case command::_INFERIEUR_:
                    arguments += "_INFERIEUR_"; 
                    break;
                case command::_SUP_EGAL_:
                    arguments += "_SUP_EGAL_"; 
                    break;
                case command::_INF_EGAL_:
                    arguments += "_INF_EGAL_"; 
                    break;

                //SAUTS (conditions, boucles, fonctions) - ok
                case command::_GOTO_:
                    arguments += "_GOTO_," + instructContent.second.intVal;
                    break;
                case command::_GOTO_TEST_:
                    arguments += "_GOTO_TEST_," + instructContent.second.intVal;
                    break;

                //VARIABLES - ok
                case command::_CREATE_VARIABLE_:
                    arguments += "_CREATE_VARIABLE_,\"" + instructContent.second.stringVal + "\"";
                    break;
                case command::_UPDATE_VARIABLE_:
                    arguments += "_UPDATE_VARIABLE_,\"" + instructContent.second.stringVal + "\"";
                    break;

                //FONCTIONS - ok
                case command::_CREATE_FUNCTION_:
                    arguments += "_CREATE_FUNCTION_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_CALL_FUNCTION_:
                    arguments += "_CALL_FUNCTION_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_ENTER_FUNCTION_:
                    arguments += "_ENTER_FUNCTION_";
                    break;
                case command::_EXIT_FUNCTION_:
                    arguments += "_EXIT_FUNCTION_";
                    break;

                //ENTREE SORTIE - ok
                case command::_WRITE_:
                    arguments += "_WRITE_"; 
                    break;
                case command::_STOP_:
                    arguments += "_STOP_"; 
                    break;
                case command::_READ_:
                    arguments += "_READ_,\"" + instructContent.second.stringVal + "\""; 
                    break;
            }

            file << "addInstruct(" << arguments << ");" << endl;
        }
	    file << endl << "=====================" << endl;
        file.close();
    }
}

void saveCompiledProgramFile() {
    ofstream file((folderName + programName + COMPILED_EXTENSION).c_str());

    if (file) {
        //todo
        file.close();
    }
}

void loadCompiledProgramFile() {
    //todo
}

