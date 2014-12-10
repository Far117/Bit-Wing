#ifndef ASTEROID_H
#define ASTEROID_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

class Asteroid{
public:
    float x,y;
    float speed;
    float bounds;
    int life;
    bool noSprite;

    RectangleShape placeHolder;

    Asteroid(int x2, int y2){
        x=x2;
        y=y2;
    }
};

#endif // ASTEROID_H
