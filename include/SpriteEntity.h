#ifndef SPRITEENTITY_H
#define SPRITEENTITY_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

extern const int width;
extern const int height;


class SpriteEntity{
public:

    Texture texture;
    Sprite sprite;
    RectangleShape collision;

    SpriteEntity(){};

    void init(float x, float y, string graphic){
        texture.loadFromFile(graphic.c_str());
        sprite.setTexture(texture);

        sprite.setPosition(x,y);
        collision.setSize(Vector2f(sprite.getGlobalBounds().width,
                                   sprite.getGlobalBounds().height));
        collision.setPosition(sprite.getPosition().x,sprite.getPosition().y);
    }

    void draw(RenderWindow &window){
        window.draw(sprite);
    }

    void travel(float x, float y){
        sprite.move(x,y);
        collision.move(x,y);
    }

    void warp(float x, float y){
        sprite.setPosition(x,y);
        collision.setPosition(x,y);
    }

    void rescale(float x, float y){
        sprite.setScale(x,y);
        collision.setScale(x,y);
    }
};

#endif // SPRITEENTITY_H
