
===== EXECUTION =====
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,3.500000);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_CREATE_VARIABLE_,"maVar");
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
addInstruct(command::_EMPILE_VALUE_,"\n");
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
addInstruct(command::_EMPILE_VALUE_,")\n\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EXIT_FUNCTION_);

=====================
