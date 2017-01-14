
#include "Graphics.hpp"



bool launch()
{
    sf::RenderWindow myWindow(sf::VideoMode(800, 640), "Draw big numbers", sf::Style::Fullscreen);
    sf::Event event;
    sf::Clock imGuiClock;

    std::vector<BigNumber> mySet;
    std::string currentDirectory;

    int currentPowerOfTen(0);//Means that the screen length = 1, half the screen length = 1/2...

    float zoomFactor(1.0);

    char newDirectory[256] = "";

    ImGui::SFML::Init(myWindow);

    myWindow.resetGLStates();

    std::stringstream interfaceString;

    sf::Clock keyWait;
    keyWait.restart();

    BigNumber numberScale;
    sf::Vector2i coordinates(-30, 30);

    bool stop(false);

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

        if (keyWait.getElapsedTime().asSeconds() >= 0.01)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                coordinates.x -= SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                coordinates.x += SPEED;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                coordinates.y += SPEED;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                coordinates.y -= SPEED;

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
            interfaceString << "Current coordinates :("<< coordinates.x << ';' << coordinates.y << "e" << currentPowerOfTen << ")\n";
            ImGui::Text(interfaceString.str().c_str());

            ImGui::InputFloat("Zoom factor", &zoomFactor);

            ImGui::InputInt("Space between points x-axis", &HORIZONTAL_AXIS_SIZE);

            if (ImGui::Button("Write sorted array to a debug.txt"))
            {
                for (int i(0) ; i < mySet.size() ; i++)
                    mySet[i].writeToFile("debug.txt");
            }

            if (ImGui::Button("Stop program"))
            {
                stop = true;
            }
        ImGui::End();

        myWindow.clear(sf::Color(255, 255, 255));


            if (zoomFactor != 0)
                drawNumbers(mySet, myWindow, zoomFactor, coordinates, numberScale);
            else
                drawNumbers(mySet, myWindow, 1.0, coordinates, numberScale);

            ImGui::Render();

        myWindow.display();

    }while (!stop);

    ImGui::SFML::Shutdown();

    return true;
}

bool loadNewSet(std::vector<BigNumber>& allNumbers, std::string const& newFilePath)
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
        allNumbers.push_back(temp);
    }

    std::sort(allNumbers.begin(), allNumbers.end());//We then sort the array

    return true;
}

void drawNumbers(std::vector<BigNumber> const& allNumbers, sf::RenderWindow const& targetWindow,
                 float zoomFactor, sf::Vector2i const& pos, BigNumber& numberScale)
{
    sf::CircleShape point;
    point.setRadius(8);

    std::vector<BigNumber> allScales;//The value of each graduation
    std::vector<double> coordinatesY;//Y coordinates of every big number visible

    int firstDifference = 0;
    double min(0.0);
    double max(0.0);

    std::stringstream unitScale;
    numberScale.changeNumber("0.0");//Value of one scale

    long canvasHeight = static_cast<long>(zoomFactor * targetWindow.getSize().y);
    long numberOfGraduations = canvasHeight;

    //We print the axis
    sf::RectangleShape axis;
    axis.setFillColor(sf::Color(0,0,0));

    if (pos.x < 0 && pos.x > targetWindow.getSize().x)
    {
        axis.setPosition( -pos.x, 0);
        axis.setSize(sf::Vector2f(AXIS_WIDTH, targetWindow.getSize().y));
        targetWindow.draw(axis);
    }

    if (pos.y > 0 && pos.y < targetWindow.getSize().y)
    {
        axis.setPosition(0, targetWindow.getSize().y - pos.y);
        axis.setSize(sf::Vector2f(targetWindow.getSize().x, AXIS_WIDTH));
        targetWindow.draw(axis);
    }

    //Since the array is sorted, we can safely assume that the first element is the smallest, and the last is the biggest. We'll use that for comparison
    if (allNumbers.size() > 0)
    {

        for (firstDifference = 0 ; allNumbers[0].getPowerOfTenDigit(firstDifference) == allNumbers[allNumbers.size() - 1].getPowerOfTenDigit(firstDifference) ; firstDifference--);

        //We then take 6 digits, for maximum precision on as much screen as possible

        for (int i(0); i < 6 ; i++)
        {
            max += allNumbers[allNumbers.size() - 1].getPowerOfTenDigit(firstDifference - i) / std::pow(10, i);
            min += allNumbers[0].getPowerOfTenDigit(firstDifference - i) / std::pow(10, i);
        }
        unitScale << "0.";

        for (int i(0) ; i < std::abs((firstDifference) + std::log10((max - min) / numberOfGraduations)) - 1 ; i++)
            unitScale << '0';

        std::stringstream tempString;
        tempString <<  ((max - min) / numberOfGraduations);
        unitScale << tempString.str().substr(2 + std::abs(std::log10((max - min) / numberOfGraduations)));

        numberScale.changeNumber(unitScale.str());

        allScales.push_back(BigNumber(allNumbers[0]));

        for (int i(1) ; i < numberOfGraduations ; i++)
        {
            allScales.push_back( numberScale + allScales[i - 1]);

        }

        for (int i(0) ; i < allNumbers.size() ; i++)
        {
            if (allNumbers[i] < allScales[0])
            { //Under the scale, shouldn't happen
                coordinatesY.push_back(-1);
                //std::cout << "Under the scale\n";
            }
            else if (allNumbers[i] > allScales[allScales.size() - 1])
            { //Above the scale, shouldn't happen either

                coordinatesY.push_back(1.1);
                //std::cout << "Above the scale\n";
            }
            else
            {
                for (int j(0) ; j < allScales.size() - 1; j++)
                {
                    if ((allNumbers[i] >= allScales[j]) && (allNumbers[i] <= allScales[j + 1]))
                    {

                        coordinatesY.push_back(((float)j / numberOfGraduations));
                        break;
                    }
                }
            }
        }

        for (int i(0) ; i < allNumbers.size() ; i++)
        {
            sf::Vector2f position;


            position.x = ((i + 1) * HORIZONTAL_AXIS_SIZE) - point.getRadius() - pos.x;
            position.y = targetWindow.getSize().y - coordinatesY[i] * canvasHeight - point.getRadius() - pos.y;
            point.setFillColor(sf::Color(255, 0, 0));

            if (position.x < - point.getRadius())
            {
                position.x = - point.getRadius();
                point.setFillColor(sf::Color(255, 50, 255));
            }
            else if (position.x > targetWindow.getSize().x - point.getRadius())
            {
                position.x = targetWindow.getSize().x - point.getRadius();
                point.setFillColor(sf::Color(255, 50, 255));
            }

            point.setPosition(position.x, position.y);


            targetWindow.draw(point);
        }

    }
}
