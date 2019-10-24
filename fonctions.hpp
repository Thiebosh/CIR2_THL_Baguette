//gestion fichiers et dossiers
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <iostream>
#include <string>
#include <vector>

bool folderExist(std::string folder, std::string extension);

int menuPrincipal();

std::vector<std::string> menuFichier(std::string folder, std::string filename, std::string extension);

