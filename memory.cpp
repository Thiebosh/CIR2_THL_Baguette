#include "declarations.cpp"

/********************************************************/
/*														*/
/*	PARTIE I : GESTION DE LA MEMOIRE					*/
/*		SOUS PARTIE 1 : STOCKAGE DE VALEURS TYPEES		*/
/*		SOUS PARTIE 2 : PILE DE VALEURS	EN UTILISATION	*/
/*		SOUS PARTIE 3 : UTILISATION DES VARIABLES		*/
/*		SOUS PARTIE 4 : LIBERATION DE MEMOIRE	    	*/
/*		SOUS PARTIE 5 : PILE DE BLOCS MEMOIRE			*/
/*														*/
/********************************************************/


/********************************************************/
/*		SOUS PARTIE 1 : STOCKAGE DE VALEURS TYPEES		*/
/********************************************************/

void printVal(string beginMessage, valAccess val, string endMessage = "") {
	cout << beginMessage;
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
		//si trouve \ suivi de t ou n dans la string, le transforme en un seul caractere
		cout << stringList[val.tabPos];
		break;
	}
	cout << endMessage;
}


/********************************************************/
/*		SOUS PARTIE 2 : PILE DE VALEURS	EN UTILISATION	*/
/********************************************************/


valAccess depiler() {
	valAccess var;
	if (!executionPile.empty()) {
		var = executionPile.top();
		executionPile.pop();
	}

	return var;//controler tabPos != -1
}


/********************************************************/
/*		SOUS PARTIE 3 : DECLARATION DES VARIABLES		*/
/********************************************************/


/********************************************************/
/*		SOUS PARTIE 4 : LIBERATION DE MEMOIRE	    	*/
/********************************************************/
void delVal(valAccess val) {
	//PILE : decremente references tableau des valeurs suivantes
	if (!executionPile.empty() && val.type == executionPile.top().type && val.tabPos == executionPile.top().tabPos) executionPile.pop();//dernier element? le retire
	else {
		stack<valAccess> reversePile;
		while (!executionPile.empty()) {
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
			executionPile.push(reversePile.top());
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
		case valType::_bool_:
			if (val.tabPos == boolList.size() - 1) boolList.pop_back();
			else boolList.erase(boolList.begin() + val.tabPos);
			break;
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
	variables.erase(name);
}

void delTabVal(string tabName, int tabCase) {
	//decaler les adresses superieures a l'adresse a supprimer
	for (auto tab : tableaux) {//parcourt tous les tableaux
		if (tab.second.type == tableaux[tabName].type) {//si tableau est du type impacte
			for (int tabPos = 0; tabPos < tab.second.valuesPos.size(); ++tabPos) {//checke toutes ses adresses
				if (tab.second.valuesPos[tabPos] > tableaux[tabName].valuesPos[tabCase]) {//si adresse au dela de celle supprimee
					tableaux[tab.first].valuesPos[tabPos]--;//reduit d'un (1 valeur supprimee)
				}
			}
		}
	}

	//supprimer le stockage de l'adresse
	tableaux[tabName].valuesPos.erase(tableaux[tabName].valuesPos.begin() + tabCase);

	//supprimer le stockage de la valeur
	switch (tableaux[tabName].type) {
	case valType::_bool_:
		if (tabCase == boolArray.size() - 1) boolArray.pop_back();
		else boolArray.erase(boolArray.begin() + tabCase);
		break;
	case valType::_int_:
		if (tabCase == intArray.size() - 1) intArray.pop_back();
		else intArray.erase(intArray.begin() + tabCase);
		break;
	case valType::_double_:
		if (tabCase == doubleArray.size() - 1) doubleArray.pop_back();
		else doubleArray.erase(doubleArray.begin() + tabCase);
		break;
	case valType::_string_:
		if (tabCase == stringArray.size() - 1) stringArray.pop_back();
		else stringArray.erase(stringArray.begin() + tabCase);
		break;
	}
}

void delTab(string tabName) {
	switch (tableaux[tabName].type) {
	case valType::_bool_:
		while (tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = tableaux[tabName].valuesPos[tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == boolArray.size() - 1) boolArray.pop_back();
			else boolArray.erase(boolArray.begin() + tabPos);

			//supprime l'adresse
			tableaux[tabName].valuesPos.pop_back();
		}
		break;
	case valType::_int_:
		while (tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = tableaux[tabName].valuesPos[tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == intArray.size() - 1) intArray.pop_back();
			else intArray.erase(intArray.begin() + tabPos);

			//supprime l'adresse
			tableaux[tabName].valuesPos.pop_back();
		}
		break;
	case valType::_double_:
		while (tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = tableaux[tabName].valuesPos[tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == doubleArray.size() - 1) doubleArray.pop_back();
			else doubleArray.erase(doubleArray.begin() + tabPos);

			//supprime l'adresse
			tableaux[tabName].valuesPos.pop_back();
		}
		break;
	case valType::_string_:
		while (tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = tableaux[tabName].valuesPos[tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == stringArray.size() - 1) stringArray.pop_back();
			else stringArray.erase(stringArray.begin() + tabPos);

			//supprime l'adresse
			tableaux[tabName].valuesPos.pop_back();
		}
		break;
	}
	//supprime tableau
	tableaux.erase(tabName);
}


/********************************************************/
/*		SOUS PARTIE 5 : PILE DE BLOCS MEMOIRE			*/
/********************************************************/


void enterMemoryLayer() {
	memoryLayer.push({ (unsigned)boolList.size(),(unsigned)intList.size(),(unsigned)doubleList.size(),(unsigned)stringList.size() });
}

void exitMemoryLayer() {
	//supprime tableaux declares dans le bloc
	map<string, tabAccess> tableauxCopy = tableaux;
	for (auto tab : tableauxCopy) {
		if (tab.second.memoryLayer == memoryLayer.size()) {
			delTab(tab.first);
		}
	}

	memoryState initial;
	if (!memoryLayer.empty()) {//pas besoin de declarer nouvel espace memoire au demarrage
		initial = memoryLayer.top();
		memoryLayer.pop();
	}

	//supprime variables declarees dans le bloc
	map<string, valAccess> variablesCopy = variables;
	for (auto var : variablesCopy) {
		if ((var.second.type == valType::_bool_	  && (unsigned)var.second.tabPos > initial.boolListSize) ||
			(var.second.type == valType::_int_	  && (unsigned)var.second.tabPos > initial.intListSize) ||
			(var.second.type == valType::_double_ && (unsigned)var.second.tabPos > initial.doubleListSize) ||
			(var.second.type == valType::_string_ && (unsigned)var.second.tabPos > initial.stringListSize)) {
			delVar(var.first);
		}
	}

	//supprime valeurs (non affectees a des variables) declarees dans le bloc (supprime par la fin pour + d'efficacite (deque) et de surete)
	while (boolList.size() > initial.boolListSize)		delVal({ valType::_int_,	(int)boolList.size() - 1 });
	while (intList.size() > initial.intListSize)		delVal({ valType::_int_,	(int)intList.size() - 1 });
	while (doubleList.size() > initial.doubleListSize)	delVal({ valType::_double_,	(int)doubleList.size() - 1 });
	while (stringList.size() > initial.stringListSize)	delVal({ valType::_string_,	(int)stringList.size() - 1 });
}
