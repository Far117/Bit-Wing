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

extern int width;
extern int height;


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
    int shields;
    int maxShields;

    RectangleShape placeHolder;

    RectangleShape body;
    RectangleShape rightWing;
    RectangleShape leftWing;
    RectangleShape rightThrust;
    RectangleShape leftThrust;
    RectangleShape cockpit;

    enum partNames{BODY=0,RIGHTWING=1, LEFTWING=2};
    enum accessoryNames{RIGHTTHRUST=0, LEFTTHRUST=1, COCKPIT=2};
    enum fireTypes{SINGLE=1,DUAL=2,TRI=3,PURGE=4};
    int fireType;

    float outlineSize;

    vector<RectangleShape> parts;
    vector<RectangleShape> accessories;

    Enemy(float x2, float y2, int type, int level){
        x=x2;
        y=y2;

        speed=.3;
        bounds=20;
        kill=false;

        noSprite=false;
        flameGrow=false;
        flameAnimationTimer=10;

        outlineSize=2;

        int chance=0;
        bool getsShield=false;

        if(level>=3){
            if(level<10){
                chance=level;

                if((rand()%10)+1>=chance){
                    getsShield=true;
                }
            }else{
                getsShield=true;
            }
        }

        fireType=SINGLE;

        if(level>=3 && level<10){
            if(rand()%(10-level)==0){
                fireType=DUAL;
            }
        }else if(level>=3 && rand()%9==0){
            fireType=DUAL;
        }

        if(level>=5 && level<12){
            if(rand()%(12-level)==0){
                fireType=TRI;
            }
        } else if(level>=5 && rand()%9==0){
            fireType==TRI;
        }

        if(level>=7 && level<14){
            if(rand()%(14-level)==0){
                fireType=PURGE;
            }
        } else if (level>=7 && rand()%9==0){
            fireType=PURGE;
        }

        if(getsShield){
            shields=rand() % (int)((level/3)+1);
        } else {
            shields=0;
        }

        maxShields=shields;


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


            if(shields>0){
                body.setOutlineColor(Color::White);
                leftWing.setOutlineColor(Color::White);
                rightWing.setOutlineColor(Color::White);
            }

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

    void hit(){
        shields--;

        if(shields>0){
            parts[BODY].setOutlineColor(Color(155*shields/maxShields+100,
                                              155*shields/maxShields+100,
                                              155*shields/maxShields+100));
            parts[LEFTWING].setOutlineColor(Color(155*shields/maxShields+100,
                                                  155*shields/maxShields+100,
                                                  155*shields/maxShields+100));
            parts[RIGHTWING].setOutlineColor(Color(155*shields/maxShields+100,
                                                   155*shields/maxShields+100,
                                                   155*shields/maxShields+100));
        } else {
            parts[BODY].setOutlineColor(Color::Black);
            parts[LEFTWING].setOutlineColor(Color::Black);
            parts[RIGHTWING].setOutlineColor(Color::Black);
        }

    }
};

#endif // ENEMY_H
