//gestion affichage
#include <iostream>
#include <iomanip>
#include <cmath>

//gestion mémoire
#include <string>
#include <deque>
#include <stack>
#include <map>

//gestion fichiers et dossiers
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#define FOLDER "programFiles/"
#define EXTENSION ".choco"

using namespace std;


//I. Memoire
//valeurs
enum class valType {//fixe types
	_int_,
	_double_,
	_string_
};

typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int index = -1;//valeur par defaut : flag d'invalidation
} valAccess;

deque<int> intList;
deque<double> doubleList;
deque<string> stringList;

void printVal(string beginMessage, valAccess val, string endMessage) {
    cout << beginMessage;
	switch (val.type) {
	case valType::_int_:
		cout << intList[val.index];
		break;
	case valType::_double_:
		cout << doubleList[val.index];
		break;
	case valType::_string_:
		cout << stringList[val.index];
		break;
	}
    cout << endMessage;
}

//valeurs en utilisation
stack<valAccess> pile;

valAccess depiler() {
	valAccess var;
	if (pile.size() > 0) {
		var = pile.top();
		pile.pop();
	}
	return var;//controler index != -1
}

enum class operation {//fixe operations
	_plus_,
	_moins_,
	_fois_,
	_divisePar_
};

void executeOperation(operation operation) {
	valAccess val2 = depiler();
	valAccess val1 = depiler();

	int val1Int(0), val2Int(0);
	double val1Double(0), val2Double(0);
	string val1String(""), val2String("");

	switch (val1.type) {
	case valType::_int_:
		val1Int = intList[val1.index];
		break;
	case valType::_double_:
		val1Double = doubleList[val1.index];
		break;
	case valType::_string_:
		val1String = stringList[val1.index];
		break;
	}
	switch (val2.type) {
	case valType::_int_:
		val2Int = intList[val2.index];
		break;
	case valType::_double_:
		val2Double = doubleList[val2.index];
		break;
	case valType::_string_:
		val2String = stringList[val2.index];
		break;
	}

	if (val1.type == valType::_int_ && val2.type == valType::_int_) {//même type
		int result(0);
		switch (operation) {
		case operation::_plus_:
			result = val1Int + val2Int;
			break;
		case operation::_moins_:
			result = val1Int - val2Int;
			break;
		case operation::_fois_:
			result = val1Int * val2Int;
			break;
		case operation::_divisePar_:
			result = val1Int / val2Int;
			break;
		}
		pile.push(valAccess({ valType::_int_,(int)intList.size() }));
		intList.push_back(result);
	}
	else if ((val1.type == valType::_int_ || val1.type == valType::_double_) &&
		(val2.type == valType::_int_ || val2.type == valType::_double_)) {//int et double ou deux doubles
		double result(0);
		switch (operation) {
		case operation::_plus_:
			result = (val1Int ? val1Int : val1Double) + (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case operation::_moins_:
			result = (val1Int ? val1Int : val1Double) - (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case operation::_fois_:
			result = (val1Int ? val1Int : val1Double) * (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case operation::_divisePar_:
			result = (val1Int ? val1Int : val1Double) / (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		}
		pile.push(valAccess({ valType::_double_,(int)doubleList.size() }));
		doubleList.push_back(result);
	}
	else if (val1.type == valType::_string_ && val2.type == valType::_string_) {
		//?
	}
	else {//string + int ou double
		//erreur?
	}
}


//variables
map<string, valAccess> variables;

void delVar(string name) {
	for (auto var : variables) {
		if (var.second.type == variables[name].type && var.second.index > variables[name].index) variables[var.first].index--;
	}
	switch (variables[name].type) {
	case valType::_int_:
		intList.erase(intList.begin() + variables[name].index);
		break;
	case valType::_double_:
		doubleList.erase(doubleList.begin() + variables[name].index);
		break;
	case valType::_string_:
		stringList.erase(stringList.begin() + variables[name].index);
		break;
	}
	variables.erase(name);
}
/*
void addVar(valType type, string name, int intVal, double doubleVal, string stringVal) {
	int index = 0;
	switch (type) {
	case valType::_int_:
		index = intList.size();
		intList.push_back(intVal);
		break;
	case valType::_double_:
		index = doubleList.size();
		doubleList.push_back(doubleVal);
		break;
	case valType::_string_:
		index = stringList.size();
		stringList.push_back(stringVal);
		break;
	}
	variables.insert({ name,valAccess{ type, index } });
}

void getVarContent(string name, int& intVal, double& doubleVal, string& stringVal) {//recupere valeur dans variable donnee en parametre
	switch (variables[name].type) {
	case valType::_int_:
		intVal = intList[variables[name].index];
		break;
	case valType::_double_:
		doubleVal = doubleList[variables[name].index];
		break;
	case valType::_string_:
		stringVal = stringList[variables[name].index];
		break;
	}
}

void setVarContent(string name, int intVal, double doubleVal, string stringVal) {
	switch (variables[name].type) {
	case valType::_int_:
		intList[variables[name].index] = intVal;
		break;
	case valType::_double_:
		doubleList[variables[name].index] = doubleVal;
		break;
	case valType::_string_:
		stringList[variables[name].index] = stringVal;
		break;
	}
}
*/

//sauts conditionnels
typedef struct {//renommer champs
	int indexInstruction_goto;
	int indexInstruction_false;
} t_adress;


//I. Mémoire
map<string,double> variables; // table de symboles


// structure pour stocker les adresses pour les sauts condistionnels et autres...
typedef struct {
    int currentInstruction_goto; 
    int currentInstruction_false;
} t_adress;


vector<double> pile; 


double depiler() {
    double t = 0;
    if (pile.size() > 0) {
        t = pile[pile.size()-1];
        pile.pop_back();
    }
    return t;
}



//II. Execution
//déclaration des commandes
enum class command {
	_PRINT_,
	_JUMP_,
	_JUMP_IF_ZERO_,
	_PLUS_,
	_MOINS_,
	_FOIS_,
	_DIVISE_PAR_,
	_NUMBER_,
	_GET_IDENTIFIER_,
	_ADD_IDENTIFIER_,
	_SET_IDENTIFIER_,
	_DEL_IDENTIFIER_
};


//utilisation des instructions
typedef pair<command, valAccess> instruction;
deque<instruction> instructionList;

int indexInstruction = 0;   // compteur instruction 
void addInstruct(command command, valAccess val = { valType::_int_,-1 }) {
	instructionList.push_back({ command, val }); 
	indexInstruction++; //necessaire pour enregistrer position
};

//execution des commandes selon les instructions
typedef void (*functionPointer)(instruction& instructContent);

const map<command, functionPointer> executeCommand = {
	{command::_PRINT_,
		[](instruction& instructContent) {
			valAccess val = depiler();
			//if (val.index != -1)
			printVal("Résultat : ",val,"\n");
		}},

	{command::_JUMP_,
		[](instruction& instructContent) {
			//tester instructContent? liste d'adresse?
			indexInstruction = intList[instructContent.second.index];
		}},
	{command::_JUMP_IF_ZERO_,
		[](instruction& instructContent) {
			//tester instructContent? liste d'adresse?
			valAccess testResult = depiler();
			if (testResult.index != -1 && testResult.type == valType::_int_ && intList[testResult.index] == 0) {
				indexInstruction = intList[instructContent.second.index];//cas if not 0 : incrementation prealable
			}
		}},

	{command::_PLUS_,
		[](instruction& instructContent) {
			executeOperation(operation::_plus_);
		}},
	{command::_MOINS_,
		[](instruction& instructContent) {
			executeOperation(operation::_moins_);
		}},
	{command::_FOIS_,
		[](instruction& instructContent) {
			executeOperation(operation::_fois_);
		}},
	{command::_DIVISE_PAR_,
		[](instruction& instructContent) {
			executeOperation(operation::_divisePar_);
		}},

	{command::_NUMBER_,
		[](instruction& instructContent) {//bison : (déterminer type); instructContent = valAccess{ type,listType.size() }; listType.push_back($1);
			pile.push(instructContent.second);
		}},
	{command::_ADD_IDENTIFIER_,
		[](instruction& instructContent) {//bison (type name = expression) : instructContent = valAccess{ $1,stringList.size() }; stringList.push_back($2);
			string name = stringList[instructContent.second.index];
			//supprimer string du tableau

			//verif types?
			if (variables.find(name) == variables.end()) variables.insert({name,depiler()});
			//else ?

			//bison incomplet (var = number) : addVar($1,$2,$3,$3,$3);
		}},
	{command::_DEL_IDENTIFIER_,
		[](instruction& instructContent) {//bison (delete name) : instructContent = valAccess{ string,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.index];
			//supprimer string du tableau

			if (variables.find(name) != variables.end()) delVar(name);
			//else?

			//a appeler aussi en fin de fonction... automatiser?
		}},
	{command::_GET_IDENTIFIER_,
		[](instruction& instructContent) {//bison : instructContent = variables[$1];
			pile.push(instructContent.second);
			//sinon : pile.push(variables[stringList[instructContent.second.index]]);
		}},
	{command::_SET_IDENTIFIER_,
		[](instruction& instructContent) {//bison (name = expression) : instructContent = valAccess{ string,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.index];
			//supprimer string du tableau

			if (variables.find(name) != variables.end()) variables[name] = depiler();
			//else?
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
	for (auto instructContent : instructionList) {
		cout << "INSTRUCTION " << setw(1 + log10(instructionList.size())) << i++ << " - ";

		switch (instructContent.first) {
		case command::_NUMBER_:
			printVal("GET NUM ",instructContent.second,"");
			break;
		case command::_SET_IDENTIFIER_:
			printVal("SET NUM ",instructContent.second," IN MEMORY"); //instructContent.second ne comprend que la première valeur de l'expression donnée à la variable
			break;
		case command::_GET_IDENTIFIER_:
			printVal("GET NUM ",instructContent.second," IN MEMORY"); //instructContent.second ne comprend que la première valeur de l'expression donnée à la variable
			break;

		case command::_JUMP_IF_ZERO_:
			printVal("IF ZERO, JUMP TO INSTRUCTION ",instructContent.second);
			break;
		case command::_JUMP_:
			printVal("JUMP TO INSTRUCTION ",instructContent.second);
			break;

		case command::_PLUS_:
			cout << "ADDITION OF TWO LAST NUM";
			break;
		case command::_MOINS_:
			cout << "SUBSTRACTION OF TWO LAST NUM";
			break;
		case command::_FOIS_:
			cout << "MULTIPLICATION OF TWO LAST NUM";
			break;
		case command::_DIVISE_PAR_:
			cout << "DIVISION OF TWO LAST NUM";
			break;

		case command::_PRINT_:
			cout << "PRINT RESULT";
			break;

		default:
			cout << "UNKNOW COMMAND : " << (int)instructContent.first;
		}
		cout << endl;
	}

	cout << "=====================" << endl;
}


//fonction 4
void executeGeneratedProgram() {//run program (similaire à de l'assembleur)
	cout << endl << "===== EXECUTION =====" << endl;
	indexInstruction = 0;
	while (indexInstruction < instructionList.size()) {
		instruction instructContent = instructionList[indexInstruction];

		indexInstruction++;
		if (executeCommand.find(instructContent.first) != executeCommand.end()) {
			(*(executeCommand.at(instructContent.first))) (instructContent);
		}
		else {
			cout << "unknow command : " << (int)instructContent.first << endl;
		}
	}
	cout << "=====================" << endl;
}
