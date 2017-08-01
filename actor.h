#ifndef ACTOR
#define ACTOR

#include <iostream>

class Actor
{
    public:
        Actor();
        ~Actor();

    private:
        std::string name;
        std::string password;
};

#endif
