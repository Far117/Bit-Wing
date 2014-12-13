#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Smoke.h"

float randomFloat(float, float);

using namespace sf;
using namespace std;

extern const int width;
extern const int height;


class Explosion{
public:
    float x,y;
    vector<Smoke> particles;
    vector<Smoke> stillParticles;
    vector<Vector2f> directions;

    Explosion(float x2, float y2){
        for(int x=0;x<(rand()%10)+3;x++){
            particles.push_back(Smoke(x2,y2,rand() % 20, false));
            directions.push_back(Vector2f(randomFloat(-10.5,10.5),randomFloat(-10.5,10.5)));
        }
    }

    void update(){

    }

    void draw(RenderWindow &window, Time &time){
        for(int x=0;x<particles.size();x++){
            particles[x].travel(directions[x],time);
            particles[x].travel(Vector2f(0,3),time);
            particles[x].update();

            if(rand()%2==0){
                float newParticleX=particles[x].graphic.getPosition().x;
                float newParticleY=particles[x].graphic.getPosition().y;
                stillParticles.push_back(Smoke(newParticleX, newParticleY,
                                               particles[x].graphic.getGlobalBounds().width,
                                               true));
            }

            if(particles[x].bounds<=0){
                particles.erase(particles.begin()+x);
                directions.erase(directions.begin()+x);
            }
        }

        for(int x=0;x<stillParticles.size();x++){
            stillParticles[x].travel(Vector2f(0,3),time);
            stillParticles[x].update();
            if(stillParticles[x].bounds<=0){
                stillParticles.erase(stillParticles.begin()+x);
            }
        }


        for(int x=0;x<particles.size();x++){
            particles[x].draw(window);
        }
        for(int x=0;x<stillParticles.size();x++){
            stillParticles[x].draw(window);
        }
    }
};

#endif // EXPLOSION_H
