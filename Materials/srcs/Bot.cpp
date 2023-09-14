#include "Bot.hpp"

Bot::Bot()
{
    DEBUGGER();
    _fileData.open("./BotSource/Bot");
    if (!_fileData.is_open())
    {
        throw Server::Excp("Unable to open file");
    }

    DEBUGGER();
    readFromMap();
    DEBUGGER();
}

void Bot::Fetch(std::string &message)
{
    std::string reply = "i am Bot i don't understand what you say";
DEBUGGER();
    if (!_replay.size())
        return ;

    if (message.find(' ') == std::string::npos)
    {
        message = reply;
        return ;
    }
DEBUGGER();
    std::map<std::string, std::string>::iterator it = _replay.begin();

    for( ; it != _replay.end(); ++it)
    {
        if (it->first == message.substr(message.find(' ') +1))
        {
            message = it->second;
            DEBUGGER();
            return ;
        }
    }
    message = reply;
    DEBUGGER();
}

void Bot::readFromMap(void)
{
    std::string strData, str[2];
DEBUGGER();
    while (getline(_fileData, strData))
    {
        DEBUGGER();
        std::stringstream ss(strData);
        if (strData.find(':') != std::string::npos)
        {
            DEBUGGER();
            getline(ss, str[0], ':');                   // key  
            getline(ss, str[1], '\0');                  // va   
            if (!str[0].empty() && !str[1].empty())
            {
                _replay[str[0]] = str[1];
            }
        }
    }
}

Bot::~Bot()
{
    _fileData.close();
}