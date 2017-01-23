
#include "Graphics.hpp"

bool displayPoints;
bool displayLines;

using boost::multiprecision::cpp_dec_float_50;

float HORIZONTAL_AXIS_SIZE(50);
float SPEED(1);//Number of pixels passed every time a key is pressed

bool launch()
{
    displayLines = true;
    displayPoints = true;

    sf::RenderWindow myWindow(sf::VideoMode(800, 640), "Draw big numbers", sf::Style::Fullscreen);
    sf::Event event;
    sf::Clock imGuiClock;

    std::vector<number<gmp_float<0>>> mySet;
    std::string currentDirectory;

    float currentPowerOfTen(0);

    char newDirectory[256] = "";

    ImGui::SFML::Init(myWindow);

    myWindow.resetGLStates();

    std::stringstream interfaceString;

    sf::Clock keyWait;
    keyWait.restart();

    bool stop(false);

    number<gmp_float<20000>> posX(0);
    number<gmp_float<20000>> posY(0);

    int numberToReach(0);
    int intervalBoundaries[2] = {0,0};

    number<gmp_float<0>> powerOfTen(10);

    do
    {
        while (myWindow.pollEvent(event))
        {
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                currentPowerOfTen += -1 * event.mouseWheelScroll.delta;
            }


            ImGui::SFML::ProcessEvent(event);
        }

        powerOfTen.precision(std::abs(static_cast<int>(currentPowerOfTen)));

        if (keyWait.getElapsedTime().asSeconds() >= 0.01)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                posX -= SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                posX += SPEED;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                posY -= SPEED * boost::multiprecision::pow(powerOfTen, currentPowerOfTen);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                posY += SPEED * boost::multiprecision::pow(powerOfTen, currentPowerOfTen);

            keyWait.restart();
        }

        ImGui::SFML::Update(myWindow, imGuiClock.restart());

        ImGui::Begin("Parameters");

            interfaceString.str(std::string());
            interfaceString << "Current number of points : " << mySet.size();
            ImGui::Text(interfaceString.str().c_str());

            interfaceString.str(std::string());
            interfaceString << "Current directory : '" << currentDirectory << "'\n";
            ImGui::Text(interfaceString.str().c_str());

            ImGui::InputText("New directory", newDirectory, 256);

            if (ImGui::Button("Load new directory"))
            {
                if (loadNewSet(mySet, newDirectory))
                {
                    //We change the directory's name
                    currentDirectory = newDirectory;

                    //We then adapt the zoom to the new set
                    if (mySet[0] != mySet[mySet.size() - 1])
                    currentPowerOfTen = findScale(mySet[0], mySet[mySet.size() - 1], myWindow.getSize().y);

                    HORIZONTAL_AXIS_SIZE = (float)myWindow.getSize().x / mySet.size();
                    posX = ((intervalBoundaries[0]+1) * HORIZONTAL_AXIS_SIZE);
                    posY = mySet[intervalBoundaries[0]];
                }

            }

            interfaceString.str(std::string());
            interfaceString << "Current coordinates :("<< posX << ';' << posY << ")\n";
            ImGui::Text(interfaceString.str().c_str());

            ImGui::InputFloat("Zoom factor", &currentPowerOfTen);
            if (currentPowerOfTen > INT_MAX/10)
                currentPowerOfTen = INT_MAX/10;

            if (currentPowerOfTen < INT_MIN/10)
                currentPowerOfTen = INT_MIN/10;

            ImGui::InputFloat("Space between points x-axis", &HORIZONTAL_AXIS_SIZE);
            if (HORIZONTAL_AXIS_SIZE <= 0)
                HORIZONTAL_AXIS_SIZE = 0.1;

            ImGui::InputFloat("Screen speed", &SPEED);

            ImGui::Checkbox("Display points", &displayPoints);
            ImGui::Checkbox("Display lines", &displayLines);

            if (mySet.size())
            {
                ImGui::InputInt("Number to reach", &numberToReach);
                if (numberToReach < 0)
                    numberToReach = 0;
                else if (numberToReach >= mySet.size())
                    numberToReach = mySet.size() - 1;

                if (ImGui::Button("Reach number"))
                {
                    posX = ((numberToReach+1) * HORIZONTAL_AXIS_SIZE) - static_cast<int>(myWindow.getSize().x) / 2;
                    posY = mySet[numberToReach];
                }
            }

            if (mySet.size() > 1)
            {
                ImGui::InputInt2("Define interval boundaries", intervalBoundaries);
                if (intervalBoundaries[1] <= intervalBoundaries[0])
                {
                    intervalBoundaries[1] = intervalBoundaries[0] - 1;
                }
                else if (intervalBoundaries[1] >= mySet.size() - 1)
                {
                    intervalBoundaries[1] = mySet.size() - 1;
                }

                if (intervalBoundaries[0] >= intervalBoundaries[1])
                {
                    intervalBoundaries[0] = intervalBoundaries[1] - 1;
                }
                else if (intervalBoundaries[0] < 0)
                {
                    intervalBoundaries[0] = 0;
                }
            }

            if (ImGui::Button("Adapt zoom to interval"))
            {
                if (mySet[intervalBoundaries[0]] != mySet[intervalBoundaries[1]])
                    currentPowerOfTen = findScale(mySet[intervalBoundaries[0]], mySet[intervalBoundaries[1]], myWindow.getSize().y);

                HORIZONTAL_AXIS_SIZE = (float)myWindow.getSize().x / mySet.size();
                posX = ((intervalBoundaries[0]+1) * HORIZONTAL_AXIS_SIZE);
                posY = mySet[intervalBoundaries[0]];
            }

            if (ImGui::Button("Write sorted array to a debug.txt"))
            {
                writeSortedArray(mySet, "debug.txt");
            }

            if (ImGui::Button("Stop program"))
            {
                stop = true;
            }
        ImGui::End();

        myWindow.clear(sf::Color(255, 255, 255));


            drawNumbers(mySet, myWindow, currentPowerOfTen, posX, posY);


            ImGui::Render();

        myWindow.display();

    }while (!stop);

    ImGui::SFML::Shutdown();

    return true;
}

float findScale(number<gmp_float<0>>& nMin, number<gmp_float<0>>& nMax, float windowHeight)
{
    unsigned int biggestPrecision(0);

    number<gmp_float<0>> windowHeightPrecision(0);
    number<gmp_float<0>> difference(0);

    if (nMin.precision() > nMax.precision())
        biggestPrecision= nMin.precision();
    else
        biggestPrecision= nMax.precision();


    windowHeightPrecision = windowHeight;

    return (boost::multiprecision::log((nMax - nMin) / windowHeightPrecision) / boost::multiprecision::log( number<gmp_float<0>>(10) ) ).convert_to<float>();
}

bool loadNewSet(std::vector<number<gmp_float<0>>>& allNumbers, std::string const& newFilePath)
{
    int newSetSize(0);

    try {
        newSetSize = BigNumber::numberQuantityInFile(newFilePath);
    } catch (const char* e) {
        std::cout << "Error while loading new set : " << e << '\n';
        return false;//The set is left unchanged
    }

    std::ifstream fileToRead;
    std::string tempRead;
    std::stringstream toConvert;
    number<gmp_float<0>> newNumber;
    number<gmp_float<0>> dummy;

    fileToRead.open(newFilePath, std::ios_base::in);

    if (fileToRead)
    {
        allNumbers.clear();

        for (int i(0) ; i < newSetSize ; i++)
        {
            tempRead = "";
            getline(fileToRead, tempRead);

            toConvert.str(std::string(""));
            toConvert.clear();
            toConvert << tempRead;
            newNumber.precision(tempRead.size());
            toConvert >> std::setprecision(std::numeric_limits<decltype(newNumber)>::max_digits10);
            toConvert >> newNumber;

            allNumbers.push_back(newNumber);
            allNumbers[i].precision(newNumber.precision());
            allNumbers[i] = newNumber;
        }

        std::sort(allNumbers.begin(), allNumbers.end());//We then sort the array

        return true;
    }
    else
    {
        std::cout << "Could not load new array from file '" << newFilePath << "'.\n";
        return false;
    }
}

bool writeSortedArray(std::vector<number<gmp_float<0>>> const& allNumbers, std::string filePath)
{
    std::ofstream fileToStream;
    fileToStream.open(filePath.c_str(), std::ofstream::out | std::ofstream::trunc);

    if (!fileToStream)
    {
        std::cout << "Couldn't write in debug file : file opening failed (file path : '" << filePath << "'.\n";
        return false;
    }
    else
    {
        for (int i(0); i < allNumbers.size() ; i++)
        {
            fileToStream << std::setprecision(std::numeric_limits<decltype(allNumbers[i])>::digits10) << allNumbers[i] << '\n';
        }
    }
}

void drawNumbers(std::vector<number<gmp_float<0>>> const& allNumbers, sf::RenderWindow const& targetWindow,
                 float powerOfTenZoom, number<gmp_float<0>> posX, number<gmp_float<0>> posY)
{
    sf::Vector2f position;

    sf::RectangleShape axis;
    axis.setFillColor(sf::Color(0,0,0));

    //x-axis
    axis.setSize(sf::Vector2f(AXIS_WIDTH, targetWindow.getSize().y));
    position.x = -posX.convert_to<float>() - (AXIS_WIDTH/2);
    if (position.x >= -(3/2*AXIS_WIDTH) && position.x <= targetWindow.getSize().x - (AXIS_WIDTH/2))
    {
        position.y = 0;
        axis.setPosition(position);
        targetWindow.draw(axis);
    }

    if (allNumbers.size() > 0)
    {
        sf::CircleShape point;
        point.setRadius(5);
        point.setFillColor(sf::Color(255,0,0));

        unsigned int biggestPrecision(0);

        for (int i(0) ; i < allNumbers.size() ; i++)
        {
            if (allNumbers[i].precision() > biggestPrecision)
                biggestPrecision = allNumbers[i].precision();
        }

        number<gmp_float<0>> oneGrad(0);
        number<gmp_float<0>> minScreenY(0);
        number<gmp_float<0>> maxScreenY(0);
        oneGrad.precision(biggestPrecision);
        minScreenY.precision(biggestPrecision);
        maxScreenY.precision(biggestPrecision);

        number<gmp_float<0>> powerOfTen(10);
        powerOfTen.precision(std::abs(powerOfTenZoom));

        oneGrad = boost::multiprecision::pow(powerOfTen, powerOfTenZoom);

        minScreenY = posY;
        maxScreenY = minScreenY + oneGrad * targetWindow.getSize().y;

        number<gmp_float<0>> positionY(0);
        number<gmp_float<0>> positionX(0);
        positionX.precision(biggestPrecision);
        positionY.precision(biggestPrecision);

        //y-axis
        axis.setSize(sf::Vector2f(targetWindow.getSize().x, AXIS_WIDTH));
        positionY = targetWindow.getSize().y - (((- minScreenY) / (maxScreenY - minScreenY)) * targetWindow.getSize().y) + posY;
        position.y = positionY.convert_to<float>() - (AXIS_WIDTH/2);
        if (position.y >= -(3/2*AXIS_WIDTH) && position.y <= targetWindow.getSize().y - (AXIS_WIDTH/2))
        {
            position.x = 0;
            axis.setPosition(position);
            targetWindow.draw(axis);
        }

        int minBound(0);
        int maxBound(allNumbers.size());

        positionX = (posX / HORIZONTAL_AXIS_SIZE) - point.getRadius() - 1;
        minBound = positionX.convert_to<int>();

        positionX = ((targetWindow.getSize().x + point.getRadius() + posX) / HORIZONTAL_AXIS_SIZE) - 1;
        maxBound = positionX.convert_to<int>();

        if (maxBound > allNumbers.size())
            maxBound = allNumbers.size();

        if (minBound < 0)
            minBound = 0;

        int toAdd(0);

        toAdd = (allNumbers.size()) / targetWindow.getSize().x;

        if (toAdd < 1)
            toAdd = 1;

        if (minBound < allNumbers.size() && maxBound >= 0)
        {
            sf::VertexArray linesVertices(sf::LinesStrip, ((maxBound - minBound) / toAdd) + 1);

            for (int i(minBound) ; i < maxBound ; i+=toAdd)
            {
                    positionX = ((i+1) * HORIZONTAL_AXIS_SIZE) - posX;
                    positionY = targetWindow.getSize().y - (((allNumbers[i] - minScreenY) / (maxScreenY - minScreenY)) * targetWindow.getSize().y) + posY;

                    positionX -= point.getRadius();
                    positionY -= point.getRadius();

                    position.x = positionX.convert_to<float>();
                    position.y = positionY.convert_to<float>();

                    linesVertices[(i - minBound)/toAdd].position = sf::Vector2f(position.x + point.getRadius(), position.y + point.getRadius());
                    linesVertices[(i - minBound)/toAdd].color = sf::Color::Blue;

                    point.setPosition(position);

                    if (displayPoints)
                        targetWindow.draw(point);
            }

            if (displayLines)
                targetWindow.draw(linesVertices);
        }
    }
}
