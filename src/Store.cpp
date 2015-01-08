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

    weapons.setFont(font);
    weapons.setColor(Color::White);
    weapons.setCharacterSize(30);
    weapons.setString("Weapons:");

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

    powers.setFont(font);
    powers.setColor(Color::White);
    powers.setCharacterSize(30);
    powers.setString("Upgrades:");

    life.setString("New [L]ife: $10,000");
    life.setFont(font);
    life.setColor(Color::White);
    life.setCharacterSize(28);

    laserPower.setFont(font);
    laserPower.setString("[I]ncrease Laser Penetration: $7,500");
    laserPower.setColor(Color::White);
    laserPower.setCharacterSize(28);

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
    window.draw(weapons);
    window.draw(powers);
    window.draw(life);
    window.draw(exitStore);

    if(level>=2){
        window.draw(dual);
    }

    if(level>=3){
        window.draw(tri);
        window.draw(laserPower);
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

    if(Keyboard::isKeyPressed(Keyboard::I) && p->money>=7500 && canBuy && level>=3){
        p->money-=7500;
        p->laserStrength++;

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
    title.setPosition(width/2-title.getGlobalBounds().width/2, title.getGlobalBounds().height);

    weapons.setPosition(width/4-weapons.getGlobalBounds().width/2, height/10+weapons.getGlobalBounds().height);

    dual.setPosition(width/4-dual.getGlobalBounds().width/2,height/10+dual.getGlobalBounds().height*3);
    tri.setPosition(width/4-tri.getGlobalBounds().width/2,height/10+tri.getGlobalBounds().height*4);
    shieldPurge.setPosition(width/4-shieldPurge.getGlobalBounds().width/2,height/10+shieldPurge.getGlobalBounds().height*5);

    powers.setPosition(width*3/4-powers.getGlobalBounds().width/2,height/10+powers.getGlobalBounds().height);

    life.setPosition(width*3/4-life.getGlobalBounds().width/2, height/10+life.getGlobalBounds().height*3);
    laserPower.setPosition(width*3/4-laserPower.getGlobalBounds().width/2,height/10+laserPower.getGlobalBounds().height*4);

    exitStore.setPosition(width/2-(exitStore.getGlobalBounds().width/2),height/10+exitStore.getGlobalBounds().height*10);
    return;
}
