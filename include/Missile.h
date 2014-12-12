#ifndef MISSILE_H
#define MISSILE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

extern const int width;
extern const int height;


class Missile{
public:
    float x,y;
    bool hostile;
    float speed;
    float bounds;
    bool noSprite;
    RectangleShape placeHolder;

    Missile(float x2, float y2, bool h){
        noSprite=true;
        x=x2;
        y=y2;
        hostile=h;
        speed=1.5;
        bounds=13;

        if(noSprite){
            placeHolder.setSize(Vector2f(bounds/2,bounds));
            placeHolder.setPosition(Vector2f(x,y));

            placeHolder.setFillColor(Color::White);
            placeHolder.setOutlineThickness(3.5);

            if(hostile){
                placeHolder.setOutlineColor(Color::Red);
            } else {
                placeHolder.setOutlineColor(Color::Blue);
            }
        }
    }

    void draw(RenderWindow &window){
        if (noSprite){
            window.draw(placeHolder);
        }
    }

    void travel(Time &time){
        x=placeHolder.getPosition().x;
        y=placeHolder.getPosition().y;
        if (hostile){
            if(noSprite){
                placeHolder.move(0,speed*time.asSeconds()*60);
            } else {
                y+=speed;
            }
        } else {
            if(noSprite){
                placeHolder.move(0,-speed*time.asSeconds()*60);
            } else {
                y-=speed;
            }
        }
    }
};

#endif // MISSILE_H
