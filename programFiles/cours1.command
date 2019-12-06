
===== EXECUTION =====
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"PARTIE 1\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1.230000);
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"hello world");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_EMPILE_VALUE_,6);
addInstruct(command::_EMPILE_VALUE_,4);
addInstruct(command::_FOIS_);
addInstruct(command::_PLUS_);
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,4);
addInstruct(command::_EMPILE_VALUE_,6);
addInstruct(command::_FOIS_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_PLUS_);
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,-1);
addInstruct(command::_EMPILE_VALUE_,2);
addInstruct(command::_EMPILE_VALUE_,5);
addInstruct(command::_PLUS_);
addInstruct(command::_FOIS_);
addInstruct(command::_EMPILE_VALUE_,3);
addInstruct(command::_DIVISE_PAR_);
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
\
\
");
addInstruct(command::_WRITE_);
addInstruct(command::_STOP_);
addInstruct(command::_EMPILE_VALUE_,"PARTIE 2\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,2);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_CREATE_VARIABLE_,"maVar");
addInstruct(command::_EMPILE_VALUE_,"j'initialise maVar à ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"maVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
entrez une valeur : ");
addInstruct(command::_WRITE_);
addInstruct(command::_READ_,"maVar");
addInstruct(command::_EMPILE_VALUE_,"Nouvelle valeur de maVar : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"maVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,3.500000);
addInstruct(command::_UPDATE_VARIABLE_,"maVar");
addInstruct(command::_EMPILE_VALUE_,"j'affecte 3.5 à maVar entière (cast) : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"maVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"j'incrémente : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_PLUS_CREMENT_,"maVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"je décrémente : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_MOINS_CREMENT_,"maVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,3);
addInstruct(command::_FOIS_CREMENT_,"maVar");
addInstruct(command::_EMPILE_VALUE_,"je multiplie maVar par 3 et lui affecte directement : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"maVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"maVar");
addInstruct(command::_EMPILE_VALUE_,2);
addInstruct(command::_DIVISE_PAR_);
addInstruct(command::_EMPILE_VALUE_,12);
addInstruct(command::_MOINS_);
addInstruct(command::_EMPILE_VALUE_,1.000000);
addInstruct(command::_CREATE_VARIABLE_,"autreVar");
addInstruct(command::_EMPILE_VALUE_,"j'affecte une opération complexe à une variable réelle 'autreVar' : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"autreVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,5);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_PLUS_CREMENT_,"autreVar");
addInstruct(command::_PLUS_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_CREATE_VARIABLE_,"calcul");
addInstruct(command::_EMPILE_VALUE_,"j'incrémente autreVar, je lui ajoute 5 et je caste le résultat : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"calcul");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_," (autreVar : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"autreVar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,")\
\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"hello");
addInstruct(command::_EMPILE_VALUE_,"");
addInstruct(command::_CREATE_VARIABLE_,"placeAuxStrings");
addInstruct(command::_EMPILE_VALUE_," world");
addInstruct(command::_PLUS_CREMENT_,"placeAuxStrings");
addInstruct(command::_EMPILE_VALUE_,"je concatène des strings : ");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"placeAuxStrings");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\
");
addInstruct(command::_WRITE_);
addInstruct(command::_EXIT_FUNCTION_);

=====================
