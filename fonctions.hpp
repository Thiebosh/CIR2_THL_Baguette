//gestion fichiers et dossiers
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
//#include <math.h>

#define FOLDER "programFiles/"
#define EXTENSION ".choco"


FILE* mainContent(int argc, char **argv);

bool folderExist();

int menuPrincipal();

FILE* workFile(bool getFolder);

std::vector<std::string> listeFichiers();

FILE* fluxFichier(bool getFolder, std::string filename);

