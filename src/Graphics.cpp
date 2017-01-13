
#include "Graphics.hpp"



bool launch()
{
    sf::RenderWindow myWindow(sf::VideoMode(800, 640), "Draw big numbers");
    sf::Event event;
    sf::Clock imGuiClock;

    std::vector<BigNumber> mySet;
    std::string currentDirectory;

    int currentPowerOfTen(1);//Means that the screen length = 1, half the screen length = 1/2...

    sf::Vector2i coordinates(0,0);

    char newDirectory[256] = "";

    ImGui::SFML::Init(myWindow);

    myWindow.resetGLStates();

    std::stringstream interfaceString;

    sf::Clock keyWait;
    keyWait.restart();

    do
    {
        while (myWindow.pollEvent(event))
        {
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                currentPowerOfTen += event.mouseWheelScroll.delta;
            }

            if (keyWait.getElapsedTime().asSeconds() >= 0.05)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    coordinates.x -= SPEED;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    coordinates.x += SPEED;

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    coordinates.y -= SPEED;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    coordinates.y += SPEED;

                keyWait.restart();
            }

            ImGui::SFML::ProcessEvent(event);
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
            interfaceString << "Current boundaries scale : 10^(" << currentPowerOfTen << ")\n";
            ImGui::Text(interfaceString.str().c_str());

            interfaceString.str(std::string());
            interfaceString << "Current coordinates :("<< coordinates.x << ';' << coordinates.y << "e" << currentPowerOfTen << ")\n";
            ImGui::Text(interfaceString.str().c_str());

        ImGui::End();

        myWindow.clear(sf::Color(255, 255, 255));


            ImGui::Render();

        myWindow.display();

    }while (1);

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
                 int powerOfTenScale, sf::Vector2i const& pos)
{

}
