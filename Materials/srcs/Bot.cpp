#include "Bot.hpp"

Bot::Bot()
{
    DEBUGGER();
    _fileData.open("./Bonus/Bot");
    if (!_fileData.is_open())
    {
        throw Server::Excp("Unable to open file");
    }

    DEBUGGER();
    redeForMap();
    DEBUGGER();
}

void Bot::Fetch(std::vector<std::string> arg,std::string &message)
{
    DEBUGGER();
    if (!_replay.size())
        return ;
    DEBUGGER();
    message = "i am Bot i don't understand what you say";
    DEBUGGER();
    std::map<std::string, std::string>::iterator it = _replay.begin();
DEBUGGER();
    for( ; it != _replay.end(); ++it)
    {
        if (it->first == arg[2])
        {
            message = it->second;
            DEBUGGER();
            return ;
        }
    }
    DEBUGGER();
}


void Bot::redeForMap(void)
{
    std::string strData, str[2];
DEBUGGER();
    while (getline(_fileData, strData))
    {
        DEBUGGER();
        std::stringstream ss(strData);
        if (strData.find(',') != std::string::npos)
        {
            DEBUGGER();
            getline(ss, str[0], ',');                   // key  
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