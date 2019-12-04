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
    
    //else implicite
    vector<string> fileList;
    DIR *fluxFolder;
    struct dirent *fileFolder;
    string filename;

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
    int saisie;
    cout << endl << "Votre sélection (0 pour quitter) : ";
    do cin >> saisie; while (saisie < -1 || saisie > fileList.size());

    if (!saisie) exit(0);

    return readOnlyFileFlow(fileList[--saisie]);
}
