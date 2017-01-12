
#include "main.hpp"

int main()
{
     BigNumber myNumber("0.123456789");
    BigNumber myNumber1("0.123456789");

    myNumber1.loadFromFile("1.txt");
    myNumber.loadFromFile("2.txt");

    if (myNumber >= myNumber1)
        std::cout << "Result : true\n";
    else
        std::cout << "Result : false\n";

    return 0;
}
