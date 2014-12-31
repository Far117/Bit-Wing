#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>
#include <memory>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include "Game.h"


using namespace std;
using namespace sf;
//const int width=VideoMode::getDesktopMode().width;
int width,height;
Settings setting;
//const int width=VideoMode::getDesktopMode().width, height=VideoMode::getDesktopMode().height;
const bool keyboardControl=true;

bool intersects (const RectangleShape &rect1,const RectangleShape &rect2){
    FloatRect r1=rect1.getGlobalBounds();
    FloatRect r2=rect2.getGlobalBounds();
    return r1.intersects(r2);
}

float randomFloat(float a, float b){
    float random=((float) rand())/(float) RAND_MAX;
    float diff=b-a;
    float r=random*diff;
    return a+r;
}

char randomLetter(){
    char c='A'+rand() % 24;
    return c;
}

string randomSector(){
    stringstream s;
    s << "Sector ";
    s << randomLetter() << randomLetter() << randomLetter();
    s << "-";
    s << rand()%10000;
    return s.str();
}

int main(){
    srand(time(NULL));
    bool replay=true;

    while(replay){
        Game game;
        if(!game.init()){
            return EXIT_FAILURE;
        }

        game.exec();

        replay=game.replay;
    }


    //return game.exec();
}
