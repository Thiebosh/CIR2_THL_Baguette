//gestion affichage
#include <iostream>
#include <cstdio> //getchar
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
		executionPile.push({ valType::_int_,(int)intList.size() });
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

		executionPile.push({ valType::_double_,(int)doubleList.size() });
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

		executionPile.push({ valType::_string_,(int)stringList.size() });
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
	_GOTO_TEST_INV_,

	//VARIABLES
	_CREATE_VARIABLE_,
	_UPDATE_VARIABLE_,

	//TABLEAUX
	_CREATE_TABLE_,
	_ADD_TABLE_ELEMENT_,
	_UPDATE_TABLE_ELEMENT_,
	_REMOVE_TABLE_ELEMENT_,

	//ENTREE SORTIE
	_PRINT_,
	_STOP_
};


/********************************************************/
/*		SOUS PARTIE 3 : GESTION DES INSTRUCTIONS		*/
/********************************************************/
typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int intVal = -1;
	double doubleVal = -1;
	string stringVal = "";
} valInstruct;

valAccess addVal(valInstruct instructContent) {
	int tabPos = 0;
	switch (instructContent.type) {
		case valType::_int_:
			tabPos = intList.size();
			intList.push_back(instructContent.intVal);
			break;
		case valType::_double_:
			tabPos = doubleList.size();
			doubleList.push_back(instructContent.doubleVal);
			break;
		case valType::_string_:
			tabPos = stringList.size();
			stringList.push_back(instructContent.stringVal);
			break;
	}
	return { instructContent.type,tabPos };
}

valAccess addVar(valInstruct instructContent) {
	string name = stringList[depiler().tabPos];

	variables.insert({ name,addVal(instructContent) });
	return { instructContent.type,variables[name].tabPos };
}


typedef pair<command, valInstruct> instruction;
deque<instruction> instructionList;

void addInstruct(command command) {
	instructionList.push_back({ command, { valType::_int_,-1,-1,"" } });
};
void addInstruct(command command, int intValue) {
	instructionList.push_back({ command, { valType::_int_,intValue,-1,"" } });
};
void addInstruct(command command, double doubleValue) {
	instructionList.push_back({ command, { valType::_double_,-1,doubleValue,"" } });
};
void addInstruct(command command, string stringValue) {
	instructionList.push_back({ command, { valType::_string_,-1,-1,stringValue } });
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

/*
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
			switch(tableaux[name].type) {
			case valType::_int_:
				executionPile.push({ valType::_int_,(int)intList.size() });
				break;
			case valType::_double_:
				executionPile.push({ valType::_int_,(int)doubleList.size() });
				break;
			case valType::_string_:
				executionPile.push({ valType::_int_,(int)stringList.size() });
				break;
			}
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

			if (instructContent.second.type == value.type) {//verif types
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
*/

/********************************************************/
/*		SOUS PARTIE 5 : EXECUTION SELON LES COMMANDES	*/
/********************************************************/
void replaceString(string& subject, const string& search, const string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

typedef void (*functionPointer)(valInstruct& instructContent);

const map<command, functionPointer> executeCommand = {
	{command::_ENTER_BLOCK_,
		[](valInstruct& instructContent) {
			enterMemoryLayer();
		}},
	{command::_EXIT_BLOCK_,
		[](valInstruct& instructContent) {
			exitMemoryLayer();
		}},


	{command::_EMPILE_VALUE_,
		[](valInstruct& instructContent) {
			executionPile.push(addVal(instructContent));
		}},
	{command::_EMPILE_VARIABLE_,
		[](valInstruct& instructContent) {
			if (variables.find(instructContent.stringVal) != variables.end()) {//var existe bien
				//empile une copie qui sera supprimee apres utilisation
				valAccess copy = { variables[instructContent.stringVal].type };
				switch(copy.type) {
					case valType::_int_:
						copy.tabPos = intList.size();
						intList.push_back(intList[variables[instructContent.stringVal].tabPos]);
					case valType::_double_:
						copy.tabPos = doubleList.size();
						doubleList.push_back(doubleList[variables[instructContent.stringVal].tabPos]);
					case valType::_string_:
						copy.tabPos = stringList.size();
						stringList.push_back(stringList[variables[instructContent.stringVal].tabPos]);
					break;
				}
				executionPile.push(copy);
			}
			//else ?
		}},
/*
	{command::_EMPILE_TABLE_SIZE_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_empile_size_);
		}},
	{command::_EMPILE_TABLE_ELEMENT_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_empile_case_);
		}},
*/

	{command::_PLUS_,
		[](valInstruct& instructContent) {
			executeOperation(operation::_plus_);
		}},
	{command::_MOINS_,
		[](valInstruct& instructContent) {
			executeOperation(operation::_moins_);
		}},
	{command::_FOIS_,
		[](valInstruct& instructContent) {
			executeOperation(operation::_fois_);
		}},
	{command::_DIVISE_PAR_,
		[](valInstruct& instructContent) {
			executeOperation(operation::_divisePar_);
		}},


	{command::_GOTO_,
		[](valInstruct& instructContent) {
			indexInstruction = instructContent.intVal;//instruction est entier naturel
		}},
	{command::_GOTO_TEST_,
		[](valInstruct& instructContent) {
			valAccess testResult = depiler();

			if (testResult.tabPos != -1 && 
				(testResult.type == valType::_int_ && intList[testResult.tabPos] == 0) ||
				(testResult.type == valType::_double_ && doubleList[testResult.tabPos] == 0)) {
				indexInstruction = instructContent.intVal;//cas if not 0 : incrementation prealable
			}

			delVal(testResult);
		}},
	{command::_GOTO_TEST_INV_,
		[](valInstruct& instructContent) {
			valAccess testResult = depiler();

			if (testResult.tabPos != -1 && 
				(testResult.type == valType::_int_ && intList[testResult.tabPos] != 0) ||
				(testResult.type == valType::_double_ && doubleList[testResult.tabPos] != 0)) {
				indexInstruction = instructContent.intVal;//cas if not 0 : incrementation prealable
			}

			delVal(testResult);
		}},


	{command::_CREATE_VARIABLE_,
		[](valInstruct& instructContent) {
			//recupere type de val
			valAccess valAdress = depiler();
			valType varType = valAdress.type;
			delVal(valAdress);

			valAccess value = depiler();//adresse de val a associer a var

			if (variables.find(instructContent.stringVal) == variables.end()) {//var est bien nouvelle
				if (varType == value.type) {
					variables.insert({instructContent.stringVal,value});
				}
				//else ? cast here
			}
			//else ?
		}},
	{command::_UPDATE_VARIABLE_,
		[](valInstruct& instructContent) {
			//recupere nom de var
			string name = instructContent.stringVal;

			valAccess value = depiler();//adresse de val a associer a var
			if (variables.find(name) != variables.end()) {//var existe bien
				if (variables[name].type == value.type) {
					variables[name] = value;//ne supprime pas value : transmet adresse
				}
				//else? cast here
			}
			//else ?
		}},

/*
	{command::_CREATE_TABLE_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_create_);
		}},
	{command::_ADD_TABLE_ELEMENT_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_add_);
		}},
	{command::_UPDATE_TABLE_ELEMENT_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_update_);
		}},
	{command::_REMOVE_TABLE_ELEMENT_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_remove_);
		}},
*/

	{command::_PRINT_,//sortie
		[](valInstruct& instructContent) {
			valAccess val = depiler();

			switch (val.type) {
			case valType::_int_:
				cout << intList[val.tabPos];
				break;
			case valType::_double_:
				cout << doubleList[val.tabPos];
				break;
			case valType::_string_:
				string display = stringList[val.tabPos];
				replaceString(display,"\\n","\n");
				replaceString(display,"\\t","\t");
				cout << display;
				break;
			}

			delVal(val);
		}},
	{command::_STOP_,
		[](valInstruct& instructContent) {
			cout << endl << "Entrez un caractère pour continuer... ";
			void* tmp;
			cin >> tmp;
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
	string name;
	int size;
	int tabPos;
	valAccess value;
	for (auto instructContent : instructionList) {
		if (instructContent.first == command::_ENTER_BLOCK_) cout << endl;
		cout << "INSTRUCTION " << setw((streamsize)(1 + log10(instructionList.size()))) << i++ << " - ";

		switch (instructContent.first) {
		case command::_ENTER_BLOCK_:
			cout << "AJOUTE STRATE MEMOIRE";
			break;
		case command::_EXIT_BLOCK_:
			cout << "SUPPRIME STRATE MEMOIRE" << endl;
			break;

		case command::_EMPILE_VALUE_:
			cout << "AJOUTE ";
			switch (instructContent.second.type) {
				case valType::_int_:
					cout << instructContent.second.intVal;
					break;
				case valType::_double_:
					cout << instructContent.second.doubleVal;
					break;
				case valType::_string_:
					cout << "\"" << instructContent.second.stringVal << "\"";
					break;
			}
			cout << " A LA PILE";
			break;
		case command::_EMPILE_VARIABLE_:
			cout << "AJOUTE VALEUR DE '" << instructContent.second.stringVal << "' A LA PILE";
			break;
/*		
		case command::_EMPILE_TABLE_SIZE_:
			name = stringList[instructContent.second.tabPos];
			if (tableaux.find(name) != tableaux.end()) {//var existe bien
				switch(tableaux[name].type) {
				case valType::_int_:
					size = intList.size();
					break;
				case valType::_double_:
					size = doubleList.size();
					break;
				case valType::_string_:
					size = stringList.size();
					break;
				}
				cout << "AJOUTE " << size << " A LA PILE";
			}
			else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
			break;
		case command::_EMPILE_TABLE_ELEMENT_:
			name = stringList[instructContent.second.tabPos];
			tabPos = intList[executionPile.top().tabPos];//recupere val associee a adresse

			if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
				tabPos = tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee
				switch(tableaux[name].type) {
				case valType::_int_:
					cout << "AJOUTE ", intArray[tabPos]," A LA PILE";
					break;
				case valType::_double_:
					cout << "AJOUTE ", doubleArray[tabPos]," A LA PILE";
					break;
				case valType::_string_:
					cout << "AJOUTE ", stringArray[tabPos]," A LA PILE";
					break;
				}
			}
			else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
			break;
*/

		case command::_PLUS_:
			cout << "ADDITIONNE DEUX DERNIERES VALEURS";
			break;
		case command::_MOINS_:
			cout << "SOUSTRAIT DEUX DERNIERES VALEURS";
			break;
		case command::_FOIS_:
			cout << "MULTIPLIE DEUX DERNIERES VALEURS";
			break;
		case command::_DIVISE_PAR_:
			cout << "DIVISE DEUX DERNIERES VALEURS";
			break;


		case command::_GOTO_:
			cout << "CONTINUER A L'INSTRUCTION " << instructContent.second.intVal;
			break;
		case command::_GOTO_TEST_:
			cout << "SI DERNIERE VALEUR VAUT 0, CONTINUER A L'INSTRUCTION " << instructContent.second.intVal;
			break;
		case command::_GOTO_TEST_INV_:
			cout << "SI DERNIERE VALEUR DIFFERENTE DE 0, CONTINUER A L'INSTRUCTION " << instructContent.second.intVal;
			break;


		case command::_CREATE_VARIABLE_:
			if (variables.find(instructContent.second.stringVal) == variables.end()) {//var est bien nouvelle
				cout << "INITIALISE VARIABLE '" << instructContent.second.stringVal << "'";
			}
			else cout << "ERREUR : VARIABLE '" << name << "' EXISTE DEJA";
			break;
		case command::_UPDATE_VARIABLE_:
			cout <<  "ACTUALISE VARIABLE '" << instructContent.second.stringVal << "'";
			break;

/*
		case command::_CREATE_TABLE_:
			name = stringList[instructContent.second.tabPos];
			if (tableaux.find(name) == tableaux.end()) {
				value = executionPile.top();
				
				if (instructContent.second.type == value.type) {
					printVal("INITIALISE TABLEAU " + name + " AVEC ",value);
				}
				else cout << "ERREUR : TYPES DIFFERENTS";
			}
			else cout << "ERREUR : TABLEAU " << name << " EXISTE DEJA";
			break;
		case command::_ADD_TABLE_ELEMENT_:
			name = stringList[instructContent.second.tabPos];
			if (tableaux.find(name) != tableaux.end()) {
				value = executionPile.top();
				
				if (instructContent.second.type == value.type) {
					printVal("PROLONGE TABLEAU " + name + " AVEC ",value);
				}
				else cout << "ERREUR : TYPES DIFFERENTS";
			}
			else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
			break;
		case command::_UPDATE_TABLE_ELEMENT_:
			name = stringList[instructContent.second.tabPos];
			if (tableaux.find(name) != tableaux.end()) {
				value = executionPile.top();
				tabPos = intList[value.tabPos];
				
				if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
					tabPos = tableaux[name].valuesPos[tabPos];
					value = executionPile.top();
					if (instructContent.second.type == value.type) {
						cout << "MODIFIE INDICE " << tabPos << " DU TABLEAU " << name;
						printVal(" AVEC ",value);
					}
					else cout << "ERREUR : TYPES DIFFERENTS";
				}
				else cout << "ERREUR : INDICE " << tabPos << "INVALIDE";
			}
			else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
			break;
		case command::_REMOVE_TABLE_ELEMENT_:
			name = stringList[instructContent.second.tabPos];
			if (tableaux.find(name) != tableaux.end()) {
				value = executionPile.top();
				tabPos = intList[value.tabPos];
				
				if (tabPos > -1 && tabPos < tableaux[name].valuesPos.size()) {
					tabPos = tableaux[name].valuesPos[tabPos];
					cout << "SUPPRIME INDICE " << tabPos << " DU TABLEAU " << name;
				}
				else cout << "ERREUR : INDICE " << tabPos << "INVALIDE";
			}
			else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
			break;
*/

		case command::_PRINT_:
			cout << "AFFICHE RESULTAT";
			break;
		case command::_STOP_:
			cout << "ATTENDRE ACTION UTILISATEUR";
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
			(*(executeCommand.at(instructContent.first))) (instructContent.second);
		}
		else {
			cout << "unknow command : " << (int)instructContent.first << endl;
		}
	}
	cout << endl << "=====================" << endl;
}
