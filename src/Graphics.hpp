#ifndef GRAPHICS_HPP_INCLUDED
#define GRAPHICS_HPP_INCLUDED

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>

#include "imgui.h"
#include "imgui-SFML.h"

#include "BigNumber.hpp"

//#define HORIZONTAL_AXIS_SIZE 50//Distance between two points on the horizontal axis, in pixels
#define AXIS_WIDTH 2




bool launch();
float findScale(number<gmp_float<0>>& nMin, number<gmp_float<0>>& nMax, float windowHeight);
bool loadNewSet(std::vector<number<gmp_float<0>>>& allNumbers, std::string const& newFilePath);
bool writeSortedArray(std::vector<number<gmp_float<0>>> const& allNumbers, std::string filePath);
void drawNumbers(std::vector<number<gmp_float<0>>> const& allNumbers, sf::RenderWindow const& targetWindow,
                 float powerOfTenZoom, number<gmp_float<0>> posX, number<gmp_float<0>> posY);

#endif // GRAPHICS_HPP_INCLUDED
