#include "utils.cpp"

const map<command, functionPointer> executeCommand = {
	{command::_ENTER_BLOCK_,[](valInstruct& instructContent) { enterMemoryLayer();	}},
	{command::_EXIT_BLOCK_,	[](valInstruct& instructContent) { exitMemoryLayer();	}},


	{command::_EMPILE_VALUE_,[](valInstruct& instructContent) { executionPile.push(addVal(instructContent)); }},
	{command::_EMPILE_VARIABLE_,
		[](valInstruct& instructContent) {
			string name = instructContent.stringVal;

			if (variables.find(name) != variables.end()) {//var existe bien
				//empile une copie qui sera supprimee apres utilisation
				valAccess copy = { variables[name].type };
				switch (copy.type) {
					case valType::_bool_:
						copy.tabPos = boolList.size();
						boolList.push_back(boolList[variables[name].tabPos]);
						break;
					case valType::_int_:
						copy.tabPos = intList.size();
						intList.push_back(intList[variables[name].tabPos]);
						break;
					case valType::_double_:
						copy.tabPos = doubleList.size();
						doubleList.push_back(doubleList[variables[name].tabPos]);
						break;
					case valType::_string_:
						copy.tabPos = stringList.size();
						stringList.push_back(stringList[variables[name].tabPos]);
						break;
				}
				executionPile.push(copy);
			}
			else error(errorCode::unknowVariable);
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

	{command::_INCREMENT_,
		[](valInstruct& instructContent) {
			string name = instructContent.stringVal;

			if (variables.find(name) != variables.end()) {//var existe bien
				valAccess copy = { variables[name].type };//copie supprimee apres utilisation

				if (variables[name].type == valType::_int_) {
					++intList[variables[name].tabPos];

					copy.tabPos = intList.size();
					intList.push_back(intList[variables[name].tabPos]);
					executionPile.push(copy);
				}
				else if (variables[name].type == valType::_double_) {
					++doubleList[variables[name].tabPos];

					copy.tabPos = intList.size();
					doubleList.push_back(doubleList[variables[name].tabPos]);
					executionPile.push(copy);
				}
			}
			else error(errorCode::unknowVariable);
		}},
	{command::_DECREMENT_,
		[](valInstruct& instructContent) {
			string name = instructContent.stringVal;

			if (variables.find(name) != variables.end()) {//var existe bien
				valAccess copy = { variables[name].type };

				if (variables[name].type == valType::_int_) {
					--intList[variables[name].tabPos];

					copy.tabPos = intList.size();
					intList.push_back(intList[variables[name].tabPos]);
					executionPile.push(copy);
				}
				else if (variables[name].type == valType::_double_) {
					--doubleList[variables[name].tabPos];

					copy.tabPos = intList.size();
					doubleList.push_back(doubleList[variables[name].tabPos]);
					executionPile.push(copy);
				}
			}
			else error(errorCode::unknowVariable);
		}},
	{command::_PLUS_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_plus_);		}},
	{command::_MOINS_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_moins_); 		}},
	{command::_FOIS_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_fois_); 		}},
	{command::_DIVISE_CREMENT_,	[](valInstruct& instructContent) { executeCrement(instructContent.stringVal, operation::_divisePar_);	}},

	{command::_PLUS_,		[](valInstruct& instructContent) { executeOperation(operation::_plus_);		}},
	{command::_MOINS_,		[](valInstruct& instructContent) { executeOperation(operation::_moins_);	}},
	{command::_FOIS_,		[](valInstruct& instructContent) { executeOperation(operation::_fois_);		}},
	{command::_DIVISE_PAR_,	[](valInstruct& instructContent) { executeOperation(operation::_divisePar_);}},

	{command::_AND_,		[](valInstruct& instructContent) { executeComparaison(comparaison::_and_);	}},
	{command::_OR_,			[](valInstruct& instructContent) { executeComparaison(comparaison::_or_);	}},
	{command::_EQUIV_,		[](valInstruct& instructContent) { executeComparaison(comparaison::_equiv_);}},
	{command::_DIFF_,		[](valInstruct& instructContent) { executeComparaison(comparaison::_diff_);	}},
	{command::_INFERIEUR_,	[](valInstruct& instructContent) { executeComparaison(comparaison::_inferieur_);}},
	{command::_SUPERIEUR_,	[](valInstruct& instructContent) { executeComparaison(comparaison::_superieur_);}},
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
	{command::_GOTO_TEST_INV_,
		[](valInstruct& instructContent) {
			valAccess testResult = depiler();

			if (testResult.tabPos != -1 &&
				(testResult.type == valType::_bool_ && boolList[testResult.tabPos] == true) ||
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

			valAccess value = castVal(depiler(), varType);//adresse de val a associer a var, convertie ou plante programme

			if (variables.find(instructContent.stringVal) == variables.end()) {//var est bien nouvelle
				variables.insert({instructContent.stringVal,value});
			}
			else error(errorCode::alreadyUseVariable);
		}},
	{command::_UPDATE_VARIABLE_,
		[](valInstruct& instructContent) {
			//recupere nom de var
			string name = instructContent.stringVal;

			valAccess value = depiler();//adresse de val a associer a var
			if (variables.find(name) != variables.end()) {//var existe bien
				variables[name] = castVal(value, variables[name].type);
			}
			else error(errorCode::unknowVariable);
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

		case command::_INCREMENT_:
			cout << "AJOUTE 1 A LA VARIABLE '" << instructContent.second.stringVal << "'";
			break;
		case command::_DECREMENT_:
			cout << "ENLEVE 1 A LA VARIABLE '" << instructContent.second.stringVal << "'";
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
			cout << "ACTUALISE VARIABLE '" << instructContent.second.stringVal << "'";
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
