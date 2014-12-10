#ifndef TEXTENTITY_H
#define TEXTENTITY_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

class TextEntity{
    Font font;
    Text text;

    int textSize;

    TextEntity(float x, float y, int charSize){

    }

};

#endif // TEXTENTITY_H
