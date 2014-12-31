#ifndef STORE_H
#define STORE_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Player.h"

using namespace sf;
using namespace std;


class Store{
public:
    Font font;

    Player *p;
    Text title;
    Text life;
    Text exitStore;
    bool quit;


    Store();
    //~Store();

    void logic(RenderWindow &window);
    void link(Player &);
    void init();
};

#endif // STORE_H
