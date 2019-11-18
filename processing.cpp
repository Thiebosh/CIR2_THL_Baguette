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

/********************************************************/
/*														*/
/*	PARTIE I : GESTION DE LA MEMOIRE					*/
/*		SOUS PARTIE 1 : STOCKAGE DE VALEURS TYPEES		*/
/*		SOUS PARTIE 2 : PILE DE VALEURS	EN UTILISATION	*/
/*		SOUS PARTIE 3 : OPERATION SUR LES VALEURS		*/
/*		SOUS PARTIE 4 : UTILISATION DES VARIABLES		*/
/*		SOUS PARTIE 5 : PILE DE BLOCS MEMOIRE			*/
/*														*/
/********************************************************/

/********************************************************/
/*		SOUS PARTIE 1 : STOCKAGE DE VALEURS TYPEES		*/
/********************************************************/
enum class valType {//fixe types
	_int_,
	_double_,
	_string_
};

typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int tabPos = -1;//valeur par defaut : flag d'invalidation
} valAccess;

deque<int>		intList;
deque<double>	doubleList;
deque<string>	stringList;

void printVal(string beginMessage, valAccess val, string endMessage = "") {
	cout << beginMessage;
	switch (val.type) {
	case valType::_int_:
		cout << intList[val.tabPos];
		break;
	case valType::_double_:
		cout << doubleList[val.tabPos];
		break;
	case valType::_string_:
		cout << stringList[val.tabPos];
		break;
	}
	cout << endMessage;
}

/********************************************************/
/*		SOUS PARTIE 2 : PILE DE VALEURS	EN UTILISATION	*/
/********************************************************/
stack<valAccess> pile;

valAccess depiler() {
	valAccess var;
	if (pile.size() > 0) {
		var = pile.top();
		pile.pop();
	}
	return var;//controler tabPos != -1
}

/********************************************************/
/*		SOUS PARTIE 3 : OPERATION SUR LES VALEURS		*/
/********************************************************/
enum class operation {//fixe operations
	_plus_,
	_moins_,
	_fois_,
	_divisePar_
};

void executeOperation(operation operation) {
	//recupere valeurs
	valAccess val2 = depiler();
	valAccess val1 = depiler();

	int val1Int(0), val2Int(0);
	double val1Double(0), val2Double(0);
	string val1String(""), val2String("");

	switch (val1.type) {
	case valType::_int_:
		val1Int = intList[val1.tabPos];
		break;
	case valType::_double_:
		val1Double = doubleList[val1.tabPos];
		break;
	case valType::_string_:
		val1String = stringList[val1.tabPos];
		break;
	}
	switch (val2.type) {
	case valType::_int_:
		val2Int = intList[val2.tabPos];
		break;
	case valType::_double_:
		val2Double = doubleList[val2.tabPos];
		break;
	case valType::_string_:
		val2String = stringList[val2.tabPos];
		break;
	}

	//execute operation et enregistre nouvelle valeur
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
		string result("");
		switch (operation) {
		case operation::_plus_://concatenation
			result = val1String + val2String;
			break;
			/* reste?
		case operation::_moins_:
			result = (val1Int ? val1Int : val1Double) - (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case operation::_fois_:
			result = (val1Int ? val1Int : val1Double) * (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
		case operation::_divisePar_:
			result = (val1Int ? val1Int : val1Double) / (val2Int ? val2Int : val2Double);//variables initialisees a 0
			break;
			*/
		}
		pile.push(valAccess({ valType::_double_,(int)doubleList.size() }));
		stringList.push_back(result);
	}
	else {//string + int ou double
		//erreur? "tostring"? repetition?
	}
}

/********************************************************/
/*		SOUS PARTIE 4 : UTILISATION DES VARIABLES		*/
/********************************************************/
map<string, valAccess> variables;

void delVal(valAccess val) {
	if (val.type == pile.top().type && val.tabPos == pile.top().tabPos) pile.pop();//dernier element? le retire
	else {
		//PILE : decremente references tableau des valeurs suivantes
		stack<valAccess> reversePile;
		while (!pile.empty()) {
			valAccess pileVal = depiler();
			if (pileVal.type == val.type) {//tableau subissant suppression
				if (pileVal.tabPos < val.tabPos) reversePile.push(pileVal); //index inferieur : modifie pas
				else if (pileVal.tabPos > val.tabPos) {//index > index_supprime, decremente
					pileVal.tabPos--;
					reversePile.push(pileVal);//stocke result
				}
				//else, valeur a supprimer : la "retire" de la pile (ne l'ajoute pas)
			}
			else reversePile.push(pileVal);//autre tableau : touche pas
		}
		while (!reversePile.empty()) {//retablit ordre de la pile
			pile.push(reversePile.top());
			reversePile.pop();
		}
	}

	//VARIABLES : decremente references tableau des valeurs suivantes
	for (auto var : variables) {
		if (var.second.type == val.type && var.second.tabPos > val.tabPos) {
			variables[var.first].tabPos--;
		}
	}

	//supprimer la valeur du tableau
	switch (val.type) {
	case valType::_int_:
		if (val.tabPos == intList.size() - 1) intList.pop_back();
		else intList.erase(intList.begin() + val.tabPos);
		break;
	case valType::_double_:
		if (val.tabPos == doubleList.size() - 1) doubleList.pop_back();
		else doubleList.erase(doubleList.begin() + val.tabPos);
		break;
	case valType::_string_:
		if (val.tabPos == stringList.size() - 1) stringList.pop_back();
		else stringList.erase(stringList.begin() + val.tabPos);
		break;
	}
}

void delVar(string name) {
	//decremente references tableau des valeurs suivantes dans la pile et la liste de variables + supprime valeur du tableau
	delVal(variables[name]);

	//supprime variable
	variables.erase(name);
}
/*
void addVar(valType type, string name, int intVal, double doubleVal = 0, string stringVal = "") {
	int tabPos = 0;
	switch (type) {
	case valType::_int_:
		tabPos = intList.size();
		intList.push_back(intVal);
		break;
	case valType::_double_:
		tabPos = doubleList.size();
		doubleList.push_back(doubleVal);
		break;
	case valType::_string_:
		tabPos = stringList.size();
		stringList.push_back(stringVal);
		break;
	}
	variables.insert({ name,valAccess{ type, tabPos } });
}

void getVarContent(string name, int& intVal, double& doubleVal, string& stringVal) {//recupere valeur dans variable donnee en parametre
	switch (variables[name].type) {
	case valType::_int_:
		intVal = intList[variables[name].tabPos];
		break;
	case valType::_double_:
		doubleVal = doubleList[variables[name].tabPos];
		break;
	case valType::_string_:
		stringVal = stringList[variables[name].tabPos];
		break;
	}
}

void setVarContent(string name, int intVal, double doubleVal = 0, string stringVal = "") {
	switch (variables[name].type) {
	case valType::_int_:
		intList[variables[name].tabPos] = intVal;
		break;
	case valType::_double_:
		doubleList[variables[name].tabPos] = doubleVal;
		break;
	case valType::_string_:
		stringList[variables[name].tabPos] = stringVal;
		break;
	}
}
*/

/********************************************************/
/*		SOUS PARTIE 5 : PILE DE BLOCS MEMOIRE			*/
/********************************************************/
typedef struct {
	unsigned int intListSize = 0;
	unsigned int doubleListSize = 0;
	unsigned int stringListSize = 0;
} memoryState;

stack<memoryState> memoryLayer;

void enterMemoryLayer() {
	memoryLayer.push(memoryState{ intList.size(),doubleList.size(),stringList.size() });
}

void exitMemoryLayer() {
	memoryState initial;
	if (!memoryLayer.empty()) {//pas besoin de declarer nouvel espace memoire au demarrage
		initial = memoryLayer.top();
		memoryLayer.pop();
	}

	map<string, valAccess> oldVariables = variables;
	for (auto var : oldVariables) {//supprime variables declarees dans le bloc
		if ((var.second.type == valType::_int_		&& (unsigned)var.second.tabPos >= initial.intListSize) ||
			(var.second.type == valType::_double_	&& (unsigned)var.second.tabPos >= initial.doubleListSize) ||
			(var.second.type == valType::_string_	&& (unsigned)var.second.tabPos >= initial.stringListSize)) {
			delVar(var.first);
		}
	}

	//supprime valeurs (non affectees a des variables) declarees dans le bloc (supprime par la fin pour + d'efficacite (deque) et de surete)
	while (intList.size() > initial.intListSize)		delVal(valAccess{ valType::_int_,	(int)intList.size() - 1 });
	while (doubleList.size() > initial.doubleListSize)	delVal(valAccess{ valType::_double_,(int)doubleList.size() - 1 });
	while (stringList.size() > initial.stringListSize)	delVal(valAccess{ valType::_string_,(int)stringList.size() - 1 });
}


/********************************************************/
/*														*/
/*	PARTIE II : GESTION DES COMMANDES					*/
/*		SOUS PARTIE 1 : DECLARATION DES COMMANDES		*/
/*		SOUS PARTIE 2 : GESTION DES INSTRUCTIONS		*/
/*		SOUS PARTIE 3 : EXECUTION SELON LES COMMANDES	*/
/*														*/
/********************************************************/

/********************************************************/
/*		SOUS PARTIE 1 : DECLARATION DES COMMANDES		*/
/********************************************************/
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
	_SET_IDENTIFIER_
};

/********************************************************/
/*		SOUS PARTIE 2 : GESTION DES INSTRUCTIONS		*/
/********************************************************/
typedef pair<command, valAccess> instruction;
deque<instruction> instructionList;

unsigned int indexInstruction = 0;   // compteur instruction 
void addInstruct(command command, valAccess val = { valType::_int_,-1 }) {
	instructionList.push_back({ command, val });
	indexInstruction++; //necessaire pour enregistrer position
};
//sauts conditionnels
typedef struct {
	int jumpToInstruct;
	int jumpToInstructIfFalse;
} idInstruct;



/********************************************************/
/*		SOUS PARTIE 3 : EXECUTION SELON LES COMMANDES	*/
/********************************************************/
typedef void (*functionPointer)(instruction& instructContent);

const map<command, functionPointer> executeCommand = {
	{command::_PRINT_,
		[](instruction& instructContent) {
			valAccess val = depiler();
			//if (val.tabPos != -1)
			printVal("Résultat : ",val,"\n");
		}},

	{command::_JUMP_,
		[](instruction& instructContent) {
			//tester instructContent? liste d'adresse?
			indexInstruction = intList[instructContent.second.tabPos];
		}},
	{command::_JUMP_IF_ZERO_,
		[](instruction& instructContent) {
			//tester instructContent? liste d'adresse?
			valAccess testResult = depiler();
			if (testResult.tabPos != -1 && testResult.type == valType::_int_ && intList[testResult.tabPos] == 0) {
				indexInstruction = intList[instructContent.second.tabPos];//cas if not 0 : incrementation prealable
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
			string name = stringList[instructContent.second.tabPos];
			//supprimer string du tableau

			//verif types?
			if (variables.find(name) == variables.end()) variables.insert({name,depiler()});
			//else ?
		}},
	{command::_GET_IDENTIFIER_,
		[](instruction& instructContent) {//bison : instructContent = variables[$1];
			pile.push(instructContent.second);
			//sinon : pile.push(variables[stringList[instructContent.second.tabPos]]);
		}},
	{command::_SET_IDENTIFIER_,
		[](instruction& instructContent) {//bison (name = expression) : instructContent = valAccess{ string,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.tabPos];
			//supprimer string du tableau

			if (variables.find(name) != variables.end()) variables[name] = depiler();
			//else?
		}}
};



/********************************************************/
/*														*/
/*	PARTIE III : MAIN FONCTIONS							*/
/*														*/
/********************************************************/

//fonction 1
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
		cout << "INSTRUCTION " << setw((streamsize)(1 + log10(instructionList.size()))) << i++ << " - ";

		switch (instructContent.first) {
		case command::_NUMBER_:
			printVal("GET NUM ", instructContent.second);
			break;
		case command::_SET_IDENTIFIER_:
			printVal("SET NUM ", instructContent.second, " IN MEMORY"); //instructContent.second ne comprend que la première valeur de l'expression donnée à la variable
			break;
		case command::_GET_IDENTIFIER_:
			printVal("GET NUM ", instructContent.second, " IN MEMORY"); //instructContent.second ne comprend que la première valeur de l'expression donnée à la variable
			break;

		case command::_JUMP_IF_ZERO_:
			printVal("IF ZERO, JUMP TO INSTRUCTION ", instructContent.second);
			break;
		case command::_JUMP_:
			printVal("JUMP TO INSTRUCTION ", instructContent.second);
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
