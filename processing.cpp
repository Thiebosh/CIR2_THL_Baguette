//gestion affichage
#include <iostream>
#include <iomanip>
#include <cmath>

#include "memory.cpp"


using namespace std;

/********************************************************/
/*														*/
/*	PARTIE II : GESTION DES COMMANDES					*/
/*		SOUS PARTIE 1 : OPERATION SUR LES VALEURS		*/
/*		SOUS PARTIE 2 : DECLARATION DES COMMANDES		*/
/*		SOUS PARTIE 3 : GESTION DES INSTRUCTIONS		*/
/*		SOUS PARTIE 4 : GESTION DES TABLEAUX			*/
/*		SOUS PARTIE 5 : EXECUTION SELON LES COMMANDES	*/
/*														*/
/********************************************************/

/********************************************************/
/*		SOUS PARTIE 1 : OPERATION SUR LES VALEURS		*/
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

	delVal(val1);
	delVal(val2);

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
		executionPile.push(valAccess({ valType::_int_,(int)intList.size() }));
		intList.push_back(result);
	}
	else if ((val1.type == valType::_int_ || val1.type == valType::_double_) ||
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
		executionPile.push(valAccess({ valType::_double_,(int)doubleList.size() }));
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
		executionPile.push(valAccess({ valType::_double_,(int)doubleList.size() }));
		stringList.push_back(result);
	}
	else {//string + int ou double
		//erreur? "tostring"? repetition?
	}
}

/********************************************************/
/*		SOUS PARTIE 2 : DECLARATION DES COMMANDES		*/
/********************************************************/
enum class command {
	//MEMOIRE
	_ENTER_BLOCK_,
	_EXIT_BLOCK_,

	//EMPILEMENT
	_EMPILE_VALUE_,
	_EMPILE_VARIABLE_,
	_EMPILE_TABLE_SIZE_,
	_EMPILE_TABLE_ELEMENT_,

	//OPERATIONS (var to var)
	_PLUS_,
	_MOINS_,
	_FOIS_,
	_DIVISE_PAR_,

	//SAUTS (conditions, boucles, fonctions)
	_GOTO_,
	_GOTO_TEST_,

	//VARIABLES
	_CREATE_VARIABLE_,
	_UPDATE_VARIABLE_,

	//TABLEAUX
	_CREATE_TABLE_,
	_ADD_TABLE_ELEMENT_,
	_UPDATE_TABLE_ELEMENT_,
	_REMOVE_TABLE_ELEMENT_,

	//ENTREE SORTIE
	_PRINT_
};


/********************************************************/
/*		SOUS PARTIE 3 : GESTION DES INSTRUCTIONS		*/
/********************************************************/
typedef pair<command, valAccess> instruction;
deque<instruction> instructionList;

void addInstruct(command command, int tabPos = -1, valType type = valType::_int_) {
	instructionList.push_back({ command, { type,tabPos } });
};

unsigned int indexInstruction = 0;   // compteur instruction 


/********************************************************/
/*		SOUS PARTIE 4 : GESTION DES TABLEAUX			*/
/********************************************************/
enum class tabAction {//fixe operations
	_empile_size_,
	_empile_case_,
	_create_,
	_add_,
	_update_,
	_remove_
};

void executeTabAction(instruction& instructContent, tabAction action) {
	string name = stringList[instructContent.second.tabPos];
	delVal(instructContent.second);//string recupere : peut supprimer du tableau

	if ((action == tabAction::_create_ && tableaux.find(name) == tableaux.end()) ||//tab est bien nouveau
		(action != tabAction::_create_ && tableaux.find(name) != tableaux.end())) {//tab existe bien
		int tabPos;
		valAccess value;
		tabAccess declaration;
		switch (action) {
		case tabAction::_empile_size_:
			executionPile.push({ valType::_int_,(int)intList.size() });
			intList.push_back(tableaux[name].valuesPos.size());//name
			break;
			
		case tabAction::_empile_case_:
			value = depiler();
			tabPos = intList[value.tabPos];//recupere val associee a adresse
			delVal(value);

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				tabPos = tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

				switch(tableaux[name].type) {
				case valType::_int_:
					executionPile.push({ valType::_int_,(int)intList.size() });
					intList.push_back(intArray[tabPos]);
					break;
				case valType::_double_:
					executionPile.push({ valType::_double_,(int)doubleList.size() });
					doubleList.push_back(doubleArray[tabPos]);
					break;
				case valType::_string_:
					executionPile.push({ valType::_string_,(int)stringList.size() });
					stringList.push_back(stringArray[tabPos]);
					break;
				}
			}
			break;

		case tabAction::_create_:
			value = depiler();//supprime pas : besoin de transmettre valeur associee

			if (instructContent.second.type == value.type /*&& value.tabPos != -1*/) {//verif types
				declaration = {(unsigned)memoryLayer.size(), value.type};//ordre de declaration

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

				tableaux.insert({name,declaration});
			}
			//else ? cast here

			delVal(value);//stocke en typeArray
			break;

		case tabAction::_add_:
			value = depiler();//supprime pas : besoin de transmettre valeur associee
			if (tableaux[name].type == value.type) {
				switch(value.type) {
				case valType::_int_:
					tableaux[name].valuesPos.push_back(intArray.size());
					intArray.push_back(intList[value.tabPos]);
					break;
				case valType::_double_:
					tableaux[name].valuesPos.push_back(doubleArray.size());
					doubleArray.push_back(doubleList[value.tabPos]);
					break;
				case valType::_string_:
					tableaux[name].valuesPos.push_back(doubleArray.size());
					stringArray.push_back(stringList[value.tabPos]);
					break;
				}
			}
			//else ? cast here

			delVal(value);//stocke en typeArray
			break;

		case tabAction::_update_:
			value = depiler();
			tabPos = intList[value.tabPos];//recupere val associee a adresse
			delVal(value);

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				tabPos = tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

				valAccess value = depiler();//supprime pas : besoin de transmettre valeur associee
				if (tableaux[name].type == value.type) {
					switch(value.type) {
					case valType::_int_:
						intArray[tabPos] = intList[value.tabPos];
						break;
					case valType::_double_:
						doubleArray[tabPos] = doubleList[value.tabPos];
						break;
					case valType::_string_:
						stringArray[tabPos] = stringList[value.tabPos];
						break;
					}
				}
				//else ? cast here

				delVal(value);//stocke en typeArray
			}
			break;

		case tabAction::_remove_:
			value = depiler();
			tabPos = intList[value.tabPos];//recupere val associee a adresse
			delVal(value);

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				delTabVal(name,tabPos);
			}
			break;
		}
	}
	//else : pb
}

/*
void executeTabAction(instruction& instructContent, tabAction action) {
	string name = stringList[instructContent.second.tabPos];
	delVal(instructContent.second);//string recupere : peut supprimer du tableau

	if ((action == tabAction::_create_ && tableaux.find(name) == tableaux.end()) ||//tab est bien nouveau
		(action != tabAction::_create_ && tableaux.find(name) != tableaux.end())) {//tab existe bien
		int tabPos;
		valAccess value;
		tabAccess declaration;

		if (action == tabAction::_empile_size_) {
			executionPile.push({ valType::_int_,(int)intList.size() });
			intList.push_back(tableaux[name].valuesPos.size());//name
		}
		else {
			value = depiler();
			if (action != tabAction::_create_ && action != tabAction::_add_) {//supprime pas : besoin de transmettre valeur associee
				tabPos = intList[value.tabPos];//recupere val associee a adresse
				delVal(value);
			}

			if (action == tabAction::_create_) {
				if (instructContent.second.type == value.type /*&& value.tabPos != -1* /) {//verif types
					declaration = {(unsigned)memoryLayer.size(), value.type};//ordre de declaration

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

					tableaux.insert({name,declaration});
				}
				//else ? cast here

				delVal(value);//stocke en typeArray
			}

			else if (action == tabAction::_add_) {
				if (tableaux[name].type == value.type) {
					switch(value.type) {
					case valType::_int_:
						tableaux[name].valuesPos.push_back(intArray.size());
						intArray.push_back(intList[value.tabPos]);
						break;
					case valType::_double_:
						tableaux[name].valuesPos.push_back(doubleArray.size());
						doubleArray.push_back(doubleList[value.tabPos]);
						break;
					case valType::_string_:
						tableaux[name].valuesPos.push_back(doubleArray.size());
						stringArray.push_back(stringList[value.tabPos]);
						break;
					}
				}
				//else ? cast here

				delVal(value);//stocke en typeArray
			}

			else if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				if (action == tabAction::_remove_) delTabVal(name,tabPos);
				else {
					tabPos = tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

					if (action == tabAction::_empile_case_) {
						switch(tableaux[name].type) {
						case valType::_int_:
							executionPile.push({ valType::_int_,(int)intList.size() });
							intList.push_back(intArray[tabPos]);
							break;
						case valType::_double_:
							executionPile.push({ valType::_double_,(int)doubleList.size() });
							doubleList.push_back(doubleArray[tabPos]);
							break;
						case valType::_string_:
							executionPile.push({ valType::_string_,(int)stringList.size() });
							stringList.push_back(stringArray[tabPos]);
							break;
						}
					}
					else {//(action == tabAction::_update_)
						value = depiler();//supprime pas : besoin de transmettre valeur associee
						if (tableaux[name].type == value.type) {
							switch(value.type) {
							case valType::_int_:
								intArray[tabPos] = intList[value.tabPos];
								break;
							case valType::_double_:
								doubleArray[tabPos] = doubleList[value.tabPos];
								break;
							case valType::_string_:
								stringArray[tabPos] = stringList[value.tabPos];
								break;
							}
						}
						//else ? cast here

						delVal(value);//stocke en typeArray
					}
				}
			}
		}
	}
	//else : pb
}
*/

/********************************************************/
/*		SOUS PARTIE 5 : EXECUTION SELON LES COMMANDES	*/
/********************************************************/
typedef void (*functionPointer)(instruction& instructContent);

const map<command, functionPointer> executeCommand = {
	{command::_ENTER_BLOCK_,
		[](instruction& instructContent) {
			enterMemoryLayer();
		}},
	{command::_EXIT_BLOCK_,
		[](instruction& instructContent) {
			exitMemoryLayer();
		}},


	{command::_EMPILE_VALUE_,
		[](instruction& instructContent) {
			executionPile.push(instructContent.second);
		}},
	{command::_EMPILE_VARIABLE_,
		[](instruction& instructContent) {
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau
			
			if (variables.find(name) != variables.end()) {//var existe bien
				executionPile.push(variables[name]);
			}
		}},
	{command::_EMPILE_TABLE_SIZE_,
		[](instruction& instructContent) {
			executeTabAction(instructContent, tabAction::_empile_size_);
		}},
	{command::_EMPILE_TABLE_ELEMENT_,
		[](instruction& instructContent) {
			executeTabAction(instructContent, tabAction::_empile_case_);
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


	{command::_GOTO_,
		[](instruction& instructContent) {
			indexInstruction = instructContent.second.tabPos;//instruction est entier naturel
		}},
	{command::_GOTO_TEST_,
		[](instruction& instructContent) {
			valAccess testResult = depiler();
			if (testResult.tabPos != -1 && 
				(testResult.type == valType::_int_ && intList[testResult.tabPos] == 0) ||
				(testResult.type == valType::_double_ && doubleList[testResult.tabPos] == 0)) {
				indexInstruction = instructContent.second.tabPos;//cas if not 0 : incrementation prealable
			}
			delVal(testResult);
		}},


	{command::_CREATE_VARIABLE_,
		[](instruction& instructContent) {
			string name = stringList[instructContent.second.tabPos];
			delVal({ valType::_string_,instructContent.second.tabPos});//supprimer string du tableau

			if (variables.find(name) == variables.end()) {//var est bien nouvelle
				valAccess value = depiler();//supprime pas : transmet adresse
				
				if (instructContent.second.type == value.type) {
					variables.insert({name,value});
				}
				//else ? cast here
			}
			//else ?
		}},
	{command::_UPDATE_VARIABLE_,
		[](instruction& instructContent) {
			string name = stringList[instructContent.second.tabPos];
			delVal(instructContent.second);//supprimer string du tableau

			if (variables.find(name) != variables.end()) {//var existe bien
				variables[name] = depiler();//ne supprime pas : transmet adresse
			}
			//else? cast here
		}},


	{command::_CREATE_TABLE_,
		[](instruction& instructContent) {
			executeTabAction(instructContent, tabAction::_create_);
		}},
	{command::_ADD_TABLE_ELEMENT_,
		[](instruction& instructContent) {
			executeTabAction(instructContent, tabAction::_add_);
		}},
	{command::_UPDATE_TABLE_ELEMENT_,
		[](instruction& instructContent) {
			executeTabAction(instructContent, tabAction::_update_);
		}},
	{command::_REMOVE_TABLE_ELEMENT_,
		[](instruction& instructContent) {
			executeTabAction(instructContent, tabAction::_remove_);
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
/*
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

		case command::_GOTO_TEST_:
			printVal("IF ZERO, JUMP TO INSTRUCTION ", instructContent.second);
			break;
		case command::_GOTO_:
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
}*/


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
