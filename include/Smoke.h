#ifndef SMOKE_H
#define SMOKE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

extern const int width;
extern const int height;

class Smoke{
public:

    float speed;
    float bounds;
    bool still;

    enum directions{LEFT, RIGHT};
    int direction;

    RectangleShape graphic;

    Smoke(float x, float y, float area, bool s){
        still=s;
        bounds=area;

        graphic.setSize(Vector2f(bounds,bounds));
        graphic.setFillColor(Color(255,0,0));
        graphic.setOutlineColor(Color(127,0,0));
        graphic.setOutlineThickness(2);
        graphic.setPosition(Vector2f(x,y));

        if(rand() % 2==0){
            direction=RIGHT;
        } else {
            direction=LEFT;
        }
    }

    void update(){
        if(still){
            bounds=bounds-0.3;
            graphic.move(0.3,0.3);
        } else {
            bounds=bounds-0.6;
            graphic.move(0.6,0.6);
            if(direction==LEFT){
                graphic.rotate(-5);
            } else {
                graphic.rotate(5);
            }
        }

        graphic.setSize(Vector2f(bounds,bounds));
    }

    void draw(RenderWindow &window){
        window.draw(graphic);
    }

    void travel(Vector2f dir, Time &time){
        graphic.move(dir.x*time.asSeconds()*60,dir.y*time.asSeconds()*60);
    }
};

#endif // SMOKE_H
