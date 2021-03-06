#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <cstdlib>
#include <sstream>

#include <TextEntity.h>

using namespace sf;
using namespace std;

extern int width;
extern int height;
extern const bool keyboardControl;

//make weapon info follow player
//rapid fire
//low damage gun $100

class Player{
public:

    float x,y;
    float speed;
    float bounds;
    float outline;

    bool noSprite;

    int coolDown;
    bool canFire;
    int lives;
    int score;
    bool flameGrow;
    int flameAnimationTimer;

    int money;
    int kills;

    int dualAmmo;
    int triAmmo;
    int shieldAmmo;

    int laserStrength;

    RectangleShape placeHolder;

    RectangleShape body;
    RectangleShape rightWing;
    RectangleShape leftWing;
    RectangleShape rightThrust;
    RectangleShape leftThrust;
    RectangleShape cockpit;

    RectangleShape topHUD;
    RectangleShape clearedHUD;
    RectangleShape weaponHUD;

    //Font font;
    Text weapon;

    enum partNames{BODY=0,RIGHTWING=1,LEFTWING=2};
    enum accessoryNames{RIGHTTHRUST=0,LEFTTHRUST=1, COCKPIT=2};
    enum hudNames{TOPHUD=0, CLEAREDHUD=1, WEAPONHUD=2};

    enum weaponChoices{BASICLASER=1, DUALSHOT=2, TRISHOT=3, SHIELDPURGE=4};
    int currentWeapon;

    vector<RectangleShape> parts;
    vector<RectangleShape> accessories;
    vector<RectangleShape> huds;

    vector<TextEntity> textEntities;

    Player(float x2, float y2, Font &font){
        x=x2;
        y=y2;
        bounds=30;
        outline=2;

        noSprite=false;
        canFire=true;
        speed=1;
        lives=3;
        score=0;

        kills=0;
        money=0;

        flameAnimationTimer=10;
        flameGrow=false;

        dualAmmo=0;
        triAmmo=0;
        shieldAmmo=0;

        laserStrength=1;

        weapon.setFont(font);
        weapon.setCharacterSize(12);
        weapon.setColor(Color::White);
        weapon.setString("Single Laser");
        //weapon.setPosition(width-250,height-weapon.getGlobalBounds().height*1.2);

        body.setFillColor(Color(0,255,33));
        body.setOutlineColor(Color(0,127,14));
        body.setOutlineThickness(outline);
        body.setSize(Vector2f(bounds/2-(outline*2),bounds-(outline*2)));
        body.setPosition(width/2-body.getGlobalBounds().width, height-body.getGlobalBounds().height-(bounds*1.5));

        leftWing.setFillColor(Color(0,255,33));
        leftWing.setOutlineColor(Color(0,127,14));
        leftWing.setOutlineThickness(outline);
        leftWing.setSize(Vector2f(bounds/4-(outline*2),bounds/1.5));
        leftWing.setPosition(Vector2f(body.getPosition().x-leftWing.getGlobalBounds().width,
                                      body.getPosition().y/2));

        rightWing.setFillColor(Color(0,255,33));
        rightWing.setOutlineColor(Color(0,127,14));
        rightWing.setOutlineThickness(outline);
        rightWing.setSize(Vector2f(bounds/4-(outline*2),bounds/1.5));
        rightWing.setPosition(Vector2f(body.getPosition().x+body.getGlobalBounds().width,
                                       body.getPosition().y/2));

        leftThrust.setFillColor(Color(255,216,0));
        leftThrust.setOutlineColor(Color(255,0,0));
        leftThrust.setOutlineThickness(outline);
        leftThrust.setSize(Vector2f(bounds/4-(outline*2), bounds/10));
        leftThrust.setPosition(Vector2f(leftThrust.getPosition().x,
                                        leftThrust.getPosition().y-leftThrust.getGlobalBounds().height));

        rightThrust.setFillColor(Color(255,216,0));
        rightThrust.setOutlineColor(Color(255,0,0));
        rightThrust.setOutlineThickness(outline);
        rightThrust.setSize(Vector2f(bounds/4-(outline*2), bounds/10));
        rightThrust.setPosition(Vector2f(rightThrust.getPosition().x,
                                        rightThrust.getPosition().y-leftThrust.getGlobalBounds().height));

        cockpit.setFillColor(Color(0,255,255));
        cockpit.setOutlineColor(Color(0,127,127));
        cockpit.setOutlineThickness(outline);
        cockpit.setSize(Vector2f(bounds/2-outline*4,bounds/5));
        cockpit.setPosition(body.getPosition().x+(bounds/15),body.getPosition().y+(bounds/20));

        topHUD.setFillColor(Color(0,255,255,100));
        topHUD.setOutlineColor(Color(0,150,150,100));
        topHUD.setOutlineThickness(outline);
        topHUD.setSize(Vector2f(width-(outline*2),height/8));
        topHUD.setPosition(outline,outline);

        clearedHUD.setFillColor(Color(0,255,255,100));
        clearedHUD.setOutlineColor(Color(0,150,150,100));
        clearedHUD.setOutlineThickness(outline);
        clearedHUD.setSize(Vector2f(250,25));
        clearedHUD.setPosition(0,height-clearedHUD.getGlobalBounds().height+(outline*2));

        weaponHUD.setFillColor(Color(0,255,255,100));
        weaponHUD.setOutlineColor(Color(0,150,150,100));
        weaponHUD.setOutlineThickness(outline);
        weaponHUD.setSize(Vector2f(250,25));
        weaponHUD.setPosition(width-weaponHUD.getGlobalBounds().width,
                              height-weaponHUD.getGlobalBounds().height);

        parts.push_back(body);
        parts.push_back(rightWing);
        parts.push_back(leftWing);

        accessories.push_back(rightThrust);
        accessories.push_back(leftThrust);
        accessories.push_back(cockpit);

        //huds.push_back(topHUD);
        //huds.push_back(clearedHUD);
        //huds.push_back(weaponHUD);

        //0=body
        //1=rightWing



        if (noSprite){
            placeHolder.setSize(Vector2f(bounds,bounds));
            placeHolder.setPosition(width/2-placeHolder.getGlobalBounds().width, height-placeHolder.getGlobalBounds().height-10);
            //placeHolder.setPosition(width/2,height/2);
            placeHolder.setFillColor(Color::Green);
        }

        currentWeapon=BASICLASER;
    }

    void strafe(RenderWindow &window, Time &time){
        weaponSelection();

        if(noSprite){
            x=placeHolder.getPosition().x;
            y=placeHolder.getPosition().y;
        }else {
            x=body.getPosition().x;
            y=body.getPosition().y;
        }
        float x2=0,y2=0;

        if (keyboardControl){
            if((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) &&
                    parts[BODY].getPosition().y>0){
                y2=-speed;
            }

            if((Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) &&
                    accessories[LEFTTHRUST].getPosition().y+accessories[LEFTTHRUST].getGlobalBounds().height<height){
                y2=speed;
            }
            if((Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
               && parts[RIGHTWING].getPosition().x+parts[RIGHTWING].getGlobalBounds().width<width){
                //if (noSprite){placeHolder.move(Vector2f(speed,0));}
                x2=speed;
            } else if ((Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
                       && parts[LEFTWING].getPosition().x>0){
                //if (noSprite){placeHolder.move(Vector2f(-speed,0));}
                x2=-speed;
            }
            if(noSprite){
                placeHolder.move(Vector2f(x2*time.asMilliseconds(),y2*time.asMilliseconds()));
            } else {
                parts[BODY].move(Vector2f(x2*time.asSeconds()*60,y2*time.asSeconds()*60));
                syncParts();
            }

        } else {
            placeHolder.setPosition(Mouse::getPosition(window).x-(placeHolder.getGlobalBounds().width/2),
                                        height-placeHolder.getGlobalBounds().height-10);
        }
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

        for(int loop=0;loop<textEntities.size();loop++){
            textEntities[loop].draw(window);
            if(textEntities[loop].timer<=0){
                textEntities.erase(textEntities.begin()+loop);
                break;
            }
        }
    }

    void drawHuds(RenderWindow &window){
        for(int x=0;x<huds.size();x++){
            window.draw(huds[x]);
        }

        refreshAmmo();

        window.draw(weapon);
    }

    void resetPosition(){
        if(noSprite){
            placeHolder.setPosition(width/2-placeHolder.getGlobalBounds().width, height-placeHolder.getGlobalBounds().height);
        } else {
            parts[BODY].setPosition(width/2-parts[BODY].getGlobalBounds().width,
                                    height-parts[BODY].getGlobalBounds().height-(bounds));
            syncParts();
        }
    }

    void syncParts(){
        parts[RIGHTWING].setPosition(Vector2f(parts[BODY].getPosition().x+parts[BODY].getGlobalBounds().width,
                                       parts[BODY].getPosition().y+(bounds/2)));
        parts[LEFTWING].setPosition(Vector2f(parts[BODY].getPosition().x-parts[LEFTWING].getGlobalBounds().width,
                                      parts[BODY].getPosition().y+(bounds/2)));
        accessories[LEFTTHRUST].setPosition(Vector2f(parts[LEFTWING].getPosition().x,
                                        parts[LEFTWING].getPosition().y+parts[LEFTWING].getGlobalBounds().height));
        accessories[RIGHTTHRUST].setPosition(Vector2f(parts[RIGHTWING].getPosition().x,
                                        parts[RIGHTWING].getPosition().y+parts[LEFTWING].getGlobalBounds().height));
        accessories[COCKPIT].setPosition(parts[BODY].getPosition().x+(bounds/15),
                                         parts[BODY].getPosition().y+(bounds/20));

        if(parts[BODY].getPosition().x-(parts[BODY].getGlobalBounds().width/2)<width/2){
            weapon.setPosition(parts[BODY].getPosition().x+parts[BODY].getGlobalBounds().width+20,
                               parts[BODY].getPosition().y);
        } else {
            weapon.setPosition(parts[BODY].getPosition().x-20-weapon.getGlobalBounds().width,
                               parts[BODY].getPosition().y);
        }
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

    void weaponSelection(){
        if(Keyboard::isKeyPressed(Keyboard::Num1)){
            currentWeapon=BASICLASER;
            weapon.setString("Single Laser");
        } else if (Keyboard::isKeyPressed(Keyboard::Num2)){
            currentWeapon=DUALSHOT;

            stringstream s;
            s << "Dual Laser\n" << dualAmmo;
            weapon.setString(s.str());
        } else if(Keyboard::isKeyPressed(Keyboard::Num3)){
            currentWeapon=TRISHOT;

            stringstream s;
            s << "Triple Laser\n" << triAmmo;
            weapon.setString(s.str());
        } else if(Keyboard::isKeyPressed(Keyboard::Num4)){
            currentWeapon=SHIELDPURGE;

            stringstream s;
            s << "Shield Purge\n" << shieldAmmo;
            weapon.setString(s.str());
        }
    }

    void refreshAmmo(){
        if(currentWeapon==DUALSHOT){
            stringstream s;
            s << "Dual Laser\n" << dualAmmo;
            weapon.setString(s.str());
        } else if(currentWeapon==TRISHOT){
            stringstream s;
            s << "Triple Laser\n" << triAmmo;
            weapon.setString(s.str());
        }else if(currentWeapon==SHIELDPURGE){
            stringstream s;
            s << "Shield Purge\n" << shieldAmmo;
            weapon.setString(s.str());
        }
    }

    void moneyPop(float enemyX, float enemyY){
        stringstream s;
        s << "$" << money;
        textEntities.push_back(TextEntity(enemyX,enemyY,16,s.str(),Color::Green,60*2));
    }

    void killPop(){
        stringstream s;
        s << "Kills: " << kills;

        textEntities.push_back(TextEntity(parts[BODY].getPosition().x,
                                         parts[BODY].getPosition().y-20,
                                         16,s.str(),Color::Red,60*1));
    }

    void deathPop(){
        stringstream s;
        s << "Ships Left: " << lives;

        textEntities.push_back(TextEntity(parts[BODY].getPosition().x,
                                         parts[BODY].getPosition().y,
                                         16,s.str(),Color::Yellow,60*3));
    }

    void enemiesRemainingPop(float enemyX, float enemyY, int remaining){
        stringstream s;
        s << remaining << " enemies left";

        textEntities.push_back(TextEntity(enemyX,enemyY,16,s.str(),Color::Red,60*1));
    }

    void sectorClearedPop(int sectorsCleared){
        stringstream s;
        s << sectorsCleared << " Sectors Cleared";

        textEntities.push_back(TextEntity(width/3,height/2,50,s.str(),Color::Yellow,60*2));
    }
};

#endif // PLAYER_H
