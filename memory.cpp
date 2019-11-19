#include <iostream>

#include <string>
#include <deque>
#include <stack>
#include <map>

using namespace std;

/********************************************************/
/*														*/
/*	PARTIE I : GESTION DE LA MEMOIRE					*/
/*		SOUS PARTIE 1 : STOCKAGE DE VALEURS TYPEES		*/
/*		SOUS PARTIE 2 : PILE DE VALEURS	EN UTILISATION	*/
/*		SOUS PARTIE 3 : OPERATION SUR LES VALEURS		*/
/*		SOUS PARTIE 4 : UTILISATION DES VARIABLES		*/
/*		SOUS PARTIE 5 : LIBERATION DE MEMOIRE	    	*/
/*		SOUS PARTIE 6 : PILE DE BLOCS MEMOIRE			*/
/*														*/
/********************************************************/


/********************************************************/
/*		SOUS PARTIE 1 : STOCKAGE DE VALEURS TYPEES		*/
/********************************************************/
enum class valType {//fixe types
	_int_,
	_double_,
	_string_
};

typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int tabPos = -1;//valeur par defaut : flag d'invalidation
} valAccess;

deque<int>		intList;
deque<double>	doubleList;
deque<string>	stringList;

void printVal(string beginMessage, valAccess val, string endMessage = "") {
	cout << beginMessage;
	switch (val.type) {
	case valType::_int_:
		cout << intList[val.tabPos];
		break;
	case valType::_double_:
		cout << doubleList[val.tabPos];
		break;
	case valType::_string_:
		cout << stringList[val.tabPos];
		break;
	}
	cout << endMessage;
}


/********************************************************/
/*		SOUS PARTIE 2 : PILE DE VALEURS	EN UTILISATION	*/
/********************************************************/
stack<valAccess> executionPile;

valAccess depiler() {
	valAccess var;
	if (executionPile.size() > 0) {
		var = executionPile.top();
		executionPile.pop();
	}
	return var;//controler tabPos != -1
}


/********************************************************/
/*		SOUS PARTIE 3 : OPERATION SUR LES VALEURS		*/
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
	else if ((val1.type == valType::_int_ || val1.type == valType::_double_) &&
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
/*		SOUS PARTIE 4 : DECLARATION DES VARIABLES		*/
/********************************************************/
map<string, valAccess> variables;

//stockage tableau a part : doit preserver valeur ajoutee dans couche memoire (voir part 5) differente de celle supportant la declaration
typedef struct {
	unsigned int memoryLayer = 0;
	valType type = valType::_int_;
	deque<int> valuesPos;
} tabAccess;

deque<int>		intArray;
deque<double>	doubleArray;
deque<string>	stringArray;

map<string, tabAccess> tableaux;


/********************************************************/
/*		SOUS PARTIE 5 : LIBERATION DE MEMOIRE	    	*/
/********************************************************/
void delVal(valAccess val) {
	//PILE : decremente references tableau des valeurs suivantes
	if (val.type == executionPile.top().type && val.tabPos == executionPile.top().tabPos) executionPile.pop();//dernier element? le retire
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
/*		SOUS PARTIE 6 : PILE DE BLOCS MEMOIRE			*/
/********************************************************/
typedef struct {
	unsigned int intListSize = 0;
	unsigned int doubleListSize = 0;
	unsigned int stringListSize = 0;
} memoryState;

stack<memoryState> memoryLayer;

void enterMemoryLayer() {
	memoryLayer.push(memoryState{ (unsigned)intList.size(),(unsigned)doubleList.size(),(unsigned)stringList.size() });
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
		if ((var.second.type == valType::_int_	  && (unsigned)var.second.tabPos >= initial.intListSize) ||
			(var.second.type == valType::_double_ && (unsigned)var.second.tabPos >= initial.doubleListSize) ||
			(var.second.type == valType::_string_ && (unsigned)var.second.tabPos >= initial.stringListSize)) {
			delVar(var.first);
		}
	}

	//supprime valeurs (non affectees a des variables) declarees dans le bloc (supprime par la fin pour + d'efficacite (deque) et de surete)
	while (intList.size() > initial.intListSize)		delVal(valAccess{ valType::_int_,	(int)intList.size() - 1 });
	while (doubleList.size() > initial.doubleListSize)	delVal(valAccess{ valType::_double_,(int)doubleList.size() - 1 });
	while (stringList.size() > initial.stringListSize)	delVal(valAccess{ valType::_string_,(int)stringList.size() - 1 });
}
