#ifndef STAR_H
#define STAR_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

extern const int width;
extern const int height;


class Star{
public:
    float x,y;
    float speed;
    float bounds;

    bool noSprite;
    RectangleShape placeHolder;

    Star(){
        x=rand() % width;
        y=rand() % height;
        bounds=(rand() % 4)+1;
        noSprite=true;
        speed=bounds/25;

        if(noSprite){
            placeHolder.setSize(Vector2f(bounds,bounds));
            placeHolder.setFillColor(Color::White);
            placeHolder.setPosition(Vector2f(x,y));
        }
    }

    void travel(Time &time){
        x=placeHolder.getPosition().x;
        y=placeHolder.getPosition().y;

        if(y>=height){
            bounds=(rand() % 4)+1;
            placeHolder.setSize(Vector2f(bounds,bounds));
            speed=bounds/25;

            placeHolder.setPosition(rand() % width, 0);
        } else {
            placeHolder.move(0,speed*time.asSeconds()*60);
        }
    }

    void draw(RenderWindow &window){
        if(noSprite){
            window.draw(placeHolder);
        }
    }
};

#endif // STAR_H
