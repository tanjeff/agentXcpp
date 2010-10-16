#include <iostream>
#include "oid.h"


using namespace agentx;

int main()
{

    oid o(1,2,6,20655,213);
    std::cout << o << std::endl;
    
    oid p;
    std::cout << p << std::endl;

    p=o;
    std::cout << p << std::endl;
    


    return 0;
}
