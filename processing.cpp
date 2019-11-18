//gestion affichage
#include <iostream>
#include <iomanip>
#include <cmath>

//gestion mémoire
#include "memory.cpp"


using namespace std;

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
	_ADD_IDENTIFIER_,
	_SET_IDENTIFIER_,
	_GET_IDENTIFIER_
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
	{command::_SET_IDENTIFIER_,
		[](instruction& instructContent) {//bison (name = expression) : instructContent = valAccess{ string,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.tabPos];
			//supprimer string du tableau

			if (variables.find(name) != variables.end()) variables[name] = depiler();
			//else?
		}},
	{command::_GET_IDENTIFIER_,
		[](instruction& instructContent) {//bison : instructContent = variables[$1];
			pile.push(instructContent.second);
			//sinon : pile.push(variables[stringList[instructContent.second.tabPos]]);
		}}
};


/********************************************************/
/*														*/
/*	PARTIE III : MAIN FONCTIONS							*/
/*														*/
/********************************************************/
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
