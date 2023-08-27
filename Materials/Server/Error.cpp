//some messages appear using the full format:
// :Name COMMAND parameter list   

//ERR_NOSUCHSERVER   <server> parameter could not be found he server must not send any
                    //  other replies after this for that command.


//If the server encounters a fatal error while parsing a message,
//an error must be sent back to the client and the parsing terminated

//In the case of messages which use parameter lists using the comma 
//as an item separator, a reply must be sent for each item.