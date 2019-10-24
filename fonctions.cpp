#include "fonctions.hpp"
using namespace std;


bool folderExist(string folder, string extension) {
    bool getFolder = true;

    if (access(folder.c_str(), F_OK) == -1) {//le "/" assure que c'est un dossier
        cout << "Dossier de programmes non trouvé : création en cours... ";
        getFolder = false;

        if (mkdir(folder.c_str(), 0777)) cout << "échec" << endl;
        else {
            cout << "fait" << endl << "Placez votre ou vos fichiers " << extension << " dans le répertoire créé" << endl;
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

vector<string> menuFichier(string folder, string filename, string extension) {
    //affiche fichiers du dossier 
    cout << endl << "Contenu du dossier " << folder << " :" << endl;
    DIR *fluxFolder = opendir(folder.c_str());
    struct dirent *fileFolder;
    vector<string> fileList;
    while (fileFolder = readdir(fluxFolder)) {
        filename = (string)fileFolder->d_name;
        if (filename.find(extension, filename.size() - extension.size()) !=  string::npos) {
        cout << fileList.size()+1 << " - \"" << filename << "\"" << endl;
        fileList.push_back(filename);
        }
    }
    closedir (fluxFolder);

    return fileList;
}

