#include "utils.cpp"

/********************************************************/
/*	PARTIE I : AJOUT ET EXECUTION DES COMMANDES			*/
/********************************************************/
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


const map<command, functionPointer> executeCommand = {
	{command::_ENTER_BLOCK_,[](valInstruct& instructContent, globalVariables& allVariables) { enterMemoryLayer(allVariables);	}},
	{command::_EXIT_BLOCK_,	[](valInstruct& instructContent, globalVariables& allVariables) { exitMemoryLayer(allVariables);	}},


	{command::_EMPILE_VALUE_,[](valInstruct& instructContent, globalVariables& allVariables) { allVariables.executionPile.push(addVal(allVariables, instructContent)); }},
	{command::_EMPILE_VARIABLE_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			string name = instructContent.stringVal;

			if (allVariables.currentExecution.top().variables.find(name) != allVariables.currentExecution.top().variables.end()) {//var existe bien
				//empile une copie qui sera supprimee apres utilisation
				valAccess copy = { allVariables.currentExecution.top().variables[name].type };
				switch (copy.type) {
					case valType::_bool_:
						copy.tabPos = allVariables.boolList.size();
						allVariables.boolList.push_back(allVariables.boolList[allVariables.currentExecution.top().variables[name].tabPos]);
						break;
					case valType::_int_:
						copy.tabPos = allVariables.intList.size();
						allVariables.intList.push_back(allVariables.intList[allVariables.currentExecution.top().variables[name].tabPos]);
						break;
					case valType::_double_:
						copy.tabPos = allVariables.doubleList.size();
						allVariables.doubleList.push_back(allVariables.doubleList[allVariables.currentExecution.top().variables[name].tabPos]);
						break;
					case valType::_string_:
						copy.tabPos = allVariables.stringList.size();
						allVariables.stringList.push_back(allVariables.stringList[allVariables.currentExecution.top().variables[name].tabPos]);
						break;
				}
				allVariables.executionPile.push(copy);
			}
			else error(allVariables, errorCode::unknowVariable);
		}},
	
	{command::_EMPILE_TABLE_SIZE_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			executeTabAction(allVariables, instructContent, tabAction::_empile_size_);
		}},
	{command::_EMPILE_TABLE_ELEMENT_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			executeTabAction(allVariables, instructContent, tabAction::_empile_case_);
		}},
	

	{command::_PLUS_CREMENT_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeCrement(allVariables, instructContent.stringVal, operation::_plus_);		}},
	{command::_MOINS_CREMENT_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeCrement(allVariables, instructContent.stringVal, operation::_moins_); 		}},
	{command::_FOIS_CREMENT_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeCrement(allVariables, instructContent.stringVal, operation::_fois_); 		}},
	{command::_DIVISE_CREMENT_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeCrement(allVariables, instructContent.stringVal, operation::_divisePar_);	}},

	{command::_PLUS_,		[](valInstruct& instructContent, globalVariables& allVariables) { executeOperation(allVariables, operation::_plus_);		}},
	{command::_MOINS_,		[](valInstruct& instructContent, globalVariables& allVariables) { executeOperation(allVariables, operation::_moins_);	}},
	{command::_FOIS_,		[](valInstruct& instructContent, globalVariables& allVariables) { executeOperation(allVariables, operation::_fois_);		}},
	{command::_DIVISE_PAR_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeOperation(allVariables, operation::_divisePar_); }},

	{command::_AND_,		[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_and_);	}},
	{command::_OR_,			[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_or_);	}},
	{command::_EQUIV_,		[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_equiv_); }},
	{command::_DIFF_,		[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_diff_);	}},
	{command::_INFERIEUR_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_inferieur_); }},
	{command::_SUPERIEUR_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_superieur_); }},
	{command::_SUP_EGAL_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_sup_egal_);	}},
	{command::_INF_EGAL_,	[](valInstruct& instructContent, globalVariables& allVariables) { executeComparaison(allVariables, comparaison::_inf_egal_);	}},


	{command::_GOTO_,		[](valInstruct& instructContent, globalVariables& allVariables) { allVariables.indexInstruction = instructContent.intVal;/*instruction est entier naturel*/	}},
	{command::_GOTO_TEST_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			valAccess testResult = depiler(allVariables);

			if (testResult.tabPos != -1 &&
				(testResult.type == valType::_bool_ && allVariables.boolList[testResult.tabPos] == false) ||
				(testResult.type == valType::_int_ && allVariables.intList[testResult.tabPos] == 0) ||
				(testResult.type == valType::_double_ && allVariables.doubleList[testResult.tabPos] == 0)) {
				allVariables.indexInstruction = instructContent.intVal;//cas if not 0 : incrementation prealable
			}

			delVal(allVariables, testResult);
		}},


	{command::_CREATE_VARIABLE_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			//recupere type de val
			valAccess valAdress = depiler(allVariables);
			valType varType = valAdress.type;
			delVal(allVariables, valAdress);

			valAccess value = castVal(allVariables, depiler(allVariables), varType);//adresse de val a associer a var, convertie ou plante programme

			if (allVariables.currentExecution.top().variables.find(instructContent.stringVal) == allVariables.currentExecution.top().variables.end()) {//var est bien nouvelle
				allVariables.currentExecution.top().variables.insert({instructContent.stringVal,value});
			}
			else error(allVariables, errorCode::alreadyUseVariable);
		}},
	{command::_UPDATE_VARIABLE_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			//recupere nom de var
			string name = instructContent.stringVal;

			valAccess value = depiler(allVariables);//adresse de val a associer a var
			if (allVariables.currentExecution.top().variables.find(name) != allVariables.currentExecution.top().variables.end()) {//var existe bien
				allVariables.currentExecution.top().variables[name] = castVal(allVariables, value, allVariables.currentExecution.top().variables[name].type);
			}
			else error(allVariables, errorCode::unknowVariable);
		}},

	
		{command::_CREATE_TABLE_,
			[](valInstruct& instructContent, globalVariables& allVariables) {
				executeTabAction(allVariables, instructContent, tabAction::_create_);
			}},
		{command::_ADD_TABLE_ELEMENT_,
			[](valInstruct& instructContent, globalVariables& allVariables) {
				executeTabAction(allVariables, instructContent, tabAction::_add_);
			}},
		{command::_UPDATE_TABLE_ELEMENT_,
			[](valInstruct& instructContent, globalVariables& allVariables) {
				executeTabAction(allVariables, instructContent, tabAction::_update_);
			}},
		{command::_REMOVE_TABLE_ELEMENT_,
			[](valInstruct& instructContent, globalVariables& allVariables) {
				executeTabAction(allVariables, instructContent, tabAction::_remove_);
			}},
	

	{command::_CREATE_FUNCTION_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			if (allVariables.fonctions.find(instructContent.stringVal) == allVariables.fonctions.end()) {//fonction est bien nouvelle
				valAccess tmp = castVal(allVariables, depiler(allVariables), valType::_int_);
				int beginInstruct = allVariables.intList[tmp.tabPos];
				delVal(allVariables, tmp);

				deque<param> listParam;
				while (!((tmp = depiler(allVariables)).type == valType::_int_ && allVariables.intList[tmp.tabPos] == -1)) {//tant que pas fin
					tmp = castVal(allVariables, tmp, valType::_string_);
					string paramName = allVariables.stringList[tmp.tabPos];
					delVal(allVariables, tmp);

					tmp = depiler(allVariables);
					valType paramType = tmp.type;
					delVal(allVariables, tmp);

					listParam.push_back({ paramName,paramType });
				}
				delVal(allVariables, tmp);//consommer le -1 de fin de parametres

				tmp = depiler(allVariables);
				valType typeFunction = tmp.type;
				delVal(allVariables, tmp);

				allVariables.fonctions.insert({instructContent.stringVal,{ beginInstruct,typeFunction,listParam } });
			}
			else error(allVariables, errorCode::alreadyDeclaredFunction);
		}},
	{command::_CALL_FUNCTION_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			if (allVariables.fonctions.find(instructContent.stringVal) != allVariables.fonctions.end()) {//fonction existe bien
				stack<valAccess> copyPile = allVariables.executionPile;//copie pile pour tester son contenu : doit avoir bon nombre de valeurs et bon types
				valAccess tmp;
				for (auto param : allVariables.fonctions[instructContent.stringVal].listParam) {
					if ((tmp = depiler(allVariables)).type == valType::_int_ && allVariables.intList[tmp.tabPos] == -1) error(allVariables, errorCode::notEnoughArgument);
					castVal(allVariables, tmp, param.second);//affiche erreur de conversion si impossible
				}
				if (!((tmp = depiler(allVariables)).type == valType::_int_ && allVariables.intList[tmp.tabPos] == -1)) error(allVariables, errorCode::tooMuchArgument);
				allVariables.executionPile = copyPile;//retablit pile initiale

				allVariables.currentExecution.push({instructContent.stringVal,allVariables.indexInstruction});//stocke fonction appellee et adresse retour
				allVariables.indexInstruction = allVariables.fonctions[instructContent.stringVal].refInstruct;//saute a adresse debut fonction (enter function)
			}
			else error(allVariables, errorCode::unknowFunction);
		}},
	{command::_ENTER_FUNCTION_,	
		[](valInstruct& instructContent, globalVariables& allVariables) {
			if (!allVariables.currentExecution.empty()) {//si entre dans une "vraie" fonction, nombre et type des arguments est ok dans pile
				for (auto param : allVariables.fonctions[instructContent.stringVal].listParam) {
					allVariables.currentExecution.top().variables.insert({ param.first,castVal(allVariables, depiler(allVariables),param.second) });//depile et initialise variables (nom, valeur castee)
				}
				delVal(allVariables, depiler(allVariables));//consommer le -1 de fin de parametres
			}
			else allVariables.currentExecution.push({"",0});//main

			enterMemoryLayer(allVariables);//nettoyage plus simple
		}},
	{command::_EXIT_FUNCTION_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			if (allVariables.currentExecution.size() < 2) {
				exitMemoryLayer(allVariables);
				allVariables.currentExecution.pop();
			}
			else { //si ne quitte pas le programme, retourne valeur et retrouve adresse d appel
				allVariables.indexInstruction = allVariables.currentExecution.top().returnAdress;
				valType returnType = allVariables.fonctions[allVariables.currentExecution.top().name].returnType;
				valAccess tmp = castVal(allVariables, depiler(allVariables), returnType);
				int tabPos = tmp.tabPos;

				int returnInt;
				double returnDouble;
				string returnString;

				switch (returnType) {
					/*case valType::_void_:
						break;
					case valType::_bool_:
						returnBool = allVariables.boolList[tabPos];
						break;*/
				case valType::_int_:
					returnInt = allVariables.intList[tabPos];
					break;
				case valType::_double_:
					returnDouble = allVariables.doubleList[tabPos];
					break;
				case valType::_string_:
					returnString = allVariables.stringList[tabPos];
					break;
				}
				delVal(allVariables, tmp);

				exitMemoryLayer(allVariables);
				allVariables.currentExecution.pop();

				switch (returnType) {//ajoute la valeur de retour a la pile
					/*case valType::_void_:
						break;
					case valType::_bool_:
						allVariables.executionPile.push({returnType,(int)allVariables.boolList.size()});
						allVariables.boolList.push_back(returnBool);
						break;*/
				case valType::_int_:
					allVariables.executionPile.push({ returnType,(int)allVariables.intList.size() });
					allVariables.intList.push_back(returnInt);
					break;
				case valType::_double_:
					allVariables.executionPile.push({ returnType,(int)allVariables.doubleList.size() });
					allVariables.doubleList.push_back(returnDouble);
					break;
				case valType::_string_:
					allVariables.executionPile.push({ returnType,(int)allVariables.stringList.size() });
					allVariables.stringList.push_back(returnString);
					break;
				}
			}
		}},


	{command::_WRITE_,//sortie
		[](valInstruct& instructContent, globalVariables& allVariables) {
			valAccess val = depiler(allVariables);
			switch (val.type) {
				case valType::_bool_:
					cout << allVariables.boolList[val.tabPos];
					break;
				case valType::_int_:
					cout << allVariables.intList[val.tabPos];
					break;
				case valType::_double_:
					cout << allVariables.doubleList[val.tabPos];
					break;
				case valType::_string_:
					string display = allVariables.stringList[val.tabPos];
					replaceString(display,"\\n","\n");
					replaceString(display,"\\t","\t");
					cout << display;
					break;
			}

			delVal(allVariables, val);
		}},
	{command::_STOP_,[](valInstruct& instructContent, globalVariables& allVariables) { pauseProcess(); }},
	{command::_READ_,
		[](valInstruct& instructContent, globalVariables& allVariables) {
			string valName = instructContent.stringVal;
			if (allVariables.currentExecution.top().variables.find(valName) != allVariables.currentExecution.top().variables.end()){
				switch (allVariables.currentExecution.top().variables[valName].type){
					case valType::_int_ :
						cin >> allVariables.intList[allVariables.currentExecution.top().variables[valName].tabPos];
						break;
					case valType::_double_ :
						cin >> allVariables.doubleList[allVariables.currentExecution.top().variables[valName].tabPos];
						break;
					case valType::_string_ :
						cin >> allVariables.stringList[allVariables.currentExecution.top().variables[valName].tabPos]; 
						break;
				}
			}
			else error(allVariables, errorCode::unknowVariable);
		}}
};


/********************************************************/
/*	PARTIE II : MAIN FONCTIONS							*/
/********************************************************/
void displayGeneratedProgram(globalVariables& allVariables) {
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
			cout << "AJOUTE TAILLE DE '" << instructContent.second.stringVal << "' A LA PILE";
			break;
		case command::_EMPILE_TABLE_ELEMENT_:
			cout << "AJOUTE CONTENU D'UNE CASE DE '" << instructContent.second.stringVal << "' A LA PILE";
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
			cout << "PROLONGE LISTE " << name << endl;
			break;
		case command::_UPDATE_TABLE_ELEMENT_:
			cout << "MODIFIE CONTENU D'UNE CASE DE '" << instructContent.second.stringVal << "'";
			break;
		case command::_REMOVE_TABLE_ELEMENT_:
			cout << "SUPPRIME UNE CASE DE '" << instructContent.second.stringVal << "'";
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
    ofstream file((folderName + programName + COMMAND_EXTENSION).c_str());

    if (file) {
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

                //SAUTS (conditions, boucles, allVariables.fonctions) - ok
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

                //allVariables.fonctions - ok
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
        file.close();
    }
}
