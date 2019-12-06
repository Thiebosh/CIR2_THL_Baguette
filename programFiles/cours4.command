
===== EXECUTION =====
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"peut faire des choses sans la fonction\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_EMPILE_VALUE_,-1);
addInstruct(command::_EMPILE_VALUE_,7);
addInstruct(command::_CREATE_FUNCTION_,"test");
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"fonction de confirmation\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,4);
addInstruct(command::_EXIT_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"creation ok\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,-1);
addInstruct(command::_CALL_FUNCTION_,"test");
addInstruct(command::_EMPILE_VALUE_,"utilisation ok\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EXIT_FUNCTION_);

=====================
