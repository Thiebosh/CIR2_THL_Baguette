#include "fonctions.hpp"
using namespace std;

FILE* mainContent(int argc, char **argv) {
    //teste existence de dossier
    bool getFolder = folderExist();

    //exécute routine
    ++argv, --argc;

    if (!argc) {
        switch(menuPrincipal()) {
            case 0 :
                return stdin;
            case 1 :
                return workFile(getFolder);
        }
    }

    return fluxFichier(getFolder, argv[0]);
}

bool folderExist() {
    bool getFolder = true;

    if (access(FOLDER, F_OK) == -1) {//le "/" assure que c'est un dossier
        cout << "Dossier de programmes non trouvé : création en cours... ";
        getFolder = false;

        if (mkdir(FOLDER, 0777)) cout << "échec" << endl;
        else {
            cout << "fait" << endl << "Placez votre ou vos fichiers " << EXTENSION << " dans le répertoire créé" << endl;
            getFolder = true;
        }
    }

    return getFolder;
}

int menuPrincipal() {
    int saisie;
    
    cout << "0 - Interprétation en console" << endl 
         << "1 - Interprétation en fichier" << endl;
    do cin >> saisie; while (saisie < 0 || saisie > 1);

    return saisie;
}

FILE* workFile(bool getFolder) {
    if (!getFolder) exit(0);//return null;?

    vector<string> fileList = listeFichiers();
    
    int saisie;
    cout << endl << "Votre sélection : ";
    do cin >> saisie; while (saisie <= 0 || saisie > fileList.size());

    return fluxFichier(getFolder, fileList[--saisie]);
}

vector<string> listeFichiers() {
    //affiche fichiers du dossier 
    cout << endl << "Contenu du dossier " << FOLDER << " :" << endl;
    DIR *fluxFolder = opendir(FOLDER);
    struct dirent *fileFolder;
    vector<string> fileList;
    string filename;
    while (fileFolder = readdir(fluxFolder)) {
        filename = (string)fileFolder->d_name;
        if (filename.find(EXTENSION, filename.size() - ((string)EXTENSION).size()) !=  string::npos) {
        cout << fileList.size()+1 << " - \"" << filename << "\"" << endl;
        fileList.push_back(filename);
        }
    }
    closedir (fluxFolder);

    return fileList;
}

FILE* fluxFichier(bool getFolder, string filename) {
    if (!getFolder) exit(0);

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
    exit(0);//return null;?
}
