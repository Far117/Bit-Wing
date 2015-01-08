#ifndef TEXTENTITY_H
#define TEXTENTITY_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

extern Font font;

class TextEntity{
public:

    Text text;

    int timer;

    TextEntity(float x, float y, int charSize, string s, Color color){
        //font.loadFromFile("bin/LCD_Solid.ttf");

        text.setFont(font);
        text.setString(s);
        text.setCharacterSize(charSize);
        text.setPosition(x,y);
        text.setColor(color);
    }

    TextEntity(float x, float y, int charSize, string s, Color color, int time){
        //font.loadFromFile("bin/LCD_Solid.ttf");

        text.setFont(font);
        text.setString(s);
        text.setCharacterSize(charSize);
        text.setPosition(x,y);
        text.setColor(color);

        timer=time;
    }

    void draw(RenderWindow &window){
        timer--;
        window.draw(text);
    }

    void travel(float x, float y){
        text.move(x,y);
    }

    void warp(float x, float y){
        text.setPosition(x,y);
    }

};

#endif // TEXTENTITY_H
