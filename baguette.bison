/*fichier bison : parse*/
%{
  #include "fonctions.hpp"

  using namespace std;  

  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char *s) { printf("%s\n", s); return 0; }


  map<string,double> variables; // table de symboles

  vector<pair<int,double>> instructions;
  int currentInstruction = 0;   // compteur instruction 
  inline void addInstruct(int c, double d) { instructions.push_back(make_pair(c, d)); currentInstruction++;};

  // structure pour stocker les adresses pour les sauts condistionnels et autres...
  typedef struct adr {
    int currentInstruction_goto; 
    int currentInstruction_false;
  } t_adress; 
%}

%union{
  double valeur;
  char nom[50];
  t_adress adresse;
}

%token <valeur> NUMBER
%token <nom> IDENTIFIER
%type <valeur> expression
%token <adresse> SI
%token ALORS
%token SINON
%token FINSI
%token REPEAT
%token JUMP 
%token JUMP_IF_NOT_ZERO
%token OUT
%token SIN
%token TAN
%token SQRT

%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */


%%
bloc :
    bloc instruction '\n' 
    |    /* Epsilon */
    ;

instruction : 
    expression { addInstruct(OUT,0);   /* imprimer le résultat de l'expression */  }

    | SI expression '\n'  { 
                            $1.currentInstruction_goto = currentInstruction;  
                            addInstruct(JUMP_IF_NOT_ZERO,0);
                          }
      ALORS '\n' bloc     { 
                            $1.currentInstruction_false = currentInstruction;
                            addInstruct(JUMP,0);
                            instructions[$1.currentInstruction_goto].second = currentInstruction;  
                          }
      SINON '\n' bloc     { instructions[$1.currentInstruction_false].second = currentInstruction; }
      FINSI               {   }

    | REPEAT '(' expression ')' expression { /* TO DO */ }

    | IDENTIFIER '=' expression { 
                                  variables[$1] = $3;
                                  cout << "Affectation de " << $3 << " à " << $1 << endl;
                                }
    |   /* Ligne vide*/
    ;

expression: 
    expression '+' expression     { addInstruct('+', 0);}
    | expression '-' expression     { addInstruct('-', 0);}
    | expression '*' expression     { addInstruct('*', 0);}
    | expression '/' expression     { addInstruct('/', 0);}
    | '(' expression ')'            { }
    | NUMBER                        { addInstruct(NUMBER, $1);}
    | IDENTIFIER                    { addInstruct(IDENTIFIER, variables[$1]);}

    ;
%%
/*
    | SIN '(' expr ')'  { $$ = sin($3); cout << "sin(" << $3 << ") = " << $$ << endl; }
    | TAN '(' expr ')'  { $$ = tan($3); cout << "tan(" << $3 << ") = " << $$ << endl; }
    | SQRT '(' expr ')' { $$ = sqrt($3); cout << "sqrt(" << $3 << ") = " << $$ << endl;}
*/


// Pour imprimer le code généré de manière plus lisible 
string nom(int instruction) {
  switch (instruction){
   case '+'               : return "ADD";
   case '*'               : return "MUL";
   case NUMBER            : return "NUM";
   case OUT               : return "OUT";
   case JUMP_IF_NOT_ZERO  : return "JNZ";   // Jump if not zero
   case JUMP              : return "JMP";   // Unconditional Jump
   default                : return to_string (instruction);
   }
}

void print_program() {
  cout << "==== CODE GENERE ====" << endl;
  int i = 0;
  for (auto instruct : instructions ) cout << i++ << '\t' << nom(instruct.first) << "\t" << instruct.second << endl;
  cout << "=====================" << endl;  
}

double depiler(vector<double> &pile) {
  double t = pile[pile.size()-1];
  //cout << "Dépiler " << t << endl; 
  pile.pop_back();
  return t;
}

void run_program() {
  vector<double> pile; 
  double x,y;

  cout << "===== EXECUTION =====" << endl;
  currentInstruction = 0;
  while ( currentInstruction < instructions.size() ){
    auto instruct = instructions[currentInstruction];
    //cout << currentInstruction << '\t' << nom(instruct.first) << "\t" << instruct.second << endl;
    
    switch(instruct.first){
      case '+':
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y+x);
        currentInstruction++;
      break;
    
      case '*':
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y*x);
        currentInstruction++;
      break;
    
      case NUMBER :
        pile.push_back(instruct.second);
        currentInstruction++;
      break;
    
      case JUMP :
        currentInstruction = instruct.second;
      break;
    
      case JUMP_IF_NOT_ZERO :
        currentInstruction = (depiler(pile) ? currentInstruction + 1 : instruct.second);
      break;

      case OUT :
        cout << "Résultat : " << depiler(pile) << endl;
        currentInstruction++;
      break;
    }
  }
  cout << "=====================" << endl;  
}


int main(int argc, char **argv) {
  yyin = mainContent(argc, argv);
  yyparse();
  print_program();
  run_program();
}
