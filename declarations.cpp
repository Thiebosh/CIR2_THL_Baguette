//memory
#include <string>
#include <deque>
#include <stack>
#include <map>

//processing (gestion affichage)
#include <iostream>
#include <cstdio> //getchar
#include <iomanip>
#include <cmath>

//files
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#define DEFAULT_FOLDER "programFiles/"
#define PROGRAM_EXTENSION ".choco"
#define COMMAND_EXTENSION ".command"
#define COMPILED_EXTENSION ".chocapic"


using namespace std;


/********************************************************/
/*	PARTIE I : ENUMERATIONS								*/
/********************************************************/
enum class valType {//fixe types
	_void_,
	_bool_,
	_int_,
	_double_,
	_string_
};

enum class operation {//fixe operations
	_plus_,
	_moins_,
	_fois_,
	_divisePar_
};

enum class comparaison {//fixe comparaison
	_and_,
	_or_,
	_equiv_,
	_diff_,
	_inferieur_,
	_superieur_,
	_inf_egal_,
	_sup_egal_
};


enum class tabAction {//fixe operations
	_empile_size_,
	_empile_case_,
	_create_,
	_add_,
	_update_,
	_remove_
};

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
	_PLUS_CREMENT_,
	_MOINS_CREMENT_,
	_FOIS_CREMENT_,
	_DIVISE_CREMENT_,
	_PLUS_,
	_MOINS_,
	_FOIS_,
	_DIVISE_PAR_,

	//COMPARAISON
	_AND_,
	_OR_,
	_EQUIV_,
	_DIFF_,
	_SUPERIEUR_,
	_INFERIEUR_,
	_SUP_EGAL_,
	_INF_EGAL_,

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

	//FONCTIONS
	_ENTER_FUNCTION_,
	_EXIT_FUNCTION_,
	_END_FUNCTION_,
	_CREATE_FUNCTION_,
	_CALL_FUNCTION_,

	//ENTREE SORTIE
	_WRITE_,
	_STOP_,
	_READ_
};

enum class errorCode {
	folderCreationFailed,
	unknowCommand,
	conversionType,
	unknowVariable,
	alreadyUseVariable,
	emptyExecutionStack,
	alreadyDeclaredFunction,
	unknowFunction,
	notEnoughArgument,
	tooMuchArgument,
	missingReturn
};


/********************************************************/
/*	PARTIE II : TYPES PERSONNALISES						*/
/********************************************************/
typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int tabPos = -1;//valeur par defaut : flag d'invalidation
} valAccess;

typedef struct {//stockage tableau a part : doit preserver valeur ajoutee dans couche memoire (voir part 5) differente de celle supportant la declaration
	unsigned int memoryLayer = 0;
	valType type = valType::_int_;
	deque<int> valuesPos;
} tabAccess;

typedef struct {
	unsigned int boolListSize = 0;
	unsigned int intListSize = 0;
	unsigned int doubleListSize = 0;
	unsigned int stringListSize = 0;
} memoryState;

typedef struct {//initialiser dans ordre de déclaration
	valType type = valType::_int_;
	int intVal = -1;
	double doubleVal = -1;
	string stringVal = "";
} valInstruct;

typedef pair<string,valType> param;

typedef struct {
	int refInstruct = -1;
	valType returnType = valType::_int_;
	deque<param> listParam;
} functionAccess;

typedef struct {
	string name;
	unsigned int returnAdress;
	//references locales
	map<string, valAccess> variables;
	map<string, tabAccess> tableaux;
} functionCall;

typedef struct {
	//valeurs simples : pile, variables
	deque<bool>     boolList;
	deque<int>		intList;
	deque<double>	doubleList;
	deque<string>	stringList;

	//valeurs multiples : tableaux
	deque<int>		intArray;
	deque<double>	doubleArray;
	deque<string>	stringArray;

	//couche memoire
	stack<memoryState> memoryLayer;

	//references globales
	map<string, functionAccess> fonctions;

	//piles d'execution
	stack<valAccess> executionPile;
	stack<functionCall> currentExecution;

	//instructions
	unsigned int indexInstruction = 0;// compteur instruction

	//erreurs
	map<errorCode, string> errorMessage = { //peut etre passe en parametres
		{errorCode::folderCreationFailed,	"[SYSTEME] Echec de création du dossier"},
		{errorCode::emptyExecutionStack,	"[EXECUTION] pile vide"},
		{errorCode::conversionType,			"[TYPE] types incompatibles - échec de conversion"},

		{errorCode::unknowCommand,			"[EXECUTION] commande inconnue"},
		{errorCode::unknowVariable,			"[VARIABLE] nom de variable inconnu"},
		{errorCode::unknowFunction,			"[FONCTION] nom de fonction inconnu"},

		{errorCode::alreadyUseVariable,		"[VARIABLE] nom de variable déjà en utilisation"},
		{errorCode::alreadyDeclaredFunction,"[FONCTION] nom de fonction déjà utilisé"},

		{errorCode::notEnoughArgument,		"[FONCTION] pas assez de valeurs en paramètres"},
		{errorCode::tooMuchArgument,		"[FONCTION] trop de valeurs en paramètres"},
		{errorCode::missingReturn,			"[FONCTION] valeur de retour attendue"}
	};
} globalVariables;

typedef pair<command, valInstruct> instruction;

typedef void (*functionPointer)(valInstruct& instructContent, globalVariables& allVariables);//necessaire pour map de commandes - fonctions lambda


/********************************************************/
/*	PARTIE III : VARIABLE GLOBALE OBLIGATOIRE (BISON)	*/
/********************************************************/
deque<instruction> instructionList;//necessairement global

/********************************************************/
/*	PARTIE IV : PROTOTYPES								*/
/********************************************************/
// Memory
//		Part 1
valAccess addVal(globalVariables& allVariables, valInstruct instructContent);
valAccess addVar(globalVariables& allVariables, valInstruct instructContent);
//		Part 2
void delVal(globalVariables& allVariables, valAccess val);
void delVar(globalVariables& allVariables, string name);
void delTabVal(globalVariables& allVariables, string tabName, int tabCase);
void delTab(globalVariables& allVariables, string tabName);
//		Part 3
void enterMemoryLayer(globalVariables& allVariables);
void exitMemoryLayer(globalVariables& allVariables);

// Utils
//		Part 1
void printVal(globalVariables& allVariables, string beginMessage, valAccess val, string endMessage = "");
void pauseProcess(string message = "");
void replaceString(string& subject, const string& search, const string& replace);
void error(globalVariables& allVariables, errorCode cause);
valAccess depiler(globalVariables& allVariables);
//		Part 2
valAccess castVal(globalVariables& allVariables, valAccess value, valType cast, bool isVar = 0);
void executeOperation(globalVariables& allVariables, operation operation);
void executeComparaison(globalVariables& allVariables, comparaison comparaison);
void executeCrement(globalVariables& allVariables, string varName, operation operation);
//		Part 3
void executeTabAction(globalVariables& allVariables, instruction& instructContent, tabAction action);


// Processing
//		Part 1
void addInstruct(command command);
void addInstruct(command command, int intValue);
void addInstruct(command command, double doubleValue);
void addInstruct(command command, string stringValue);
//const map<command, functionPointer> executeCommand;
//		Part 2
void displayGeneratedProgram(globalVariables& allVariables);
void saveCommandProgramFile(string folderName, string programName);
