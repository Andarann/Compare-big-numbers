
#include "Graphics.hpp"

bool displayPoints;
bool displayLines;

bool launch()
{
    displayLines = true;
    displayPoints = true;

    sf::RenderWindow myWindow(sf::VideoMode(800, 640), "Draw big numbers", sf::Style::Fullscreen);
    sf::Event event;
    sf::Clock imGuiClock;

    std::vector<number<gmp_float<0>>> mySet;
    std::string currentDirectory;

    float currentPowerOfTen(1);

    float zoomFactor(1.0);

    char newDirectory[256] = "";

    ImGui::SFML::Init(myWindow);

    myWindow.resetGLStates();

    std::stringstream interfaceString;

    sf::Clock keyWait;
    keyWait.restart();

    BigNumber numberScale;

    bool stop(false);

    number<gmp_float<20000>> posX(0);
    number<gmp_float<20000>> posY(0);

    int numberToReach(0);

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

        powerOfTen.precision(std::abs(currentPowerOfTen));

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
                    currentDirectory = newDirectory;
            }

            interfaceString.str(std::string());
            interfaceString << "Current coordinates :("<< posX << ';' << posY << ")\n";
            ImGui::Text(interfaceString.str().c_str());

            ImGui::InputFloat("Zoom factor", &currentPowerOfTen);
            if (currentPowerOfTen > INT_MAX/10)
                currentPowerOfTen = INT_MAX/10;

            if (currentPowerOfTen < INT_MIN/10)
                currentPowerOfTen = INT_MIN/10;

            ImGui::InputInt("Space between points x-axis", &HORIZONTAL_AXIS_SIZE);

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
                    posX = ((numberToReach+1) * HORIZONTAL_AXIS_SIZE);
                    posY = mySet[numberToReach];
                }
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

bool loadNewSet(std::vector<number<gmp_float<0>>>& allNumbers, std::string const& newFilePath)
{
    int newSetSize(0);

    try {
        newSetSize = BigNumber::numberQuantityInFile(newFilePath);
    } catch (const char* e) {
        std::cout << "Error while loading new set : " << e << '\n';
        return false;//The set is left unchanged
    }

    allNumbers.clear();
    BigNumber temp;

    for (int i(0) ; i < newSetSize ; i++)
    {
        temp.loadFromFile(newFilePath, i);
        allNumbers.push_back(temp.convertBigNumber());
    }

    std::sort(allNumbers.begin(), allNumbers.end());//We then sort the array

    return true;
}

bool writeSortedArray(std::vector<number<gmp_float<0>>> const& allNumbers, std::string filePath)
{
    std::ofstream fileToStream;
    fileToStream.open(filePath.c_str(), std::ios::app);

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
    if (allNumbers.size() > 0)
    {
        sf::CircleShape point;
        point.setRadius(8);
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

        sf::Vector2f position;

        sf::VertexArray linesVertices(sf::LinesStrip, allNumbers.size() );



        for (int i(0) ; i < allNumbers.size() ; i++)
        {
                positionX = ((i+1) * HORIZONTAL_AXIS_SIZE) - posX;
                positionY = targetWindow.getSize().y - (((allNumbers[i] - minScreenY) / (maxScreenY - minScreenY)) * targetWindow.getSize().y) + posY;

                positionX -= point.getRadius();
                positionY -= point.getRadius();

                position.x = positionX.convert_to<float>();
                position.y = positionY.convert_to<float>();

                linesVertices[i ].position = sf::Vector2f(position.x + point.getRadius(), position.y + point.getRadius());
                linesVertices[i ].color = sf::Color::Blue;

                point.setPosition(position);

                if (displayPoints)
                    targetWindow.draw(point);
        }

        if (displayLines)
            targetWindow.draw(linesVertices);
    }
}
