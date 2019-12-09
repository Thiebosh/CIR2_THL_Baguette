#include "utils.cpp"

/********************************************************/
/*	PARTIE I : AJOUT ET EXECUTION DES COMMANDES			*/
/********************************************************/
void addInstruct(command command) {
	cout << (int)command << endl;
	instructionList.push_back({ command, { valType::_int_,-1,-1,"" } });
	cout << "fait" << endl;
};
void addInstruct(command command, int intValue) {
	cout << (int)command << "  " << intValue << endl;
	instructionList.push_back({ command, { valType::_int_,intValue,-1,"" } });
	cout << "fait" << endl;
};
void addInstruct(command command, double doubleValue) {
	cout << (int)command << "  " << doubleValue << endl;
	instructionList.push_back({ command, { valType::_double_,-1,doubleValue,"" } });
	cout << "fait" << endl;
};
void addInstruct(command command, string stringValue) {
	cout << (int)command << "  " << stringValue << endl;
	instructionList.push_back({ command, { valType::_string_,-1,-1,stringValue } });
	cout << "fait" << endl;
	cout << "nuéro de commande " <<(int)command::_EXIT_FUNCTION_ << endl;
};


const map<command, functionPointer> executeCommand = {
	{command::_ENTER_BLOCK_,[](valInstruct& instructContent) { enterMemoryLayer();	}},
	{command::_EXIT_BLOCK_,	[](valInstruct& instructContent) { exitMemoryLayer();	}},


	{command::_EMPILE_VALUE_,[](valInstruct& instructContent) { executionPile.push(addVal(instructContent)); }},
	{command::_EMPILE_VARIABLE_,
		[](valInstruct& instructContent) {
			string name = instructContent.stringVal;

			if (variables.top().find(name) != variables.top().end()) {//var existe bien
				//empile une copie qui sera supprimee apres utilisation
				valAccess copy = { variables.top()[name].type };
				switch (copy.type) {
					case valType::_bool_:
						copy.tabPos = boolList.size();
						boolList.push_back(boolList[variables.top()[name].tabPos]);
						break;
					case valType::_int_:
						copy.tabPos = intList.size();
						intList.push_back(intList[variables.top()[name].tabPos]);
						break;
					case valType::_double_:
						copy.tabPos = doubleList.size();
						doubleList.push_back(doubleList[variables.top()[name].tabPos]);
						break;
					case valType::_string_:
						copy.tabPos = stringList.size();
						stringList.push_back(stringList[variables.top()[name].tabPos]);
						break;
				}
				executionPile.push(copy);
			}
			else error(errorCode::unknowVariable);
		}},
	
	{command::_EMPILE_TABLE_SIZE_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_empile_size_);
		}},
	{command::_EMPILE_TABLE_ELEMENT_,
		[](valInstruct& instructContent) {
			executeTabAction(instructContent, tabAction::_empile_case_);
		}},
	

	{command::_PLUS_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_plus_);		}},
	{command::_MOINS_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_moins_); 		}},
	{command::_FOIS_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_fois_); 		}},
	{command::_DIVISE_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_divisePar_);	}},

	{command::_PLUS_,		[](valInstruct& instructContent) { executeOperation(operation::_plus_);		}},
	{command::_MOINS_,		[](valInstruct& instructContent) { executeOperation(operation::_moins_);	}},
	{command::_FOIS_,		[](valInstruct& instructContent) { executeOperation(operation::_fois_);		}},
	{command::_DIVISE_PAR_,	[](valInstruct& instructContent) { executeOperation(operation::_divisePar_); }},

	{command::_AND_,		[](valInstruct& instructContent) { executeComparaison(comparaison::_and_);	}},
	{command::_OR_,			[](valInstruct& instructContent) { executeComparaison(comparaison::_or_);	}},
	{command::_EQUIV_,		[](valInstruct& instructContent) { executeComparaison(comparaison::_equiv_); }},
	{command::_DIFF_,		[](valInstruct& instructContent) { executeComparaison(comparaison::_diff_);	}},
	{command::_INFERIEUR_,	[](valInstruct& instructContent) { executeComparaison(comparaison::_inferieur_); }},
	{command::_SUPERIEUR_,	[](valInstruct& instructContent) { executeComparaison(comparaison::_superieur_); }},
	{command::_SUP_EGAL_,	[](valInstruct& instructContent) { executeComparaison(comparaison::_sup_egal_);	}},
	{command::_INF_EGAL_,	[](valInstruct& instructContent) { executeComparaison(comparaison::_inf_egal_);	}},


	{command::_GOTO_,		[](valInstruct& instructContent) { indexInstruction = instructContent.intVal;/*instruction est entier naturel*/	}},
	{command::_GOTO_TEST_,
		[](valInstruct& instructContent) {
			valAccess testResult = depiler();

			if (testResult.tabPos != -1 &&
				(testResult.type == valType::_bool_ && boolList[testResult.tabPos] == false) ||
				(testResult.type == valType::_int_ && intList[testResult.tabPos] == 0) ||
				(testResult.type == valType::_double_ && doubleList[testResult.tabPos] == 0)) {
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

			valAccess value = castVal(depiler(), varType);//adresse de val a associer a var, convertie ou plante programme

			if (variables.top().find(instructContent.stringVal) == variables.top().end()) {//var est bien nouvelle
				variables.top().insert({instructContent.stringVal,value});
			}
			else error(errorCode::alreadyUseVariable);
		}},
	{command::_UPDATE_VARIABLE_,
		[](valInstruct& instructContent) {
			//recupere nom de var
			string name = instructContent.stringVal;

			valAccess value = depiler();//adresse de val a associer a var
			if (variables.top().find(name) != variables.top().end()) {//var existe bien
				variables.top()[name] = castVal(value, variables.top()[name].type);
			}
			else error(errorCode::unknowVariable);
		}},

	
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
	

	{command::_CREATE_FUNCTION_,
		[](valInstruct& instructContent) {
			if (fonctions.find(instructContent.stringVal) == fonctions.end()) {//fonction est bien nouvelle
				valAccess tmp = castVal(depiler(), valType::_int_);
				int beginInstruct = intList[tmp.tabPos];
				delVal(tmp);

				deque<param> listParam;
				while (!((tmp = depiler()).type == valType::_int_ && intList[tmp.tabPos] == -1)) {//tant que pas fin
					tmp = castVal(tmp, valType::_string_);
					string paramName = stringList[tmp.tabPos];
					delVal(tmp);

					tmp = depiler();
					valType paramType = tmp.type;
					delVal(tmp);

					listParam.push_back({ paramName,paramType });
				}
				delVal(tmp);//consommer le -1 de fin de parametres

				tmp = depiler();
				valType typeFunction = tmp.type;
				delVal(tmp);

				fonctions.insert({instructContent.stringVal,{ beginInstruct,typeFunction,listParam } });
			}
			else error(errorCode::alreadyDeclaredFunction);
		}},
	{command::_CALL_FUNCTION_,
		[](valInstruct& instructContent) {
			if (fonctions.find(instructContent.stringVal) != fonctions.end()) {//fonction existe bien
				stack<valAccess> copyPile = executionPile;//copie pile pour tester son contenu : doit avoir bon nombre de valeurs et bon types
				valAccess tmp;
				for (auto param : fonctions[instructContent.stringVal].listParam) {
					if ((tmp = depiler()).type == valType::_int_ && intList[tmp.tabPos] == -1) error(errorCode::notEnoughArgument);
					castVal(tmp, param.second);//affiche erreur de conversion si impossible
				}
				if (!((tmp = depiler()).type == valType::_int_ && intList[tmp.tabPos] == -1)) error(errorCode::tooMuchArgument);
				executionPile = copyPile;//retablit pile initiale

				appelFonction.push({instructContent.stringVal,indexInstruction});//stocke fonction appellee et adresse retour
				indexInstruction = fonctions[instructContent.stringVal].refInstruct;//saute a adresse debut fonction (enter function)
			}
			else error(errorCode::unknowFunction);
		}},
	{command::_ENTER_FUNCTION_,	
		[](valInstruct& instructContent) { 
			variables.push({});//separation memoire
			tableaux.push({});//separation memoire

			if (!appelFonction.empty()) {//si entre dans une "vraie" fonction, nombre et type des arguments est ok dans pile
				for (auto param : fonctions[instructContent.stringVal].listParam) {
					variables.top().insert({ param.first,castVal(depiler(),param.second) });//depile et initialise variables (nom, valeur castee)
				}
				delVal(depiler());//consommer le -1 de fin de parametres
			}

			enterMemoryLayer();//nettoyage plus simple
		}},
	{command::_EXIT_FUNCTION_,
		[](valInstruct& instructContent) {
			functionCall leavingFonction;
			valType returnType;
			bool returnBool;
			int returnInt;
			double returnDouble;
			string returnString;

			if (!appelFonction.empty()) { //si ne quitte pas le programme, retourne valeur et retrouve adresse d appel
				leavingFonction = appelFonction.top();
				appelFonction.pop();

				returnType = fonctions[leavingFonction.name].returnType;
				valAccess tmp = castVal(depiler(),returnType);
				int tabPos = tmp.tabPos;
				switch(returnType) {
					//case valType::_void_:
						//break;
					case valType::_bool_:
						returnBool = boolList[tabPos];
						break;
					case valType::_int_:
						returnInt = intList[tabPos];
						break;
					case valType::_double_:
						returnDouble = doubleList[tabPos];
						break;
					case valType::_string_:
						returnString = stringList[tabPos];
						break;
				}
				delVal(tmp);
			}
			else return;//quitte programme

			exitMemoryLayer();
			variables.pop();
			tableaux.pop();

			indexInstruction = leavingFonction.returnAdress;
			switch(returnType) {//ajoute la valeur de retour a la pile
				//case valType::_void_:
					//break;
				case valType::_bool_:
					executionPile.push({returnType,(int)boolList.size()});
					boolList.push_back(returnBool);
					break;
				case valType::_int_:
					executionPile.push({returnType,(int)intList.size()});
					intList.push_back(returnInt);
					break;
				case valType::_double_:
					executionPile.push({returnType,(int)doubleList.size()});
					doubleList.push_back(returnDouble);
					break;
				case valType::_string_:
					executionPile.push({returnType,(int)stringList.size()});
					stringList.push_back(returnString);
					break;
			}
		}},


	{command::_WRITE_,//sortie
		[](valInstruct& instructContent) {
			valAccess val = depiler();
			switch (val.type) {
				case valType::_bool_:
					cout << boolList[val.tabPos];
					break;
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
			cout << endl << "Appuyez sur entrée pour continuer... ";
			cin.ignore();
			cin.get();
		}},
	{command::_READ_,
		[](valInstruct& instructContent) {
			string valName = instructContent.stringVal;
			if (variables.top().find(valName) != variables.top().end()){
				switch (variables.top()[valName].type){
					case valType::_int_ :
						cin >> intList[variables.top()[valName].tabPos];
						break;
					case valType::_double_ :
						cin >> doubleList[variables.top()[valName].tabPos];
						break;
					case valType::_string_ :
						cin >> stringList[variables.top()[valName].tabPos]; 
						break;
				}
			}
			else error(errorCode::unknowVariable);
		}}
};


/********************************************************/
/*	PARTIE II : MAIN FONCTIONS							*/
/********************************************************/
void displayGeneratedProgram() {
	cout << endl << "==== CODE GENERE ====" << endl;

	int i = 0;
	string name;
	int size;
	int tabPos;
	valAccess value;
	for (instruction instructContent : instructionList) {
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
		case command::_EMPILE_TABLE_SIZE_:
		name =instructContent.second.stringVal;
		if (tableaux.top().find(name) != tableaux.top().end()) {//var existe bien
			switch(tableaux.top()[name].type) {
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
		case command::_EMPILE_TABLE_SIZE_:
				//IDEM
			name =instructContent.second.stringVal;
			if (tableaux.top().find(name) != tableaux.top().end()) {//var existe bien
				switch(tableaux.top()[name].type) {
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
				//IDEM
				/*
			name = instructContent.second.stringVal;
			tabPos = executionPile.top().tabPos;//recupere val associee a adresse

			if (tabPos > -1 && tabPos < tableaux.top()[name].valuesPos.size()) {
				tabPos = tableaux.top()[name].valuesPos[tabPos];//recupere val a case souhaitee
				switch(tableaux.top()[name].type) {
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
			*/
			break;


		case command::_PLUS_CREMENT_:
			cout << "SOMME LA VARIABLE '" << instructContent.second.stringVal << "' ET LA DERNIERE VALEUR";
			break;
		case command::_MOINS_CREMENT_:
			cout << "SOUSTRAIT LA VARIABLE '" << instructContent.second.stringVal << "' PAR LA DERNIERE VALEUR";
			break;
		case command::_FOIS_CREMENT_:
			cout << "MULTIPLIE LA VARIABLE '" << instructContent.second.stringVal << "' ET LA DERNIERE VALEUR";
			break;
		case command::_DIVISE_CREMENT_:
			cout << "DIVISE LA VARIABLE '" << instructContent.second.stringVal << "' PAR LA DERNIERE VALEUR";
			break;

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
			
		case command::_AND_:
			cout << "COMBINE DEUX DERNIERES VALEURS (ET LOGIQUE)";
			break;
		case command::_OR_:
			cout << "COMBINE DEUX DERNIERES VALEURS (OU LOGIQUE)";
			break;
		case command::_EQUIV_:
			cout << "COMPARE DEUX DERNIERES VALEURS (==)";
			break;
		case command::_DIFF_:
			cout << "COMPARE DEUX DERNIERES VALEURS (!=)";
			break;
		case command::_SUPERIEUR_:
			cout << "COMPARE DEUX DERNIERES VALEURS (<)";
			break;
		case command::_INFERIEUR_:
			cout << "COMPARE DEUX DERNIERES VALEURS (>)";
			break;
		case command::_SUP_EGAL_:
			cout << "COMPARE DEUX DERNIERES VALEURS (<=)";
			break;
		case command::_INF_EGAL_:
			cout << "COMPARE DEUX DERNIERES VALEURS (>=)";
			break;


		case command::_GOTO_:
			cout << "CONTINUER A L'INSTRUCTION " << instructContent.second.intVal;
			break;
		case command::_GOTO_TEST_:
			cout << "SI DERNIERE VALEUR VAUT 0, CONTINUER A L'INSTRUCTION " << instructContent.second.intVal;
			break;


		case command::_CREATE_VARIABLE_:
			cout << "INITIALISE VARIABLE '" << instructContent.second.stringVal << "'";
			break;
		case command::_UPDATE_VARIABLE_:
			cout << "ACTUALISE VARIABLE '" << instructContent.second.stringVal << "'";
			break;

		
			case command::_CREATE_TABLE_:
				cout << "INITIALISE LISTE '" << instructContent.second.stringVal << "'";
				break;
			case command::_ADD_TABLE_ELEMENT_:
				//IDEM
				name = instructContent.second.stringVal;
				if (tableaux.top().find(name) != tableaux.top().end()) {
					value = executionPile.top();

					if (instructContent.second.type == value.type) {
						printVal("PROLONGE TABLEAU " + name + " AVEC ",value);
					}
					else cout << "ERREUR : TYPES DIFFERENTS";
				}
				else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
				break;
			case command::_UPDATE_TABLE_ELEMENT_:
				//IDEM
				name = instructContent.second.stringVal;
				if (tableaux.top().find(name) != tableaux.top().end()) {
					value = executionPile.top();
					tabPos = intList[value.tabPos];

					if (tabPos > -1 && tabPos < tableaux.top()[name].valuesPos.size()) {
						tabPos = tableaux.top()[name].valuesPos[tabPos];
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
				//IDEM
				name = instructContent.second.stringVal;
				if (tableaux.top().find(name) != tableaux.top().end()) {
					value = executionPile.top();
					tabPos = intList[value.tabPos];

					if (tabPos > -1 && tabPos < tableaux.top()[name].valuesPos.size()) {
						tabPos = tableaux.top()[name].valuesPos[tabPos];
						cout << "SUPPRIME INDICE " << tabPos << " DU TABLEAU " << name;
					}
					else cout << "ERREUR : INDICE " << tabPos << "INVALIDE";
				}
				else cout << "ERREUR : TABLEAU " << name << " N'EXISTE PAS";
				break;
		

		case command::_CREATE_FUNCTION_:
			cout << "INITIALISE FONCTION '" << instructContent.second.stringVal << "'";
			break;
		case command::_CALL_FUNCTION_:
			cout << "APPELLE FONCTION '" << instructContent.second.stringVal << "'";
			break;
		case command::_ENTER_FUNCTION_:
			cout << "AJOUTE ZONE D'EXECUTION pour '" << instructContent.second.stringVal << "'";
			break;
		case command::_EXIT_FUNCTION_:
			cout << "SUPPRIME ZONE D'EXECUTION" << endl << endl;
			break;


		case command::_WRITE_:
			cout << "AFFICHE RESULTAT";
			break;
		case command::_STOP_:
			cout << "ATTENDRE ACTION UTILISATEUR";
			break;
		case command::_READ_:
			cout << "RECUPERER VALEUR UTILISATEUR";
			break;

		default:
			cout << "UNKNOW COMMAND : " << (int)instructContent.first;
		}
		cout << endl;
	}

	cout << "=====================" << endl;
}

void saveCommandProgramFile(string folderName, string programName) {
    programName = programName.substr(0,programName.size() - ((string)PROGRAM_EXTENSION).size());
    ofstream file((folderName + programName + COMMAND_EXTENSION).c_str());

    if (file) {
        file << endl << "===== EXECUTION =====" << endl;
        for (instruction instructContent : instructionList) {
            string arguments = "command::";
            switch (instructContent.first) {
                //MEMOIRE - ok
                case command::_ENTER_BLOCK_: 
                    arguments += "_ENTER_BLOCK_"; 
                    break;
                case command::_EXIT_BLOCK_: 
                    arguments += "_EXIT_BLOCK_"; 
                    break;

                //EMPILEMENT - ok
                case command::_EMPILE_VALUE_:
                    arguments += "_EMPILE_VALUE_,"; 
                    switch (instructContent.second.type) {
                        case valType::_int_:
                            arguments += to_string(instructContent.second.intVal);
                            break;
                        case valType::_double_:
                            arguments += to_string(instructContent.second.doubleVal);
                            break;
                        case valType::_string_:
                            arguments += "\"" + instructContent.second.stringVal + "\"";
                            break;
                    }
                    break;
                case command::_EMPILE_VARIABLE_:
                    arguments += "_EMPILE_VARIABLE_,\"" + instructContent.second.stringVal + "\""; 
                    break;

                //OPERATIONS (var to var) - ok
                case command::_PLUS_CREMENT_:
                    arguments += "_PLUS_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_MOINS_CREMENT_:
                    arguments += "_MOINS_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_FOIS_CREMENT_:
                    arguments += "_FOIS_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_DIVISE_CREMENT_:
                    arguments += "_DIVISE_CREMENT_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_PLUS_:
                    arguments += "_PLUS_"; 
                    break;
                case command::_MOINS_:
                    arguments += "_MOINS_"; 
                    break;
                case command::_FOIS_:
                    arguments += "_FOIS_"; 
                    break;
                case command::_DIVISE_PAR_:
                    arguments += "_DIVISE_PAR_"; 
                    break;

                //COMPARAISON - ok
                case command::_AND_:
                    arguments += "_AND_"; 
                    break;
                case command::_OR_:
                    arguments += "_OR_"; 
                    break;
                case command::_EQUIV_:
                    arguments += "_EQUIV_"; 
                    break;
                case command::_DIFF_:
                    arguments += "_DIFF_"; 
                    break;
                case command::_SUPERIEUR_:
                    arguments += "_SUPERIEUR_"; 
                    break;
                case command::_INFERIEUR_:
                    arguments += "_INFERIEUR_"; 
                    break;
                case command::_SUP_EGAL_:
                    arguments += "_SUP_EGAL_"; 
                    break;
                case command::_INF_EGAL_:
                    arguments += "_INF_EGAL_"; 
                    break;

                //SAUTS (conditions, boucles, fonctions) - ok
                case command::_GOTO_:
                    arguments += "_GOTO_," + to_string(instructContent.second.intVal);
                    break;
                case command::_GOTO_TEST_:
                    arguments += "_GOTO_TEST_," + to_string(instructContent.second.intVal);
                    break;

                //VARIABLES - ok
                case command::_CREATE_VARIABLE_:
                    arguments += "_CREATE_VARIABLE_,\"" + instructContent.second.stringVal + "\"";
                    break;
                case command::_UPDATE_VARIABLE_:
                    arguments += "_UPDATE_VARIABLE_,\"" + instructContent.second.stringVal + "\"";
                    break;

                //FONCTIONS - ok
                case command::_CREATE_FUNCTION_:
                    arguments += "_CREATE_FUNCTION_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_CALL_FUNCTION_:
                    arguments += "_CALL_FUNCTION_,\"" + instructContent.second.stringVal + "\""; 
                    break;
                case command::_ENTER_FUNCTION_:
                    arguments += "_ENTER_FUNCTION_,\"" + instructContent.second.stringVal + "\"";
                    break;
                case command::_EXIT_FUNCTION_:
                    arguments += "_EXIT_FUNCTION_";
                    break;

                //ENTREE SORTIE - ok
                case command::_WRITE_:
                    arguments += "_WRITE_"; 
                    break;
                case command::_STOP_:
                    arguments += "_STOP_"; 
                    break;
                case command::_READ_:
                    arguments += "_READ_,\"" + instructContent.second.stringVal + "\""; 
                    break;
            }

            file << "addInstruct(" << arguments << ");" << endl;
        }
	    file << endl << "=====================" << endl;
        file.close();
    }
}
