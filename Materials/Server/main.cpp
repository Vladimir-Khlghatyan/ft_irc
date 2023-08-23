#include "Server.hpp"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout<<"must by enter three arguments"<<std::endl;
        return 1;
    }
    try{
        Server s(argv[1], argv[2]);
        s.Chack();
        std::cout<<"congratulations server is up and running"<<std::endl;
        s.mainServer();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}