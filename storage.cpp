#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#define FOLDER "programFiles/"
#define EXTENSION ".choco"

using namespace std;

bool folderExist() {
  if (access(FOLDER, F_OK) == -1) {//le / assure que c'est un dossier
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
