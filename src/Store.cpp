#include "Store.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Player.h"

using namespace sf;
using namespace std;

extern int width;
extern int height;

Store::Store(){
    quit=false;
    font.loadFromFile("bin/LCD_Solid.ttf");

    title.setString("Store");
    title.setFont(font);
    title.setColor(Color::White);
    title.setCharacterSize(32);

    life.setString("New [L]ife: $10,000");
    life.setFont(font);
    life.setColor(Color::White);
    life.setCharacterSize(28);

    dual.setString("[D]ual Laser Ammo: $25");
    dual.setFont(font);
    dual.setColor(Color::White);
    dual.setCharacterSize(28);

    tri.setString("[T]ri Laser Ammo: $40");
    tri.setFont(font);
    tri.setColor(Color::White);
    tri.setCharacterSize(28);

    shieldPurge.setString("Shield [P]urge: $60");
    shieldPurge.setFont(font);
    shieldPurge.setColor(Color::White);
    shieldPurge.setCharacterSize(28);

    exitStore.setFont(font);
    exitStore.setString("[Q]uit");
    exitStore.setColor(Color::Red);
    exitStore.setCharacterSize(28);

    canBuy=true;
    buyDelay=5;
}
/*
Store::~Store(){
    delete p;
}
*/

void Store::logic(RenderWindow &window, int level){
    window.draw(title);
    window.draw(life);
    window.draw(exitStore);

    if(level>=2){
        window.draw(dual);
    }

    if(level>=3){
        window.draw(tri);
    }

    if(level>=5){
        window.draw(shieldPurge);
    }

    if(Keyboard::isKeyPressed(Keyboard::L) && p->money>=10000 && canBuy){
        p->lives++;
        p->money-=10000;

        canBuy=false;
    }

    if(Keyboard::isKeyPressed(Keyboard::D) && p->money>=25 && canBuy && level>=2){
        p->dualAmmo++;
        p->money-=25;

        canBuy=false;
    }

    if(Keyboard::isKeyPressed(Keyboard::T) && p->money>=40 && canBuy && level>=3){
        p->triAmmo++;
        p->money-=40;

        canBuy=false;
    }

    if(Keyboard::isKeyPressed(Keyboard::P) && p->money>=60 && canBuy && level>=5){
        p->shieldAmmo++;
        p->money-=60;

        canBuy=false;
    }

    if(Keyboard::isKeyPressed(Keyboard::Q)){
        quit=true;
    }

    buyDelay--;

    if(buyDelay<=0){
        buyDelay=5;
        canBuy=true;
    }
}

void Store::link(Player &player){
    p=&player;

    init();
    return;
}

void Store::init(){
    title.setPosition(width/2-(title.getGlobalBounds().width/2), title.getGlobalBounds().height);
    life.setPosition(width/2-(life.getGlobalBounds().width/2), height/10+life.getGlobalBounds().height);
    dual.setPosition(width/2-dual.getGlobalBounds().width/2,height/10+dual.getGlobalBounds().height*2);
    tri.setPosition(width/2-tri.getGlobalBounds().width/2,height/10+tri.getGlobalBounds().height*3);
    shieldPurge.setPosition(width/2-shieldPurge.getGlobalBounds().width/2,height/10+shieldPurge.getGlobalBounds().height*4);
    exitStore.setPosition(width/2-(exitStore.getGlobalBounds().width/2),height/10+exitStore.getGlobalBounds().height*5);
    return;
}
