#include "memory.cpp"

/********************************************************/
/*	PARTIE I : UTILITAIRES								*/
/********************************************************/
void printVal(globalVariables& allVariables, string beginMessage, valAccess val, string endMessage) {
	cout << beginMessage;
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
		//si trouve \ suivi de t ou n dans la string, le transforme en un seul caractere
		cout << allVariables.stringList[val.tabPos];
		break;
	}
	cout << endMessage;
}

void pauseProcess(string message) {
	cout << endl;
	if (message != "") cout << message << ". ";
	cout << "Appuyez sur entrée pour continuer... ";
	cin.ignore();
	cin.get();
}

void replaceString(string& subject, const string& search, const string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}

void error(globalVariables& allVariables, errorCode cause) {
	cout << "ERREUR A L'INSTRUCTION " << allVariables.indexInstruction - 1 << " : " << allVariables.errorMessage[cause] << endl;
	exit((int)cause + 1);//code erreur
}

valAccess depiler(globalVariables& allVariables) {
	valAccess var;

	if (!allVariables.executionPile.empty()) {
		var = allVariables.executionPile.top();
		allVariables.executionPile.pop();
	}
	else error(allVariables, errorCode::emptyExecutionStack);

	return var;
}


/********************************************************/
/*	PARTIE II : COMBINAISONS							*/
/********************************************************/
valAccess castVal(globalVariables& allVariables, valAccess value, valType cast, bool isVar) {
	if (value.type == cast) return value;

	else if (value.type != valType::_string_ && cast != valType::_string_) {
		valAccess result = { cast };//string vers string
		switch (cast) {
		case valType::_bool_:
			result.tabPos = allVariables.boolList.size();
			switch (value.type) {
			case valType::_int_:
				allVariables.boolList.push_back(allVariables.intList[value.tabPos]);
				break;
			case valType::_double_:
				allVariables.boolList.push_back(allVariables.doubleList[value.tabPos]);
				break;
			}
			break;

		case valType::_int_:
			result.tabPos = allVariables.intList.size();
			switch (value.type) {
			case valType::_bool_:
				allVariables.intList.push_back(allVariables.boolList[value.tabPos]);
				break;
			case valType::_double_:
				allVariables.intList.push_back((int)allVariables.doubleList[value.tabPos]);
				break;
			}
			break;

		case valType::_double_:
			result.tabPos = allVariables.doubleList.size();
			switch (value.type) {
			case valType::_bool_:
				allVariables.doubleList.push_back(allVariables.boolList[value.tabPos]);
				break;
			case valType::_int_:
				allVariables.doubleList.push_back(allVariables.intList[value.tabPos]);
				break;
			}
			break;
		}

		if (!isVar) delVal(allVariables, value);
		return result;
	}

	error(allVariables, errorCode::conversionType);
}

valAccess toTab(globalVariables& allVariables, valAccess value) {
	valType type = value.type;
	int newpos = -1;
	switch(type) {
		case valType::_int_:
			newpos = allVariables.intArray.size();
			allVariables.intArray.push_back(allVariables.intList[value.tabPos]);
			break;
		case valType::_double_:
			newpos = allVariables.doubleArray.size();
			allVariables.doubleArray.push_back(allVariables.doubleList[value.tabPos]);
			break;
		case valType::_string_:
			newpos = allVariables.stringArray.size();
			allVariables.stringArray.push_back(allVariables.stringList[value.tabPos]);
			break;
	}
	delVal(allVariables, value);
	return {type,newpos};
}

void executeOperation(globalVariables& allVariables, operation operation) {
	//recupere valeurs
	valAccess val2 = depiler(allVariables);
	valAccess val1 = depiler(allVariables);

	if (val1.type == valType::_string_ && val2.type == valType::_string_) {
		switch (operation) {
		case operation::_plus_://concatenation
			allVariables.stringList[val1.tabPos] += allVariables.stringList[val2.tabPos];
			break;
		default:
			error(allVariables, errorCode::unsupportedOperation);
			break;
		}
		delVal(allVariables, val1);
		if (val2.tabPos > val1.tabPos) --val2.tabPos;//prend en compte suppression
		delVal(allVariables, val2);

		allVariables.executionPile.push({ valType::_string_,(int)allVariables.stringList.size() });
		allVariables.stringList.push_back(allVariables.stringList[val1.tabPos]);
	}

	else {//si pas deux reels, erreur levee par fonction cast
		val2 = castVal(allVariables, val2, valType::_double_);
		val1 = castVal(allVariables, val1, valType::_double_);

		double result(0);//cast au plus haut
		switch (operation) {
		case operation::_plus_:
			result = allVariables.doubleList[val1.tabPos] + allVariables.doubleList[val2.tabPos];
			break;
		case operation::_moins_:
			result = allVariables.doubleList[val1.tabPos] - allVariables.doubleList[val2.tabPos];
			break;
		case operation::_fois_:
			result = allVariables.doubleList[val1.tabPos] * allVariables.doubleList[val2.tabPos];
			break;
		case operation::_divisePar_:
			result = allVariables.doubleList[val1.tabPos] / allVariables.doubleList[val2.tabPos];
			break;
		}
		delVal(allVariables, val1);
		if (val2.tabPos > val1.tabPos) --val2.tabPos;//prend en compte suppression
		delVal(allVariables, val2);

		allVariables.executionPile.push({ valType::_double_,(int)allVariables.doubleList.size() });
		allVariables.doubleList.push_back(result);
	}
}

void executeComparaison(globalVariables& allVariables, comparaison operation) {
	//recupere valeurs
	valAccess val2 = depiler(allVariables);
	valAccess val1 = depiler(allVariables);

	if (val1.type != valType::_string_ && val2.type != valType::_string_) {
		val2 = castVal(allVariables, val2, valType::_double_);
		val1 = castVal(allVariables, val1, valType::_double_);

		double result(0);//cast au plus haut
		switch (operation) {
		case comparaison::_and_:
			result = allVariables.doubleList[val1.tabPos] && allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_or_:
			result = allVariables.doubleList[val1.tabPos] || allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_equiv_:
			result = allVariables.doubleList[val1.tabPos] == allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_diff_:
			result = allVariables.doubleList[val1.tabPos] != allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_inferieur_:
			result = allVariables.doubleList[val1.tabPos] > allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_superieur_:
			result = allVariables.doubleList[val1.tabPos] < allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_inf_egal_:
			result = allVariables.doubleList[val1.tabPos] >= allVariables.doubleList[val2.tabPos];
			break;
		case comparaison::_sup_egal_:
			result = allVariables.doubleList[val1.tabPos] <= allVariables.doubleList[val2.tabPos];
			break;
		}
		delVal(allVariables, val1);
		if (val2.tabPos > val1.tabPos) --val2.tabPos;//prend en compte suppression
		delVal(allVariables, val2);

		allVariables.executionPile.push({ valType::_double_,(int)allVariables.doubleList.size() });
		allVariables.doubleList.push_back(result);
	}
	else {
		delVal(allVariables, val1);
		if (val2.tabPos > val1.tabPos) --val2.tabPos;//prend en compte suppression
		delVal(allVariables, val2);
		error(allVariables, errorCode::conversionType);
	}
}

void executeCrement(globalVariables& allVariables, string varName, operation operation) {
	//recupere valeur
	valAccess valCast = depiler(allVariables);

	if (allVariables.currentExecution.top().variables.find(varName) != allVariables.currentExecution.top().variables.end()) {//var existe bien
		//1 : changer val en double ou string
		double valDouble(0);
		string valString("");
		if (valCast.type == valType::_string_) {
			valString = allVariables.stringList[valCast.tabPos];
		}
		else {
			valCast = castVal(allVariables, valCast, valType::_double_);
			valDouble = allVariables.doubleList[valCast.tabPos];
		}
		delVal(allVariables, valCast);

		//2 : changer variable content en double ou string
		valAccess varCast;
		double varDouble(0);
		string varString("");
		if (allVariables.currentExecution.top().variables[varName].type == valType::_string_) {
			varCast = allVariables.currentExecution.top().variables[varName];
			varString = allVariables.stringList[varCast.tabPos];
		}
		else {
			varCast = castVal(allVariables, allVariables.currentExecution.top().variables[varName], valType::_double_, 1);
			varDouble = allVariables.doubleList[varCast.tabPos];
		}

		//3 : opérer les deux
		switch (operation) {
		case operation::_plus_:
			if (varCast.type == valType::_string_) varString += valString;
			else varDouble += valDouble;
			break;
		case operation::_moins_:
			varDouble -= valDouble;
			break;
		case operation::_fois_:
			varDouble *= valDouble;
			break;
		case operation::_divisePar_:
			varDouble /= valDouble;
			break;
		}

		//4 : intégrer résultat dans variable (verif de type)
		switch (allVariables.currentExecution.top().variables[varName].type) {
		case valType::_int_:
			allVariables.intList[allVariables.currentExecution.top().variables[varName].tabPos] = (int)varDouble;
			break;
		case valType::_double_:
			allVariables.doubleList[allVariables.currentExecution.top().variables[varName].tabPos] = varDouble;
			break;
		case valType::_string_:
			allVariables.stringList[allVariables.currentExecution.top().variables[varName].tabPos] = varString;
			break;
		}
		
		//5 : ajouter copie a la pile si besoin
		if (allVariables.currentExecution.top().variables[varName].type == valType::_string_) {
			allVariables.executionPile.push({ valType::_string_,(int)allVariables.stringList.size() });
			allVariables.stringList.push_back(varString);
		}
		else {
			allVariables.executionPile.push({ valType::_double_,(int)allVariables.doubleList.size() });
			allVariables.doubleList.push_back(varDouble);
		}
	}
	else {
		error(allVariables, errorCode::unknowVariable);
	}
}


/********************************************************/
/*	PARTIE III : TABLEAUX								*/
/********************************************************/
void executeTabAction(globalVariables& allVariables, valInstruct& instructContent, tabAction action) {
	string name = instructContent.stringVal;

	if ((action == tabAction::_create_ && allVariables.currentExecution.top().tableaux.find(name) == allVariables.currentExecution.top().tableaux.end()) ||//tab est bien nouveau
		(action != tabAction::_create_ && allVariables.currentExecution.top().tableaux.find(name) != allVariables.currentExecution.top().tableaux.end())) {//tab existe bien
		int tabPos;
		valAccess value;
		valAccess tmp;
		valType type;
		tabAccess declaration;
		switch (action) {
	
		case tabAction::_empile_size_:
			switch(allVariables.currentExecution.top().tableaux[name].type) {
			case valType::_int_:
				allVariables.executionPile.push({ valType::_int_,(int)allVariables.intList.size() });
				break;
			case valType::_double_:
				allVariables.executionPile.push({ valType::_int_,(int)allVariables.doubleList.size() });
				break;
			case valType::_string_:
				allVariables.executionPile.push({ valType::_int_,(int)allVariables.stringList.size() });
				break;
			}
			allVariables.intList.push_back(allVariables.currentExecution.top().tableaux[name].valuesPos.size());//name
			break;
		
		case tabAction::_empile_case_:
			value = depiler(allVariables);
			tabPos = allVariables.intList[value.tabPos];//recupere val associee a adresse
			delVal(allVariables, value);

			if(tabPos==-1){
				switch((allVariables.currentExecution.top().tableaux[name]).type){
					case valType::_int_:
						tabPos = allVariables.intArray.size()-1;
						break;

					case valType::_double_:
						tabPos = allVariables.doubleArray.size()-1;
						break;

					case valType::_string_:
						tabPos = allVariables.stringArray.size()-1;
						break;
				}
			}

			if (tabPos > -1 && tabPos < (int)allVariables.currentExecution.top().tableaux[name].valuesPos.size()) {
				tabPos = allVariables.currentExecution.top().tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

				switch(allVariables.currentExecution.top().tableaux[name].type) {
					case valType::_int_:
						allVariables.executionPile.push({ valType::_int_,(int)allVariables.intList.size() });
						allVariables.intList.push_back(allVariables.intArray[tabPos]);
						break;
					case valType::_double_:
						allVariables.executionPile.push({ valType::_double_,(int)allVariables.doubleList.size() });
						allVariables.doubleList.push_back(allVariables.doubleArray[tabPos]);
						break;
					case valType::_string_:
						allVariables.executionPile.push({ valType::_string_,(int)allVariables.stringList.size() });
						allVariables.stringList.push_back(allVariables.stringArray[tabPos]);
						break;
					}
			}
			break;

		case tabAction::_create_:
			tmp = depiler(allVariables);
			type = tmp.type;
			delVal(allVariables, tmp);

			declaration = {(unsigned)allVariables.memoryLayer.size(), type};//ordre de declaration

			declaration.valuesPos.push_back(toTab(allVariables, castVal(allVariables, depiler(allVariables),type)).tabPos);

			allVariables.currentExecution.top().tableaux.insert({name,declaration});
			break;
			
		case tabAction::_add_:
			value = depiler(allVariables);//supprime pas : besoin de transmettre valeur associee
			
			allVariables.currentExecution.top().tableaux[name].valuesPos.push_back(toTab(allVariables, castVal(allVariables, value,allVariables.currentExecution.top().tableaux[name].type)).tabPos);
			break;

		case tabAction::_update_:
			value = depiler(allVariables);
			tabPos = allVariables.intList[value.tabPos];//recupere val associee a adresse
			delVal(allVariables, value);

			if (tabPos > -1 && tabPos < (int)allVariables.currentExecution.top().tableaux[name].valuesPos.size()) {
				tabPos = allVariables.currentExecution.top().tableaux[name].valuesPos[tabPos];//recupere val a case souhaitee

				valAccess value = depiler(allVariables);//supprime pas : besoin de transmettre valeur associee
				if (allVariables.currentExecution.top().tableaux[name].type == value.type) {
					switch(value.type) {
					case valType::_int_:
						allVariables.intArray[tabPos] = allVariables.intList[value.tabPos];
						break;
					case valType::_double_:
						allVariables.doubleArray[tabPos] = allVariables.doubleList[value.tabPos];
						break;
					case valType::_string_:
						allVariables.stringArray[tabPos] = allVariables.stringList[value.tabPos];
						break;
					}
				}
				//else ? cast here

				delVal(allVariables, value);//stocke en typeArray
			}
			break;

		case tabAction::_remove_:
			value = depiler(allVariables);
			tabPos = allVariables.intList[value.tabPos];//recupere val associee a adresse
			delVal(allVariables, value);

			if (tabPos > -1 && tabPos < (int)allVariables.currentExecution.top().tableaux[name].valuesPos.size()) {
				delTabVal(allVariables, name,tabPos);
			}
			break;
		}
	}
	//else : pb
	// retester existence du tableau 
}


