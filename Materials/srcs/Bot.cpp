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


Bot::~Bot()
{
    _fileData.close();
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
    if ("Time" == message.substr(message.find(' ') + 1))
    {
        message = getCurrentTime();
        return ;
    }
    std::map<std::string, std::string>::iterator it = _replay.begin();

    for( ; it != _replay.end(); ++it)
    {
        if (it->first == message.substr(message.find(' ') + 1))
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
            getline(ss, str[1], '\0');                  // value 
            if (!str[0].empty() && !str[1].empty())
            {
                _replay[str[0]] = str[1];
            }
        }
    }
}


std::string Bot::getCurrentTime(void)
{
    time_t currentTime;
    struct tm *localTime;
    char timeString[100];

    // Get the current time
    time(&currentTime);
    localTime = localtime(&currentTime);

    // Format the time as a string
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", localTime);

    return std::string(timeString);
}
