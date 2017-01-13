#ifndef BIGNUMBER_HPP_INCLUDED
#define BIGNUMBER_HPP_INCLUDED

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <cmath>

#define MAX_DIGIT_NUMBER 4//Defines the maximum number of digits of one space of the vector

class BigNumber{
public:
    BigNumber();//The number will be declared empty (equal to 0)
    BigNumber(std::string const& numberToTake);//From a string that the constructor will cut down to numbers
    BigNumber(std::string&& numberToTake);//The code for this one and the above are the same, one just uses rvalues
    ~BigNumber();

    bool writeToFile(std::string filePath);//For testing shenanigans
    bool loadFromFile(std::string filePath, unsigned int numberIndex = 0);

    static int numberQuantityInFile(std::string filePath);//Careful : currently a crappy function only counting the number of lines in a program (supposed to be similar, but what if there are parasites ?)


    void changeIntPart(std::string const& newIntPart);
    void changeDecimalPart(std::string const& newDecimalPart);

    std::string getString(int digitNumber = -1) const;
    short getPowerOfTenDigit(int powerOfTen) const;//Returns

    //The code is very similar to the one of the constructor, however in case of
    //error, the BigNumber isn't changed
    void changeNumber(std::string newNumber);

    bool checkStringIntegrity(std::string const& toTest);

    template<typename A>
    A evaluateBigNumber()//use this template function to have a rough evaluation of the number, using usual types. Be careful of an information loss from output (example : no decimal part with integer evaluations)
    {
        A intPartHolder(0), decimalPartHolder(0);
        A tempValue(0);

        int powerOfTenDivide(1);

        for (int i(0);i<MAX_DIGIT_NUMBER;i++)
            powerOfTenDivide*=10;

        for (int i(0) ; i < intPart.size() ; i++)
        {
            tempValue = intPart[i];

            for (int j(0) ; j < i ; j++)
                tempValue *= powerOfTenDivide;

            intPartHolder += tempValue;
        }

        for (int i(0) ; i < decimalPart.size() - 1 ; i++)
        {
            tempValue = decimalPart[i];

            for (int j(0) ; j <= i ; j++)
                tempValue /= powerOfTenDivide;

            decimalPartHolder += tempValue;
        }

        tempValue = evaluateLastDecimal();

        for (int i(0) ; i < decimalPart.size() ; i++)
            tempValue /= powerOfTenDivide;

        decimalPartHolder += tempValue;

        return (intPartHolder + decimalPartHolder);
    };

    short evaluateLastDecimal() const;

    template<typename A, typename B>
    friend bool operator<(A const& t1, B const& t2)
    {
        BigNumber first, second;
        std::stringstream convertToString;

        convertToString << t1;
        first.changeNumber(convertToString.str());

        convertToString.str(std::string());

        convertToString << t2;
        second.changeNumber(convertToString.str());

        return (first < second);
    };
    template<typename A, typename B>
    friend bool operator==(A const& t1, B const& t2)
    {
        BigNumber first, second;
        std::stringstream convertToString;

        convertToString << t1;
        first.changeNumber(convertToString.str());

        convertToString.str(std::string());

        convertToString << t2;
        second.changeNumber(convertToString.str());

        return (first == second);
    };
    template<typename A, typename B>
    friend bool operator>(A const& t1, B const& t2)
    {
        return !((t1 < t2) || (t1 == t2));
    };
    template<typename A, typename B>
    friend bool operator<=(A const& t1, B const& t2)
    {
        return ((t1 < t2) || (t1 == t2));
    };
    template<typename A, typename B>
    friend bool operator>=(A const& t1, B const& t2)
    {
        return !(t1 < t2);
    };
    template<typename A, typename B>
    friend bool operator!=(A const& t1, B const& t2)
    {
        return !(t1 == t2);
    };

    friend bool operator<(BigNumber const& comp1, BigNumber const& comp2);
    friend bool operator==(BigNumber const& comp1, BigNumber const& comp2);
    friend bool operator>(BigNumber const& comp1, BigNumber const& comp2);
    friend bool operator<=(BigNumber const& comp1, BigNumber const& comp2);
    friend bool operator>=(BigNumber const& comp1, BigNumber const& comp2);
    friend bool operator!=(BigNumber const& comp1, BigNumber const& comp2);
    friend BigNumber operator+(BigNumber const& comp1, BigNumber const& comp2);
    friend std::ostream& operator<<(std::ostream& os, const BigNumber& toStream);
private:
    /**Each short contains a value ranging from 0 to 9999.
    If the value is superior to 9999, then we add the superficial value the next
    biggest number in the vector

    The two vectors grow from a logic relative to their position from the dot :
    intPart <-- . --> decimalPart
    Which means, that the more intPart grows, the bigger the numbers contained are,
    and the the more decimalPart, the smaller the numbers contained are
    Example : number os 94616874.120505166
    Let's cut it 9461|6874.1205|0516|6
                   1    0    0    1   2

    */
    std::vector<short> decimalPart;
    short lastDecimalPartPower;//Used to determine number of zero before the last part. Huge garbage, needs to be deprecated asap
    std::vector<short> intPart;

    long numberOfIntegerDigits;
    long numberOfDecimalDigits;
};

#endif // BIGNUMBER_HPP_INCLUDED
