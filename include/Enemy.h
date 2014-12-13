#ifndef ENEMY_H
#define ENEMY_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <cstdlib>
#include <vector>

using namespace sf;
using namespace std;

extern const int width;
extern const int height;


class Enemy{
public:

    float x,y;
    float speed;
    float bounds;
    bool noSprite;
    bool kill;
    bool flameGrow;
    int flameAnimationTimer;

    enum direction{LEFT, RIGHT};
    enum availableAI{CHARGER, WEAVER, STRAFER};
    int direction;
    int ai;

    RectangleShape placeHolder;

    RectangleShape body;
    RectangleShape rightWing;
    RectangleShape leftWing;
    RectangleShape rightThrust;
    RectangleShape leftThrust;
    RectangleShape cockpit;

    enum partNames{BODY=0,RIGHTWING=1, LEFTWING=2};
    enum accessoryNames{RIGHTTHRUST=0, LEFTTHRUST=1, COCKPIT=2};
    float outlineSize;

    vector<RectangleShape> parts;
    vector<RectangleShape> accessories;

    Enemy(float x2, float y2, int type){
        x=x2;
        y=y2;

        speed=.3;
        bounds=20;
        kill=false;

        noSprite=false;
        flameGrow=false;
        flameAnimationTimer=10;

        outlineSize=2;

        switch(type){
            case 1:
                ai=CHARGER;
                break;
            case 2:
                ai=WEAVER;
                break;
            case 3:
                ai=STRAFER;
                break;
        }

        if(noSprite){
            placeHolder.setSize(Vector2f(bounds,bounds));
            placeHolder.setPosition(Vector2f(x,y));
            placeHolder.setFillColor(Color::Red);
        } else {
            body.setSize(Vector2f(bounds-outlineSize,bounds/1.5-outlineSize));
            body.setPosition(x,y);
            body.setFillColor(Color(255,0,0));
            body.setOutlineColor(Color(127,0,0));
            body.setOutlineThickness(outlineSize);

            leftWing.setSize(Vector2f(bounds/2-outlineSize,bounds-outlineSize));
            leftWing.setPosition(body.getPosition().x-leftWing.getGlobalBounds().width,
                                 body.getPosition().y+(bounds/1.25));
            leftWing.setFillColor(Color(255,0,0));
            leftWing.setOutlineColor(Color(127,0,0));
            leftWing.setOutlineThickness(outlineSize);

            rightWing.setSize(Vector2f(bounds/2-outlineSize,bounds-outlineSize));
            rightWing.setPosition(body.getPosition().x+body.getGlobalBounds().width+rightWing.getGlobalBounds().width,
                                  body.getPosition().y+(bounds/1.25));
            rightWing.setFillColor(Color(255,0,0));
            rightWing.setOutlineColor(Color(127,0,0));
            rightWing.setOutlineThickness(outlineSize);

            leftThrust.setSize(Vector2f(bounds/2,bounds/5));
            leftThrust.setPosition(leftWing.getPosition().x,leftWing.getPosition().y-leftThrust.getGlobalBounds().height);
            leftThrust.setFillColor(Color(0,255,255));
            leftThrust.setOutlineColor(Color(0,19,127));
            leftThrust.setOutlineThickness(outlineSize);

            rightThrust.setSize(Vector2f(bounds/2,bounds/5));
            rightThrust.setPosition(rightWing.getPosition().x,rightWing.getPosition().y+rightThrust.getGlobalBounds().height);
            rightThrust.setFillColor(Color(0,255,255));
            rightThrust.setOutlineColor(Color(0,19,127));
            rightThrust.setOutlineThickness(outlineSize);

            cockpit.setFillColor(Color(64,64,64));
            cockpit.setOutlineColor(Color(0,0,0));
            cockpit.setOutlineThickness(outlineSize);
            cockpit.setSize(Vector2f(bounds-outlineSize*4,bounds/4));
            cockpit.setPosition(body.getPosition().x+bounds/15,
                                body.getPosition().y-bounds/20);

            parts.push_back(body);
            parts.push_back(rightWing);
            parts.push_back(leftWing);
            accessories.push_back(leftThrust);
            accessories.push_back(rightThrust);
            accessories.push_back(cockpit);
        }

        if(ai==WEAVER || ai==STRAFER){
            if(rand() % 2==0){
                direction=RIGHT;
            } else {
                direction=LEFT;
            }
        }
    }

    void logic(Time &time){
        x=parts[BODY].getPosition().x;
        y=parts[BODY].getPosition().y;

        if(ai==CHARGER){
            parts[BODY].move(0,speed*time.asSeconds()*60);
        } else if(ai==WEAVER){
            if(rand() % 300==0){
                if(direction==LEFT){
                    direction=RIGHT;
                } else {
                    direction=LEFT;
                }
            }

            if(x>=width-placeHolder.getGlobalBounds().width){
                direction=LEFT;
            }

            if(x<=0){
                direction=RIGHT;
            }

            if(direction==RIGHT){
                parts[BODY].move(speed/2*time.asSeconds()*60,speed*time.asSeconds()*60);
            }else {
                parts[BODY].move(-speed/2*time.asSeconds()*60,speed*time.asSeconds()*60);
            }
        } else if(ai=STRAFER){
            if(x<=0){
                parts[BODY].move(0,speed*50*time.asSeconds()*60);
                direction=RIGHT;
            }

            if(x>=width-placeHolder.getGlobalBounds().width){
                parts[BODY].move(0,speed*10*time.asSeconds()*60);
                direction=LEFT;
            }

            if(direction==LEFT){
                parts[BODY].move(-speed*2*time.asSeconds()*60,0);
            } else if(direction==RIGHT){
                parts[BODY].move(speed*2*time.asSeconds()*60,0);
            }
        }

        if(y>(height+(rand() % height)+(height/4))){
            resetPosition();
        }

        syncParts();
    }

    void resetPosition(){
        x=(rand() % (int)(width-bounds*2))+bounds*2;
        y=0;
        parts[BODY].setPosition(Vector2f(x,y));
    }

    void draw(RenderWindow &window){
        animate();

        if(noSprite){
            window.draw(placeHolder);
        } else {
            for(int x=0;x<parts.size();x++){
                window.draw(parts[x]);
            }
            for(int x=0;x<accessories.size();x++){
                window.draw(accessories[x]);
            }
        }
    }

    bool shouldFire(){
        if (rand()%60==0){
            return true;
        }

        return false;
    }

    void syncParts(){
        parts[LEFTWING].setPosition(parts[BODY].getPosition().x-parts[LEFTWING].getGlobalBounds().width,
                                 parts[BODY].getPosition().y-(bounds/3));
        parts[RIGHTWING].setPosition(parts[BODY].getPosition().x+parts[BODY].getGlobalBounds().width,
                                 parts[BODY].getPosition().y-(bounds/3));
        accessories[LEFTTHRUST].setPosition(parts[LEFTWING].getPosition().x-outlineSize,
                                      parts[LEFTWING].getPosition().y-accessories[LEFTTHRUST].getGlobalBounds().height);
        accessories[RIGHTTHRUST].setPosition(parts[RIGHTWING].getPosition().x-outlineSize,
                                       parts[RIGHTWING].getPosition().y-accessories[RIGHTTHRUST].getGlobalBounds().height);
        accessories[COCKPIT].setPosition(parts[BODY].getPosition().x+bounds/6,
                                parts[BODY].getPosition().y+bounds/3);
    }

    void animate(){
        flameAnimationTimer--;

        if(flameAnimationTimer<=0){
            flameGrow=!flameGrow;
            flameAnimationTimer=10;
        }

        if(flameGrow){
            accessories[RIGHTTHRUST].setSize(Vector2f(accessories[RIGHTTHRUST].getSize().x,
                                                      accessories[RIGHTTHRUST].getSize().y+.2));
            accessories[LEFTTHRUST].setSize(Vector2f(accessories[LEFTTHRUST].getSize().x,
                                                     accessories[LEFTTHRUST].getSize().y+.2));
        } else {
            accessories[RIGHTTHRUST].setSize(Vector2f(accessories[RIGHTTHRUST].getSize().x,
                                                      accessories[RIGHTTHRUST].getSize().y-.2));
            accessories[LEFTTHRUST].setSize(Vector2f(accessories[LEFTTHRUST].getSize().x,
                                                     accessories[LEFTTHRUST].getSize().y-.2));
        }

        syncParts();
    }
};

#endif // ENEMY_H
