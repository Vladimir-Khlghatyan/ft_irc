#include "Server.hpp"
#include "../Debugger/debugger.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "usage: ./ft_irc [port] [password]" << std::endl;
        return 1;
    }

    // if (!argsAreValid(argv[1], argv[2])) // test
    // {
    //     return 2;
    // }
    try{
        Server s(argv[1], argv[2]);
        s.start();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl; // std::flush;
    }
    return 0;
}