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

    dual.setString("[D]ual Lasers-10 ammunition: 500");
    dual.setFont(font);
    dual.setColor(Color::White);
    dual.setCharacterSize(28);

    exitStore.setFont(font);
    exitStore.setString("[Q]uit");
    exitStore.setColor(Color::Red);
    exitStore.setCharacterSize(28);
}
/*
Store::~Store(){
    delete p;
}
*/

void Store::logic(RenderWindow &window){
    window.draw(title);
    window.draw(life);
    window.draw(dual);
    window.draw(exitStore);

    if(Keyboard::isKeyPressed(Keyboard::L) && p->money>=10000){
        p->lives++;
        p->money-=10000;
    }

    if(Keyboard::isKeyPressed(Keyboard::D) && p->money>=500){
        p->dualAmmo+=10;
        p->money-=500;
    }

    if(Keyboard::isKeyPressed(Keyboard::Q)){
        quit=true;
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
    exitStore.setPosition(width/2-(exitStore.getGlobalBounds().width/2),height/10+exitStore.getGlobalBounds().height*3);
    return;
}
