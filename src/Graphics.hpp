#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

#include <SFML/Graphics.hpp>
#include <set>
#include <vector>
#include <algorithm>

#include "imgui.h"
#include "imgui-SFML.h"

#include "BigNumber.hpp"

#define HORIZONTAL_AXIS_SIZE 50//Distance between two points on the horizontal axis, in pixels
#define SPEED 1//Number of pixels passed every time a key is pressed

bool launch();
bool loadNewSet(std::vector<BigNumber>& allNumbers, std::string const& newFilePath);
void drawNumbers(std::vector<BigNumber> const& allNumbers, sf::RenderWindow const& targetWindow,
                 int powerOfTenScale, sf::Vector2i const& pos);

#endif // GRAPHICS_HPP_INCLUDED
