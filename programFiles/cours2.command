
===== EXECUTION =====
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_ENTER_BLOCK_);
addInstruct(command::_EMPILE_VALUE_,3);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_CREATE_VARIABLE_,"mavar");
addInstruct(command::_EMPILE_VARIABLE_,"mavar");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EXIT_BLOCK_);
addInstruct(command::_EMPILE_VALUE_,"on s'attend à ce que le programme plante car il ne connait plus la variable : \n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VARIABLE_,"mavar");
addInstruct(command::_WRITE_);
addInstruct(command::_EXIT_FUNCTION_);

=====================
