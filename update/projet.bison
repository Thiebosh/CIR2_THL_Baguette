%{
  #include <iostream>
  #include <map>
  #include <string>
  #include <vector>  
  using namespace std;
  extern FILE *yyin;
  extern int yylex ();
  int yyerror(char *s) { printf("%s\n", s); }

  map<string, double> variables;

  vector<pair<int,double>> instructions;
  int ic = 0;   // compteur instruction 
  inline ins(int c, double d) { instructions.push_back(make_pair(c, d)); ic++;};

  // structure pour stocker les adresses pour les sauts condistionnels et autres...
  typedef struct adr {
    int ic_goto; 
    int ic_false;
  } t_adresse; 

%}

%union
{
  double valeur;
  char nom[50];
  t_adresse adresse;
}

%token <valeur> NUMBER
%token <nom> IDENTIFIER
%type <valeur> expression
%token <adresse> SI
%token ALORS
%token SINON
%token FINSI
%token REPEAT
%token JMP 
%token JNZ
%token OUT


%left '+' '-'     /* associativité à gauche */
%left '*' '/'     /* associativité à gauche */

%%
bloc : bloc instruction '\n' 
      |    /* Epsilon */
      ;
instruction : expression { ins (OUT,0);   /* imprimer le résultat de l'expression */  }

            | SI expression '\n' { $1.ic_goto = ic;  
                                   ins (JNZ,0);    }
              ALORS '\n' bloc    { $1.ic_false = ic;
                                   ins (JMP,0);
                                   instructions[$1.ic_goto].second = ic;  
                                  }
              SINON '\n' bloc    { instructions[$1.ic_false].second = ic; }
              FINSI              {   }

            | REPEAT '(' expression ')' expression { /* TO DO */ }

            | IDENTIFIER '=' expression  { variables[$1] = $3;
                         cout << "Affectation de " << $3 << " à " << $1 << endl;}
            |   /* Ligne vide*/
            ;
expression: expression '+' expression     { ins('+', 0);}
          | expression '-' expression     { ins('-', 0);}
          | expression '*' expression     { ins('*', 0);}
          | expression '/' expression     { ins('/', 0);}
          | '(' expression ')'            { }
          | NUMBER                        { ins(NUMBER, $1);}
          | IDENTIFIER                    { ins(IDENTIFIER, variables[$1]);}
          ;
%%


// Pour imprimer le code généré de manière plus lisible 
string nom(int instruction){
  switch (instruction){
   case '+'     : return "ADD";
   case '*'     : return "MUL";
   case NUMBER  : return "NUM";
   case OUT     : return "OUT";
   case JNZ     : return "JNZ";   // Jump if not zero
   case JMP     : return "JMP";   // Unconditional Jump
   default  : return to_string (instruction);
   }
}

void print_program(){
  cout << "==== CODE GENERE ====" << endl;
  int i = 0;
  for (auto ins : instructions )
    cout << i++ << '\t' << nom(ins.first) << "\t" << ins.second << endl;
  cout << "=====================" << endl;  
}

double depiler(vector<double> &pile) {
  double t = pile[pile.size()-1];
  //cout << "Dépiler " << t << endl; 
  pile.pop_back();
  return t;
}

void run_program(){
  vector<double> pile; 
  double x,y;

  cout << "===== EXECUTION =====" << endl;
  ic = 0;
  while ( ic < instructions.size() ){
    auto ins = instructions[ic];
    //cout << ic << '\t' << nom(ins.first) << "\t" << ins.second << endl;
    
    switch(ins.first){
      case '+':
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y+x);
        ic++;
      break;
    
      case '*':
        x = depiler(pile);
        y = depiler(pile);
        pile.push_back(y*x);
        ic++;
      break;
    
      case NUMBER :
        pile.push_back(ins.second);
        ic++;
      break;
    
      case JMP :
        ic = ins.second;
      break;
    
      case JNZ :
        x = depiler(pile);
        ic = ( x ? ic + 1 : ins.second);
      break;

      case OUT :
        cout << "Résultat : " << depiler(pile) << endl;
        ic++;
      break;
    }
  }
  cout << "=====================" << endl;  
}

int main(int argc, char **argv) {
  
  if ( argc > 1 )
    yyin = fopen( argv[1], "r" );
  else
    yyin = stdin;
  yyparse();

  print_program();

  run_program();

}