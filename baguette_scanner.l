%{
  #include <unistd.h>
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <dirent.h>

  #include <iostream>
  #include <string>
  #include <vector>

  #include "parser.bison.hpp"

  using namespace std;
%}

%%

[0-9]+(\.[0-9]*)?([Ee][0-9]+)? {  yylval.val = atof(yytext); return NUM; }

\n     {  return '\n'; }
plus   { return '+'; }
fois   { return '*'; }
moins  { return '-';}
divise { return '/' ;}
puissance { return '^' ;}
racine { return SQRT ;}
et     { return '&&' ;}
ou     { return '||' ;}
non    { return '!' ;}
" "    {}
un     { yylval.val = 1 ; return NUM;}
deux   { yylval.val = 2 ; return NUM;}
sin    {return SIN;}
tan    {return TAN;}
Si|if {return IF ;}
Alors|then {return THEN ;}
Sinon|else {return ELSE ;}

[a-zA-Z_][0-9a-zA-Z_]* { strcpy(yylval.name,yytext); return ID; }

.      { return yytext[0]; }

%%

int yywrap(){}

int main(int argc, char **argv) {
  string folder = "programFiles/", filename, extension = ".choco";

  //teste existence de dossier
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

  //exécute routine
  ++argv, --argc;
  if (!argc) {
    cout << "0 - Interprétation en console" << endl 
         << "1 - Interprétation en fichier" << endl;
    int saisie;
    do cin >> saisie; while (saisie < 0 || saisie > 1);

    switch(saisie) {
      case 0 :
        yyin = stdin;
        break;
      case 1 :
        if (!getFolder) exit(0);

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

        //traite entrée utilisateur
        cout << endl << "Votre sélection : ";
        do cin >> saisie; while (saisie <= 0 || saisie > fileList.size());

        //exécute fichier s'il existe
        filename = fileList[--saisie];
        cout << filename << endl;
        if (access((folder+filename).c_str(), F_OK) != -1) {
          yyin = fopen((folder+filename).c_str(),"r");
        }
        else {
          cout << "Echec d'accès au fichier" << endl;
          exit(0);
        }
        break;
    }
    cout << endl;
  }
  else {
    if (!getFolder) exit(0);

    filename = argv[0];
    if (filename.find(extension, filename.size() - extension.size()) ==  string::npos) {
      filename += extension;
    }
    if (access((folder+filename).c_str(), F_OK) != -1) {
      yyin = fopen((folder+filename).c_str(),"r");
    }
    else {
      cout << "Ce fichier n'existe pas" << endl;
      exit(0);
    }
  }
  
  yyparse();
}