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
#define COMPILED_EXTENSION ".chocapic"

using namespace std;

bool folderExist() {
    if (access(DEFAULT_FOLDER, F_OK) == -1) {//le / assure que c'est un dossier
        cout << "Dossier de programmes non trouvé : création en cours... ";

        if (mkdir(DEFAULT_FOLDER, 0777)) {//échec de création
            cout << "Echec de création du dossier " << DEFAULT_FOLDER << endl;
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
    if (access((DEFAULT_FOLDER+filename).c_str(), F_OK) != -1) {
        return fopen((DEFAULT_FOLDER+filename).c_str(),"r");
    }
    
    cout << "Echec d'accès au fichier" << endl;
    return NULL;
}

FILE* programGeneration(int argc, char** argv) {
    ++argv, --argc;
    if (argc) {//remplacer par dossier de fichiers
        return readOnlyFileFlow(argv[0]);
    }
    
    //else implicite
    vector<string> fileList;
    DIR *fluxFolder;
    struct dirent *fileFolder;
    string filename;

    //liste contenu du dossier
    cout << endl << "Contenu du dossier " << DEFAULT_FOLDER << " :" << endl;
    fluxFolder = opendir(DEFAULT_FOLDER);
    while (fileFolder = readdir(fluxFolder)) {
        filename = (string)fileFolder->d_name;
        if (filename.find(PROGRAM_EXTENSION, filename.size() - ((string)PROGRAM_EXTENSION).size()) !=  string::npos) {
            cout << fileList.size()+1 << " - \"" << filename << "\"" << endl;
            fileList.push_back(filename);
        }
    }
    closedir (fluxFolder);

    //choisit fichier à traiter
    int saisie;
    cout << endl << "Votre sélection (0 pour quitter) : ";
    do cin >> saisie; while (saisie < -1 || saisie > fileList.size());

    if (!saisie) exit(0);

    return readOnlyFileFlow(fileList[--saisie]);
}


void saveGeneratedProgramFile() {
    //string const nomFichier("C:/Nanoc/scores.txt");
    //ofstream monFlux(nomFichier.c_str());
    ofstream file("programFiles/program.chocapic");

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

