
#include "BigNumber.hpp"

BigNumber::BigNumber()
{
    isNegative = false;
    //Not much, we initialize both of the numbers to 0
    changeDecimalPart("0");
    changeIntPart("0");
}

BigNumber::BigNumber(std::string const& numberToTake)
{
    isNegative = false;
    lastDecimalPartPower = 0;

    //We first check if there is any non-numerical character
    if (!checkStringIntegrity(numberToTake))
    {
        std::cout << "Error from constructor : The number contains non-number characters !\n";
        changeDecimalPart("0");
        changeIntPart("0");
        return;
    }

    switch (std::count(numberToTake.begin(), numberToTake.end(), '.'))
    {
    case 0: //Only an integer !
        decimalPart.push_back(0);//Not needed, so we put it to 0

        try{
            changeIntPart(numberToTake);
        }catch (std::string const& ex){
            std::cout << "Error in constructor : " << ex << "\nGenerating an empty intPart\n";
            changeIntPart("0");
        }

        break;
    case 1: //Both
        std::string decimalString;
        std::string intString;

        std::size_t dotPosition = numberToTake.find(".");
        intString = numberToTake.substr(0, dotPosition);
        decimalString = numberToTake.substr(dotPosition + 1);

        try{
            changeIntPart(intString);
        }catch (std::string const& ex){
            std::cout << "Error in constructor : " << ex << "\nGenerating an empty intPart\n";
            changeIntPart("0");
        }

        try{
            changeDecimalPart(decimalString);
        }catch (std::string const& ex){
            std::cout << "Error in constructor : " << ex << "\nGenerating an empty decimalPart\n";
            changeDecimalPart("0");
        }

        break;
    default:
        std::cout << "Error from constructor : Multiple decimal points found (found "
                  << std::count(numberToTake.begin(), numberToTake.end(), '.') << ")\n";
        changeDecimalPart("0");
        changeIntPart("0");
        break;
    }
}

BigNumber::BigNumber(std::string&& numberToTake)
{
    isNegative = false;
    lastDecimalPartPower = 0;

    if (!checkStringIntegrity(numberToTake))
    {
        std::cout << "Error from constructor : The number contains non-number characters !\n";
        changeDecimalPart("0");
        changeIntPart("0");
        return;
    }

    switch (std::count(numberToTake.begin(), numberToTake.end(), '.'))
    {
    case 0:
        changeDecimalPart("0");

        try{
            changeIntPart(numberToTake);
        }catch (std::string const& ex){
            std::cout << "Error in constructor : " << ex << "\nGenerating an empty intPart\n";
            changeIntPart("0");
        }

        break;
    case 1:
        std::string decimalString;
        std::string intString;

        std::size_t dotPosition = numberToTake.find(".");
        intString = numberToTake.substr(0, dotPosition);
        decimalString = numberToTake.substr(dotPosition + 1);

        try{
            changeIntPart(intString);
        }catch (std::string const& ex){
            std::cout << "Error in constructor : " << ex << "\nGenerating an empty intPart\n";
            changeIntPart("0");
        }

        try{
            changeDecimalPart(decimalString);
        }catch (std::string const& ex){
            std::cout << "Error in constructor : " << ex << "\nGenerating an empty decimalPart\n";
            changeDecimalPart("0");
        }

        break;
    default:
        std::cout << "Error from constructor : Multiple decimal points found (found "
                  << std::count(numberToTake.begin(), numberToTake.end(), '.') << ")\n";
        changeDecimalPart("0");
        changeIntPart("0");
        break;
    }
}

void BigNumber::changeIntPart(std::string const& newIntPart)
{
    if (!checkStringIntegrity(newIntPart))
    {
        std::cout << "Error from changeIntPart : The number contains non-number characters !\n";
        throw "Non-numbers characters\n";
    }

    intPart.clear();

    std::string stringTreated;

    std::size_t last0Pos(0);

    //We must eliminate any zeros before the first digit
    for ( ; last0Pos < newIntPart.size() ; last0Pos++)
    {
        if (newIntPart[last0Pos] != '0')
            break;
    }

    stringTreated = newIntPart.substr(last0Pos);

    if (stringTreated.size() != 0)
    {
        numberOfIntegerDigits = stringTreated.size();

        std::stringstream takeChunks;

        const int rest = stringTreated.size() % MAX_DIGIT_NUMBER;
        const int numberOfIterations = (stringTreated.size() - (rest)) / MAX_DIGIT_NUMBER;
        int pos = (numberOfIterations - 1) * MAX_DIGIT_NUMBER + (rest);

        for ( ; rest <= pos ; pos-=MAX_DIGIT_NUMBER)
        {
            for (int j(0) ; j < MAX_DIGIT_NUMBER ; j++)
            {
                takeChunks << stringTreated[j + pos];
            }
            intPart.push_back(atoi(takeChunks.str().c_str()));
            takeChunks.str(std::string());
        }

        if (rest)
        {
            //Now we only have to add the rest
            for (int i(0) ; i < rest ; i++)
                takeChunks << stringTreated[i];

            intPart.push_back(atoi(takeChunks.str().c_str()));
            takeChunks.str(std::string());
        }
    }
    else
    {
        numberOfIntegerDigits = 1;
        intPart.push_back(0);
    }
}

void BigNumber::changeDecimalPart(std::string const& newDecimalPart)
{

    if (!checkStringIntegrity(newDecimalPart))
    {
        std::cout << "Error from changeDecimalPart : The number contains non-number characters !\n";
        throw "Non-numbers characters\n";
    }

    decimalPart.clear();

    //Now we must remove any superficial 0 at the end of the string
    std::size_t last0Pos(0);

    for (last0Pos = newDecimalPart.size() - 1 ; last0Pos >= 0 ; last0Pos--)
    {
        if (newDecimalPart[last0Pos] != '0')
            break;
    }

    std::string stringTreated = newDecimalPart.substr(0, last0Pos + 1);

    if (stringTreated.size() != 0)
    {
        numberOfDecimalDigits = stringTreated.size();

        std::stringstream takeChunks;

        const int rest = stringTreated.size() % MAX_DIGIT_NUMBER;
        const int numberOfIterations = (stringTreated.size() - (rest)) / MAX_DIGIT_NUMBER;

        for (int i(0) ; i < numberOfIterations ; i++)
        {
            for (int j(0) ; j < MAX_DIGIT_NUMBER ; j++)
            {
                takeChunks << stringTreated[i * MAX_DIGIT_NUMBER + j];
            }

            decimalPart.push_back(atoi(takeChunks.str().c_str()));
            takeChunks.str(std::string());
        }

        if (rest)
        {
            //And then the rest
            takeChunks.str(std::string());
            for (int i(0) ; i <= rest ; i++)
            {
                takeChunks << stringTreated[stringTreated.size() - rest + i];
            }
            decimalPart.push_back(atoi(takeChunks.str().c_str()));

            lastDecimalPartPower = 0;
            while (takeChunks.str()[lastDecimalPartPower] == '0')
                lastDecimalPartPower++;


        }
        else//For the sake of the last digit's number of zeros
        {
            std::string lastString = stringTreated.substr(stringTreated.size() - MAX_DIGIT_NUMBER, MAX_DIGIT_NUMBER);

            lastDecimalPartPower = 0;
            while (lastString[lastDecimalPartPower] == '0')
                lastDecimalPartPower++;
        }
    }
    else
    {
        numberOfDecimalDigits = 1;
        decimalPart.push_back(0);
        lastDecimalPartPower = 0;
    }
}

std::string BigNumber::getString(int digitNumber) const
{
    std::stringstream toReturn;
    toReturn << *this;

    if (digitNumber == -1)
        return toReturn.str();
    else
        return toReturn.str().substr(0, digitNumber);
}

short BigNumber::getPowerOfTenDigit(int powerOfTen) const
{
    short value(0);
    //Each is as follow : n,n-1...2,1,0(<- int decimal ->)-1,-2...
    if (powerOfTen < 0) //Is decimal
    {
        int digitSeeked = std::abs(powerOfTen) - 1;

        if (digitSeeked >= numberOfDecimalDigits) //Non-defined digits
            return 0;
        else
        {
            if (decimalPart.size() - 1 > (digitSeeked / MAX_DIGIT_NUMBER))
            {
                value = decimalPart[digitSeeked / MAX_DIGIT_NUMBER];
            }
            else
            {
                value = evaluateLastDecimal();
            }

            short bigPart = 0;
            short smallPart = 0;
            int tempPowerOfTen = 1;
            digitSeeked = MAX_DIGIT_NUMBER - (std::abs(powerOfTen) % MAX_DIGIT_NUMBER);

            for (int i(0); i < digitSeeked % MAX_DIGIT_NUMBER ; i++)
                tempPowerOfTen *= 10;

            smallPart = value % tempPowerOfTen;
            tempPowerOfTen *= 10;
            bigPart = value - (value % tempPowerOfTen);

            return ((value - smallPart - bigPart) / (tempPowerOfTen / 10));
        }
    }
    else //Is integer
    {
        if (powerOfTen >= numberOfIntegerDigits) //Non-defined digits
            return 0;
        else
        {
            value = intPart[powerOfTen / MAX_DIGIT_NUMBER];

            short bigPart = 0;
            short smallPart = 0;
            int tempPowerOfTen = 1;

            for (int i(0); i < powerOfTen % MAX_DIGIT_NUMBER ; i++)
                tempPowerOfTen *= 10;

            smallPart = value % tempPowerOfTen;
            tempPowerOfTen *= 10;
            bigPart = value - (value % tempPowerOfTen);

            return ((value - smallPart - bigPart) / (tempPowerOfTen / 10));
        }
    }

    return value;
}

void BigNumber::changeNumber(std::string newNumber)
{
    if (!checkStringIntegrity(newNumber))
    {
        //std::cout << "Error from changeNumber : The number contains non-number characters !\n";
        //std::cout << newNumber << '\n';
        return;
    }

    switch (std::count(newNumber.begin(), newNumber.end(), '.'))
    {
    case 0: //Only an integer !
        changeDecimalPart("0");//Not needed, so we put it to 0

        try{
            changeIntPart(newNumber);
        }catch (std::string const& ex){
            std::cout << "Error in changeNumber : " << ex << "\nGenerating an empty intPart\n";
        }

        break;
    case 1: //Both
        std::string decimalString;
        std::string intString;

        std::size_t dotPosition = newNumber.find(".");
        intString = newNumber.substr(0, dotPosition);
        decimalString = newNumber.substr(dotPosition + 1);

        try{
            changeIntPart(intString);
        }catch (std::string const& ex){
            std::cout << "Error in changeNumber : " << ex << "\nGenerating an empty intPart\n";
        }

        try{
            changeDecimalPart(decimalString);
        }catch (std::string const& ex){
            std::cout << "Error in changeNumber : " << ex << "\nGenerating an empty decimalPart\n";
        }

        break;
    default:
        std::cout << "Error from changeNumber : Multiple decimal points found (found "
                  << std::count(newNumber.begin(), newNumber.end(), '.') << ")\n";
        break;
    }
}

bool BigNumber::checkStringIntegrity(std::string const& toTest)
{
    for (auto it = toTest.begin() ; it != toTest.end() ; it++)
    {
        if (*it != '0' && *it != '1' && *it != '2' && *it != '3' &&
           *it != '4' && *it != '5' && *it != '6' && *it != '7' &&
           *it != '8' && *it != '9' && *it != '.')
        {
            return false;
        }
    }

    return true;
}

BigNumber::~BigNumber()
{

}

bool BigNumber::writeToFile(std::string filePath)
{
    std::ofstream writeFile;
    writeFile.open(filePath.c_str(), std::ios::app);

    if (!writeFile)
    {
        std::cout << "Error occurred during the file's opening (file name was (" << filePath << "). Failed to write the number\n";
        return false;
    }
    else
    {
        writeFile << *this << '\n';
    }

    return true;
}

bool BigNumber::loadFromFile(std::string filePath, unsigned int numberIndex)
{
    std::ifstream fileToRead;
    fileToRead.open(filePath.c_str());

    if (!fileToRead)
    {
        std::cout << "Error : Could not open file " << filePath << ". You should verify the path!\n";
        return false;
    }
    else
    {
        std::string readNumber;

        for (int i(0) ; i <= numberIndex ; i++)
        {
            if (fileToRead.eof())
            {
                break;
            }
            else
                std::getline(fileToRead, readNumber);
        }

        changeNumber(readNumber);
    }

    return true;
}

static int BigNumber::numberQuantityInFile(std::string filePath)
{
    int quantity(0);

    std::ifstream fileToRead;
    std::string trash;

    fileToRead.open(filePath.c_str());

    if (!fileToRead)
    {
        std::cout << "Error : Could not open file " << filePath << ". You should verify the path!\n";
        throw "File opening failed";
    }
    else
    {
        while (!fileToRead.eof())
        {
            std::getline(fileToRead, trash);
            quantity++;
        }
    }

    return quantity;
}

short BigNumber::evaluateLastDecimal() const
{
    short remainingSpace = MAX_DIGIT_NUMBER - lastDecimalPartPower - 1;

    short lastPart = decimalPart[decimalPart.size() - 1];

    if (lastPart > 9) remainingSpace--;
    if (lastPart > 99) remainingSpace--;
    if (lastPart > 999) remainingSpace--;

    for (int i(0) ; i < remainingSpace ; i++)
        lastPart *= 10;

    return lastPart;
}

std::ostream& operator<<(std::ostream& os, const BigNumber& toStream)
{
    os << *toStream.intPart.rbegin();
    //We print the first integer. We do that because it is the only one who cannot have any zeros in front of him, so
    //there is no need for some useless testing

    for (auto it = toStream.intPart.rbegin() + 1 ; it != toStream.intPart.rend() ; it++)//It is necessary to print the integer part backwards
    {
        //Now, what we'll need to do is determine how many zeros we will need to put before the integer part
        if (*it < 10)
        {
            os << "000" << *it;
        }
        else if (*it < 100)
        {
            os << "00" << *it;
        }
        else if (*it < 1000)
        {
            os << "0" << *it;
        }
        else
        {
            os << *it;
        }
    }

    os << '.';

    for (auto it = toStream.decimalPart.begin() ; it != toStream.decimalPart.end() ; it++)
    {
        if (it != toStream.decimalPart.end() - 1)
        {
            if (*it < 10)
            {
                os << "000" << *it;
            }
            else if (*it < 100)
            {
                os << "00" << *it;
            }
            else if (*it < 1000)
            {
                os << "0" << *it;
            }
            else
            {
                os << *it;
            }
        }
        else
        {
            for (int i(0); i < toStream.lastDecimalPartPower ; i++)
                os << '0';


            os << *it;
        }
    }

    return os;
}

bool operator<(BigNumber const& comp1, BigNumber const& comp2)
{
    //We first compare the size of the integers parts
    if (comp1.numberOfIntegerDigits < comp2.numberOfIntegerDigits)
    {
        //std::cout << "Less integers from first member\n";
        return true;
    }
    else if (comp1.numberOfIntegerDigits > comp2.numberOfIntegerDigits)
    {
        //std::cout << "Less integers from second member\n";
        return false;
    }
    else
    {
        //Then, we compare the actual integer parts
        for (int i(0) ; i < comp1.intPart.size() ; i++)
        {
            if (comp1.intPart[i] < comp2.intPart[i])
            {
                //std::cout << "Found a small int from first member\n";
                return true;
            }
            else if (comp1.intPart[i] > comp2.intPart[i])
            {
                //std::cout << "Found a small int from second member\n";
                return false;
            }
        }

        //If this didn't give any results, we pass on to the decimal part
        if (comp1.decimalPart.size() < comp2.decimalPart.size())
        {
            if (comp1.decimalPart.size() > 1)
            {
                for (int i(0);i<comp1.decimalPart.size() - 2;i++)
                {
                     if (comp1.decimalPart[i] < comp2.decimalPart[i])
                        return true;
                     else if (comp1.decimalPart[i] > comp2.decimalPart[i])
                        return false;
                }
            }

            short lastValue1 = comp1.evaluateLastDecimal();

            if (lastValue1 < comp2.decimalPart[comp1.decimalPart.size() - 1])
                return true;
            else if (lastValue1 > comp2.decimalPart[comp1.decimalPart.size() - 1])
                return false;
            else
                return true;
        }
        else if (comp1.decimalPart.size() > comp2.decimalPart.size())
        {
            if (comp2.decimalPart.size() > 1)
            {
                for (int i(0);i<comp2.decimalPart.size() - 2;i++)
                {
                     if (comp2.decimalPart[i] < comp1.decimalPart[i])
                        return false;
                     else if (comp2.decimalPart[i] > comp1.decimalPart[i])
                        return true;
                }
            }

            short lastValue2 = comp2.evaluateLastDecimal();

            if (lastValue2 < comp1.decimalPart[comp2.decimalPart.size() - 1])
                return false;
            else if (lastValue2 > comp1.decimalPart[comp2.decimalPart.size() - 1])
                return true;
            else
                return false;
        }
        else
        { //They both have the same size
            for (int i(0);i<comp1.decimalPart.size() - 1;i++)
            {
                 if (comp1.decimalPart[i] < comp2.decimalPart[i])
                    return true;
                 else if (comp1.decimalPart[i] > comp2.decimalPart[i])
                    return false;
            }

            if (comp1.evaluateLastDecimal() < comp2.evaluateLastDecimal())
                return true;
            else
                return false;
        }
    }

    return false;
}

bool operator==(BigNumber const& comp1, BigNumber const& comp2)
{
    if ((comp1.numberOfIntegerDigits != comp2.numberOfIntegerDigits) ||
        (comp1.numberOfDecimalDigits != comp2.numberOfDecimalDigits))
    {
        return false;
    }
    else
    {
        for (int i(0) ; i < comp1.intPart.size() ; i++)
            if (comp1.intPart[i] != comp2.intPart[i])
                return false;

        for (int i(0) ; i < comp1.decimalPart.size() ; i++)
            if (comp1.decimalPart[i] != comp2.decimalPart[i])
                return false;

        if (comp1.lastDecimalPartPower != comp2.lastDecimalPartPower)
            return false;
    }

    return true;
}

bool operator>(BigNumber const& comp1, BigNumber const& comp2)
{
    return !((comp1 < comp2) || (comp1 == comp2));
}

bool operator<=(BigNumber const& comp1, BigNumber const& comp2)
{
    return ((comp1 < comp2) || (comp1 == comp2));
}

bool operator>=(BigNumber const& comp1, BigNumber const& comp2)
{
    return !(comp1 < comp2);
}

bool operator!=(BigNumber const& comp1, BigNumber const& comp2)
{
    return !(comp1 == comp2);
}

BigNumber operator+(BigNumber const& comp1, BigNumber const& comp2)
{
    //std::cout << "Numbers in entry are " << comp1.getString(20) << "&" << comp2.getString(20) << '\n';

    BigNumber finalNumber;

    short remainingDecimalValue(0);

    //We first evaluate the decimal part
    if (comp1.decimalPart.size() < comp2.decimalPart.size())
    {
        finalNumber.decimalPart = comp2.decimalPart;
        finalNumber.lastDecimalPartPower = comp2.lastDecimalPartPower;

        for (int i(comp1.decimalPart.size() - 1) ; i >= 0 ; i--)//We live room for 0
        {
            if (i == comp1.decimalPart.size() - 1)
                finalNumber.decimalPart[i] += comp1.evaluateLastDecimal();
            else
                finalNumber.decimalPart[i] += comp1.decimalPart[i];

            if (finalNumber.decimalPart[i] > 9999)
            {
                if (i == 0)
                    remainingDecimalValue += (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000))/ 10000;
                else
                    finalNumber.decimalPart[i - 1] += (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000))/ 10000;

                finalNumber.decimalPart[i] -= (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000));
            }
        }
    }
    else if (comp1.decimalPart.size() > comp2.decimalPart.size())
    {
        finalNumber.decimalPart = comp1.decimalPart;
        finalNumber.lastDecimalPartPower = comp1.lastDecimalPartPower;

        for (int i(comp2.decimalPart.size() - 1) ; i >= 0 ; i--)//We live room for 0
        {
            if (i == comp2.decimalPart.size() - 1)
                finalNumber.decimalPart[i] += comp2.evaluateLastDecimal();
            else
                finalNumber.decimalPart[i] += comp2.decimalPart[i];

            if (finalNumber.decimalPart[i] > 9999)
            {
                if (i == 0)
                    remainingDecimalValue += (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000))/ 10000;
                else
                    finalNumber.decimalPart[i - 1] += (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000))/ 10000;

                finalNumber.decimalPart[i] -= (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000));
            }
        }
    }
    else //Both have the same size
    {
        finalNumber.decimalPart = comp1.decimalPart;
        short lastValue = 0;

        for (int i(comp2.decimalPart.size() - 1) ; i >= 0 ; i--)//We live room for 0
        {
            if (i == comp2.decimalPart.size() - 1)
            {
                lastValue = comp1.evaluateLastDecimal() + comp2.evaluateLastDecimal();

                if (lastValue > 9999)
                {
                    finalNumber.lastDecimalPartPower = MAX_DIGIT_NUMBER - std::ceil(std::log10(lastValue - 10000));
                }

                finalNumber.decimalPart[comp2.decimalPart.size() - 1] = lastValue;
            }
            else
                finalNumber.decimalPart[i] += comp2.decimalPart[i];

            if (finalNumber.decimalPart[i] > 9999)
            {
                if (i == 0)
                    remainingDecimalValue += (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000))/ 10000;
                else
                    finalNumber.decimalPart[i - 1] += (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000))/ 10000;

                finalNumber.decimalPart[i] -= (finalNumber.decimalPart[i] - (finalNumber.decimalPart[i] % 10000));
            }
        }
    }

    if (comp1.intPart.size() < comp2.intPart.size())
    {
        finalNumber.intPart = comp2.intPart;
        finalNumber.intPart[0] += remainingDecimalValue;

        const int initialSize = comp1.intPart.size();

        for (int i(0) ; i < initialSize ; i++)
        {
            finalNumber.intPart[i] += comp1.intPart[i];

            if (finalNumber.intPart[i] > 9999)
            {
                if (i < initialSize - 1)
                {
                    finalNumber.intPart[i + 1] += (finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000))/ 10000;
                    finalNumber.intPart[i] -= (finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000));
                }
                else
                {
                    finalNumber.intPart.push_back((finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000))/ 10000);
                    finalNumber.intPart[i] -= (finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000));
                }
            }
        }
    }
    else
    {
        finalNumber.intPart = comp1.intPart;
        finalNumber.intPart[0] += remainingDecimalValue;

        const int initialSize = comp2.intPart.size();

        for (int i(0) ; i < initialSize ; i++)
        {
            finalNumber.intPart[i] += comp2.intPart[i];

            if (finalNumber.intPart[i] > 9999)
            {
                if (i < initialSize - 1)
                {
                    finalNumber.intPart[i + 1] += (finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000))/ 10000;
                    finalNumber.intPart[i] -= (finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000));
                }
                else
                {
                    finalNumber.intPart.push_back((finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000))/ 10000);
                    finalNumber.intPart[i] -= (finalNumber.intPart[i] - (finalNumber.intPart[i] % 10000));
                }
            }
        }
    }

    return finalNumber;
}

BigNumber operator-(BigNumber const& comp1, BigNumber const& comp2)
{

}
