#include "declarations.cpp"


/********************************************************/
/*	PARTIE I : AJOUT									*/
/********************************************************/
valAccess addVal(globalVariables& allVariables, valInstruct instructContent) {
	int tabPos = 0;
	switch (instructContent.type) {
	case valType::_int_:
		tabPos = allVariables.intList.size();
		allVariables.intList.push_back(instructContent.intVal);
		break;
	case valType::_double_:
		tabPos = allVariables.doubleList.size();
		allVariables.doubleList.push_back(instructContent.doubleVal);
		break;
	case valType::_string_:
		tabPos = allVariables.stringList.size();
		allVariables.stringList.push_back(instructContent.stringVal);
		break;
	}
	return { instructContent.type,tabPos };
}

valAccess addVar(globalVariables& allVariables, valInstruct instructContent) {
	string name = allVariables.stringList[castVal(allVariables, depiler(allVariables), valType::_string_).tabPos];

	allVariables.currentExecution.top().variables.insert({ name,addVal(allVariables, instructContent) });
	return { instructContent.type,allVariables.currentExecution.top().variables[name].tabPos };
}


/********************************************************/
/*	PARTIE II : SUPRESSION								*/
/********************************************************/
void delVal(globalVariables& allVariables, valAccess val) {
	//PILE : decremente references tableau des valeurs suivantes
	if (!allVariables.executionPile.empty() && val.type == allVariables.executionPile.top().type && val.tabPos == allVariables.executionPile.top().tabPos) allVariables.executionPile.pop();//dernier element? le retire
	else {
		stack<valAccess> reversePile;
		while (!allVariables.executionPile.empty()) {
			valAccess pileVal = depiler(allVariables);
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
			allVariables.executionPile.push(reversePile.top());
			reversePile.pop();
		}
	}

	//VARIABLES : decremente references tableau des valeurs suivantes
	for (auto var : allVariables.currentExecution.top().variables) {
		if (var.second.type == val.type && var.second.tabPos > val.tabPos) {
			allVariables.currentExecution.top().variables[var.first].tabPos--;
		}
	}

	//supprimer la valeur du tableau
	switch (val.type) {
	case valType::_bool_:
		if (val.tabPos == allVariables.boolList.size() - 1) allVariables.boolList.pop_back();
		else allVariables.boolList.erase(allVariables.boolList.begin() + val.tabPos);
		break;
	case valType::_int_:
		if (val.tabPos == allVariables.intList.size() - 1) allVariables.intList.pop_back();
		else allVariables.intList.erase(allVariables.intList.begin() + val.tabPos);
		break;
	case valType::_double_:
		if (val.tabPos == allVariables.doubleList.size() - 1) allVariables.doubleList.pop_back();
		else allVariables.doubleList.erase(allVariables.doubleList.begin() + val.tabPos);
		break;
	case valType::_string_:
		if (val.tabPos == allVariables.stringList.size() - 1) allVariables.stringList.pop_back();
		else allVariables.stringList.erase(allVariables.stringList.begin() + val.tabPos);
		break;
	}
}

void delVar(globalVariables& allVariables, string name) {
	//decremente references tableau des valeurs suivantes dans la pile et la liste de variables + supprime valeur du tableau
	delVal(allVariables, allVariables.currentExecution.top().variables[name]);
	allVariables.currentExecution.top().variables.erase(name);
}

void delTabVal(globalVariables& allVariables, string tabName, int tabCase) {
	//decaler les adresses superieures a l'adresse a supprimer
	for (auto tab : allVariables.currentExecution.top().tableaux) {//parcourt tous les tableaux
		if (tab.second.type == allVariables.currentExecution.top().tableaux[tabName].type) {//si tableau est du type impacte
			for (int tabPos = 0; tabPos < (int)tab.second.valuesPos.size(); ++tabPos) {//checke toutes ses adresses
				if (tab.second.valuesPos[tabPos] > allVariables.currentExecution.top().tableaux[tabName].valuesPos[tabCase]) {//si adresse au dela de celle supprimee
					allVariables.currentExecution.top().tableaux[tab.first].valuesPos[tabPos]--;//reduit d'un (1 valeur supprimee)
				}
			}
		}
	}

	//supprimer le stockage de l'adresse
	allVariables.currentExecution.top().tableaux[tabName].valuesPos.erase(allVariables.currentExecution.top().tableaux[tabName].valuesPos.begin() + tabCase);

	//supprimer le stockage de la valeur
	switch (allVariables.currentExecution.top().tableaux[tabName].type) {
	case valType::_int_:
		if (tabCase == allVariables.intArray.size() - 1) allVariables.intArray.pop_back();
		else allVariables.intArray.erase(allVariables.intArray.begin() + tabCase);
		break;
	case valType::_double_:
		if (tabCase == allVariables.doubleArray.size() - 1) allVariables.doubleArray.pop_back();
		else allVariables.doubleArray.erase(allVariables.doubleArray.begin() + tabCase);
		break;
	case valType::_string_:
		if (tabCase == allVariables.stringArray.size() - 1) allVariables.stringArray.pop_back();
		else allVariables.stringArray.erase(allVariables.stringArray.begin() + tabCase);
		break;
	}
}

void delTab(globalVariables& allVariables, string tabName) {
	switch (allVariables.currentExecution.top().tableaux[tabName].type) {
	case valType::_int_:
		while (allVariables.currentExecution.top().tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = allVariables.currentExecution.top().tableaux[tabName].valuesPos[allVariables.currentExecution.top().tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == allVariables.intArray.size() - 1) allVariables.intArray.pop_back();
			else allVariables.intArray.erase(allVariables.intArray.begin() + tabPos);

			//supprime l'adresse
			allVariables.currentExecution.top().tableaux[tabName].valuesPos.pop_back();
		}
		break;
	case valType::_double_:
		while (allVariables.currentExecution.top().tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = allVariables.currentExecution.top().tableaux[tabName].valuesPos[allVariables.currentExecution.top().tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == allVariables.doubleArray.size() - 1) allVariables.doubleArray.pop_back();
			else allVariables.doubleArray.erase(allVariables.doubleArray.begin() + tabPos);

			//supprime l'adresse
			allVariables.currentExecution.top().tableaux[tabName].valuesPos.pop_back();
		}
		break;
	case valType::_string_:
		while (allVariables.currentExecution.top().tableaux[tabName].valuesPos.size() > 0) {//tant que non vide
			//supprime la valeur
			int tabPos = allVariables.currentExecution.top().tableaux[tabName].valuesPos[allVariables.currentExecution.top().tableaux[tabName].valuesPos.size() - 1];
			if (tabPos == allVariables.stringArray.size() - 1) allVariables.stringArray.pop_back();
			else allVariables.stringArray.erase(allVariables.stringArray.begin() + tabPos);

			//supprime l'adresse
			allVariables.currentExecution.top().tableaux[tabName].valuesPos.pop_back();
		}
		break;
	}
	//supprime tableau
	allVariables.currentExecution.top().tableaux.erase(tabName);
}


/********************************************************/
/*	PARTIE III : COUCHES MEMOIRES						*/
/********************************************************/
void enterMemoryLayer(globalVariables& allVariables) {
	allVariables.memoryLayer.push({ (unsigned)allVariables.boolList.size(),(unsigned)allVariables.intList.size(),(unsigned)allVariables.doubleList.size(),(unsigned)allVariables.stringList.size() });
}

void exitMemoryLayer(globalVariables& allVariables) {
	//supprime tableaux declares dans le bloc
	map<string, tabAccess> tableauxCopy = allVariables.currentExecution.top().tableaux;
	for (auto tab : tableauxCopy) {
		if (tab.second.memoryLayer == allVariables.memoryLayer.size()) {
			delTab(allVariables, tab.first);
		}
	}

	memoryState initial = allVariables.memoryLayer.top();//memoryState a 1 minimum
	allVariables.memoryLayer.pop();

	//supprime variables declarees dans le bloc
	map<string, valAccess> variablesCopy = allVariables.currentExecution.top().variables;
	for (auto var : variablesCopy) {
		if ((var.second.type == valType::_bool_ 	&& (unsigned)var.second.tabPos >= initial.boolListSize) ||
			(var.second.type == valType::_int_ 		&& (unsigned)var.second.tabPos >= initial.intListSize) ||
			(var.second.type == valType::_double_ 	&& (unsigned)var.second.tabPos >= initial.doubleListSize) ||
			(var.second.type == valType::_string_ 	&& (unsigned)var.second.tabPos >= initial.stringListSize)) {
			delVar(allVariables, var.first);
		}
	}

	//supprime valeurs (non affectees a des variables) declarees dans le bloc (supprime par la fin pour + d'efficacite (deque) et de surete)
	while (allVariables.boolList.size() > initial.boolListSize)		delVal(allVariables, { valType::_int_,	(int)allVariables.boolList.size() - 1 });
	while (allVariables.intList.size() > initial.intListSize)		delVal(allVariables, { valType::_int_,	(int)allVariables.intList.size() - 1 });
	while (allVariables.doubleList.size() > initial.doubleListSize)	delVal(allVariables, { valType::_double_,	(int)allVariables.doubleList.size() - 1 });
	while (allVariables.stringList.size() > initial.stringListSize)	delVal(allVariables, { valType::_string_,	(int)allVariables.stringList.size() - 1 });
}
