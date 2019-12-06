
===== EXECUTION =====
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"peut faire des choses avant la fonction\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,1);
addInstruct(command::_EMPILE_VALUE_,-1);
addInstruct(command::_EMPILE_VALUE_,8);
addInstruct(command::_CREATE_FUNCTION_,"test");
addInstruct(command::_GOTO_,13);
addInstruct(command::_ENTER_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"fonction de confirmation\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,4);
addInstruct(command::_EXIT_FUNCTION_);
addInstruct(command::_EMPILE_VALUE_,"creation ok\n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,-1);
addInstruct(command::_CALL_FUNCTION_,"test");
addInstruct(command::_EMPILE_VALUE_,"utilisation ok \n");
addInstruct(command::_WRITE_);
addInstruct(command::_EMPILE_VALUE_,"test");
addInstruct(command::_WRITE_);
addInstruct(command::_EXIT_FUNCTION_);

=====================
