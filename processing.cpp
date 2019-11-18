//gestion affichage
#include <iostream>
#include <iomanip>
#include <cmath>

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
	//SAUTS (conditions, boucles, fonctions)
	_JUMP_,
	_JUMP_IF_ZERO_,

	//DECLARATION (valeur, variable, tableau)
	_EMPILE_VALUE_,
	_CREATE_VARIABLE_,
	_EMPILE_VARIABLE_,
	_UPDATE_VARIABLE_,
	_CREATE_VAR_TABLE_,
	_EMPILE_VAR_TABLE_,
	_EMPILE_VAR_TABLE_ELEMENT_,
	_UPDATE_VAR_TABLE_ELEMENT_,
	_REMOVE_VAR_TABLE_ELEMENT_,

	//OPERATIONS (var to var)
	_PLUS_,
	_MOINS_,
	_FOIS_,
	_DIVISE_PAR_,

	//ENTREE SORTIE
	_PRINT_
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


	{command::_EMPILE_VALUE_,
		[](instruction& instructContent) {//bison : (déterminer type); instructContent = valAccess{ type,listType.size() }; listType.push_back($1);
			executionPile.push(instructContent.second);
		}},
	{command::_CREATE_VARIABLE_,
		[](instruction& instructContent) {//bison (type name = expression) : instructContent = valAccess{ $1 (type var),stringList.size() }; stringList.push_back($2);
			string name = stringList[instructContent.second.tabPos];
			delVal(varAccess{ varType::_string_,instructContent.second.tabPos});//supprimer string du tableau

			//verif types?
			if (variables.find(name) == variables.end()) variables.insert({name,depiler()});
			//else ?
		}},
	{command::_EMPILE_VARIABLE_,
		[](instruction& instructContent) {//bison (name) : instructContent = valAccess{ typeVar::_string_,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau
			
			if (variables.find(name) != variables.end()) executionPile.push(variables[name]);
		}},
	{command::_UPDATE_VARIABLE_,
		[](instruction& instructContent) {//bison (name = expression) : instructContent = valAccess{ typeVar::_string_,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau

			if (variables.find(name) != variables.end()) variables[name] = depiler();
			//else?
		}},
	{command::_CREATE_VAR_TABLE_,
		[](instruction& instructContent) {//bison (tab<type> name) = expression (optionnelle mais doit empiler un varAccess pour le type)) : instructContent = valAccess{ $1 (type var),stringList.size() }; stringList.push_back($2);
			string name = stringList[instructContent.second.tabPos];
			delVal(varAccess{ varType::_string_,instructContent.second.tabPos});//supprimer string du tableau

			//verif types?
			if (tableaux.find(name) == tableaux.end()) {
				varAccess value = depiler();
				tabAccess declaration = {memoryLayer.size(), value.type};//ordre de declaration

				if (value.tabPos != -1) {
					switch(value.type) {
					case valType::_int_:
						declaration.valuesPos.push_back(intArray.size());
						intArray.push_back(intList[value.tabPos]);
						break;
					case valType::_double_:
						declaration.valuesPos.push_back(doubleArray.size());
						doubleArray.push_back(doubleList[value.tabPos]);
						break;
					case valType::_string_:
						declaration.valuesPos.push_back(stringArray.size());
						stringArray.push_back(stringList[value.tabPos]);
						break;
					}
				}

				tableaux.insert({name,declaration});
			}
			//else ?
		}},
	{command::_EMPILE_VAR_TABLE_SIZE_,
		[](instruction& instructContent) {//bison (SIZE name) : instructContent = valAccess{ typeVar::_string_,stringList.size() }; stringList.push_back($1);
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau

			if (tableaux.find(name) != tableaux.end()) executionPile.push(valAccess{ valType::_int_,tableaux[name].valuesPos.size() });
			//else?
		}},
	{command::_EMPILE_VAR_TABLE_ELEMENT_,
		[](instruction& instructContent) {//bison (name[indice]) : instructContent = valAccess{ typeVar::_string_,stringList.size() }; stringList.push_back($1)
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau
			
			if (tableaux.find(name) != tableaux.end()) {
				int tabPos = intList[depiler().tabPos];

				if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
					tabPos = tableaux[name].valuesPos[tabPos];

					switch(tableaux[name].type) {
					case valType::_int_:
						executionPile.push(valAccess{ typeVar::_int_,intList.size() });
						intList.push_back(intArray[tabPos]);
						break;
					case valType::_double_:
						executionPile.push(valAccess{ typeVar::_double_,doubleList.size() });
						doubleList.push_back(doubleArray[tabPos]);
						break;
					case valType::_string_:
						executionPile.push(valAccess{ typeVar::_string_,stringList.size() });
						stringList.push_back(stringArray[tabPos]);
						break;
					}
				}
			}
		}},
	{command::_UPDATE_VAR_TABLE_ELEMENT_,
		[](instruction& instructContent) {//bison (name[indice] = expression) : instructContent = valAccess{ typeVar::_string_,stringList.size() }; stringList.push_back($1), executionPile.push(valAccess{valType::_int_,$2});
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau
			
			if (tableaux.find(name) != tableaux.end()) {
				int tabPos = intList[depiler().tabPos];

				if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
					tabPos = tableaux[name].valuesPos[tabPos];

					switch(tableaux[name].type) {
					case valType::_int_:
						intArray[tabPos] = depiler();
						break;
					case valType::_double_:
						doubleArray[tabPos] = depiler();
						break;
					case valType::_string_:
						stringArray[tabPos] = depiler();
						break;
					}
				}
			}
		}},
	{command::_REMOVE_VAR_TABLE_ELEMENT_,
		[](instruction& instructContent) {//bison (DELETE name[indice]) : instructContent = valAccess{ typeVar::_string_,stringList.size() }; stringList.push_back($1); executionPile.push(valAccess{valType::_int_,$2});
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau
			
			if (tableaux.find(name) != tableaux.end()) {
				int tabPos = intList[depiler().tabPos];

				if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
					tabPos = tableaux[name].valuesPos[tabPos];

					//delTabVal()
				}
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


	{command::_PRINT_,//sortie
		[](instruction& instructContent) {
			valAccess val = depiler();
			//if (val.tabPos != -1)
			printVal("Résultat : ",val,"\n");
		}}
	//entree
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
