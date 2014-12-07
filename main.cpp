#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

const static int width=800, height=640;
const static bool keyboardControl=true;

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

class TextEntity{
    Font font;
    Text text;

    int textSize;

    TextEntity(float x, float y, int charSize){

    }

};

class Star{
public:
    float x,y;
    float speed;
    float bounds;

    bool noSprite;
    RectangleShape placeHolder;

    Star(){
        x=rand() % width;
        y=rand() % height;
        bounds=(rand() % 4)+1;
        noSprite=true;
        speed=bounds/25;

        if(noSprite){
            placeHolder.setSize(Vector2f(bounds,bounds));
            placeHolder.setFillColor(Color::White);
            placeHolder.setPosition(Vector2f(x,y));
        }
    }

    void travel(){
        x=placeHolder.getPosition().x;
        y=placeHolder.getPosition().y;

        if(y>=height){
            bounds=(rand() % 4)+1;
            placeHolder.setSize(Vector2f(bounds,bounds));
            speed=bounds/25;

            placeHolder.setPosition(rand() % width, 0);
        } else {
            placeHolder.move(0,speed);
        }
    }

    void draw(RenderWindow &window){
        if(noSprite){
            window.draw(placeHolder);
        }
    }
};

class Asteroid{
public:
    float x,y;
    float speed;
    float bounds;
    int life;
    bool noSprite;

    RectangleShape placeHolder;

    Asteroid(int x2, int y2){
        x=x2;
        y=y2;
    }
};

class Smoke{
public:

    float speed;
    float bounds;
    bool still;

    enum directions{LEFT, RIGHT};
    int direction;

    RectangleShape graphic;

    Smoke(float x, float y, float area, bool s){
        still=s;
        bounds=area;

        graphic.setSize(Vector2f(bounds,bounds));
        graphic.setFillColor(Color(255,0,0));
        graphic.setOutlineColor(Color(127,0,0));
        graphic.setOutlineThickness(2);
        graphic.setPosition(Vector2f(x,y));

        if(rand() % 2==0){
            direction=RIGHT;
        } else {
            direction=LEFT;
        }
    }

    void update(){
        if(still){
            bounds=bounds-0.3;
            graphic.move(0.3,0.3);
        } else {
            bounds=bounds-0.6;
            graphic.move(0.6,0.6);
            if(direction==LEFT){
                graphic.rotate(-5);
            } else {
                graphic.rotate(5);
            }
        }

        graphic.setSize(Vector2f(bounds,bounds));
    }

    void draw(RenderWindow &window){
        window.draw(graphic);
    }

    void travel(Vector2f dir){
        graphic.move(dir.x,dir.y);
    }
};

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

    void draw(RenderWindow &window){
        for(int x=0;x<particles.size();x++){
            particles[x].travel(directions[x]);
            particles[x].travel(Vector2f(0,3));
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
            stillParticles[x].travel(Vector2f(0,3));
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

class Missile{
public:
    float x,y;
    bool hostile;
    float speed;
    float bounds;
    bool noSprite;
    RectangleShape placeHolder;

    Missile(float x2, float y2, bool h){
        noSprite=true;
        x=x2;
        y=y2;
        hostile=h;
        speed=1.5;
        bounds=13;

        if(noSprite){
            placeHolder.setSize(Vector2f(bounds/2,bounds));
            placeHolder.setPosition(Vector2f(x,y));

            placeHolder.setFillColor(Color::White);
            placeHolder.setOutlineThickness(3.5);

            if(hostile){
                placeHolder.setOutlineColor(Color::Red);
            } else {
                placeHolder.setOutlineColor(Color::Blue);
            }
        }
    }

    void draw(RenderWindow &window){
        if (noSprite){
            window.draw(placeHolder);
        }
    }

    void travel(){
        x=placeHolder.getPosition().x;
        y=placeHolder.getPosition().y;
        if (hostile){
            if(noSprite){
                placeHolder.move(0,speed);
            } else {
                y+=speed;
            }
        } else {
            if(noSprite){
                placeHolder.move(0,-speed);
            } else {
                y-=speed;
            }
        }
    }
};

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

    void logic(){
        x=parts[BODY].getPosition().x;
        y=parts[BODY].getPosition().y;

        if(ai==CHARGER){
            parts[BODY].move(0,speed);
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
                parts[BODY].move(speed/2,speed);
            }else {
                parts[BODY].move(-speed/2,speed);
            }
        } else if(ai=STRAFER){
            if(x<=0){
                parts[BODY].move(0,speed*50);
                direction=RIGHT;
            }

            if(x>=width-placeHolder.getGlobalBounds().width){
                parts[BODY].move(0,speed*10);
                direction=LEFT;
            }

            if(direction==LEFT){
                parts[BODY].move(-speed*2,0);
            } else if(direction==RIGHT){
                parts[BODY].move(speed*2,0);
            }
        }

        if(y>(height+(rand() % height)+(height/4))){
            x=(rand() % (int)(width-placeHolder.getGlobalBounds().width))+placeHolder.getGlobalBounds().width;
            y=0;
            parts[BODY].setPosition(Vector2f(x,y));
        }

        syncParts();
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

    RectangleShape placeHolder;

    RectangleShape body;
    RectangleShape rightWing;
    RectangleShape leftWing;
    RectangleShape rightThrust;
    RectangleShape leftThrust;
    RectangleShape cockpit;

    RectangleShape topHUD;
    RectangleShape clearedHUD;

    enum partNames{BODY=0,RIGHTWING=1,LEFTWING=2};
    enum accessoryNames{RIGHTTHRUST=0,LEFTTHRUST=1, COCKPIT=2};
    enum hudNames{TOPHUD=0, CLEAREDHUD=1};

    vector<RectangleShape> parts;
    vector<RectangleShape> accessories;
    vector<RectangleShape> huds;

    Player(float x2, float y2){
        x=x2;
        y=y2;
        bounds=30;
        outline=2;

        noSprite=false;
        canFire=true;
        speed=1;
        lives=3;
        score=0;

        flameAnimationTimer=10;
        flameGrow=false;

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

        parts.push_back(body);
        parts.push_back(rightWing);
        parts.push_back(leftWing);

        accessories.push_back(rightThrust);
        accessories.push_back(leftThrust);
        accessories.push_back(cockpit);

        huds.push_back(topHUD);
        huds.push_back(clearedHUD);

        //0=body
        //1=rightWing



        if (noSprite){
            placeHolder.setSize(Vector2f(bounds,bounds));
            placeHolder.setPosition(width/2-placeHolder.getGlobalBounds().width, height-placeHolder.getGlobalBounds().height-10);
            //placeHolder.setPosition(width/2,height/2);
            placeHolder.setFillColor(Color::Green);
        }
    }

    void strafe(RenderWindow &window){

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

            if((Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::D)) &&
                    parts[LEFTWING].getPosition().y+parts[LEFTWING].getGlobalBounds().height<height){
                y2=speed;
            }
            if((Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
               && parts[RIGHTWING].getPosition().x+parts[RIGHTWING].getGlobalBounds().width<width){
                //if (noSprite){placeHolder.move(Vector2f(speed,0));}
                x2=speed;
            } else if ((Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
                       && parts[LEFTWING].getPosition().x>0){
                //if (noSprite){placeHolder.move(Vector2f(-speed,0));}
                x2=-speed;
            }
            if(noSprite){
                placeHolder.move(Vector2f(x2,y2));
            } else {
                parts[BODY].move(Vector2f(x2,y2));
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
    }

    void drawHuds(RenderWindow &window){
        for(int x=0;x<huds.size();x++){
            window.draw(huds[x]);
        }
    }

    void resetPosition(){
        if(noSprite){
            placeHolder.setPosition(width/2-placeHolder.getGlobalBounds().width, height-placeHolder.getGlobalBounds().height);
        } else {
            parts[BODY].setPosition(width/2-parts[BODY].getGlobalBounds().width, height-parts[BODY].getGlobalBounds().height);
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

class Game{
public:

    static const int FRAMES_PER_SECOND = 60;
    static const int MAX_FRAMESKIP = 10;

    RenderWindow window;
    Font font;

    Text fps;
    Text levelSplash;
    Text lost;
    Text score;
    Text lives;
    Text levelCounter;
    Text enemyCounter;
    Text sectorCounter;
    Text quit;
    int quitTimer;

    string sectorName;

    SoundBuffer goodLaserBuffer;
    SoundBuffer badLaserBuffer;
    SoundBuffer explosionBuffer;

    Sound goodLaser;
    Sound badLaser;
    Sound explosion;

    Music introBG;
    Music bg1;
    Music bg2;
    Music bg3;
    Music bg4;
    Music bg5;
    Music bg6;
    int SONGS;

    vector<Music> backgroundSongs;
    int lastPlayedSong;

    enum states {INTRO, PLAYING, LEVEL, GAME_LOST};
    int gameState;
    int level;
    int enemiesLeft;
    int spawnsLeft;
    int levelSplashTime;
    bool replay;

    Time time;
    Int32 updateTime;

    unsigned int playerScore;
    unsigned int playerLives;

    vector<Enemy> enemies;
    vector<Missile> missiles;
    vector<Player> player;
    vector<Star> stars;
    vector<Explosion> explosions;

    Game(){}

    bool init(){
        VideoMode videoMode(width, height);
        window.create(videoMode, "Bit Wing");
        window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(FRAMES_PER_SECOND);

        setup();
        return true;
    }

    int exec(){
        Clock renderClock, updateClock;

        while(window.isOpen()){
            time=renderClock.getElapsedTime();
            float fFps=1000000/time.asMicroseconds();

            std::stringstream s;
            s<<"fps: "<<fFps;
            //fps.setString(s.str());

            renderClock.restart();

            const Int64 frameTime = 1000000/FRAMES_PER_SECOND;

            Clock c;
            Time t=c.getElapsedTime();
            Int64 nextFrameTime=t.asMicroseconds()+frameTime;

            int loops = 0;
            while( t.asMicroseconds() < nextFrameTime && loops < MAX_FRAMESKIP){
                processEvents();
                checkCollisions();
                updateTime=updateClock.restart().asMilliseconds();
                update();
                t=c.getElapsedTime();
                loops++;
            }

            timer(); //called 60 times per second
            display();
        }

        return EXIT_SUCCESS;
    }

private:
    void processEvents(){
        Event event;
        while (window.pollEvent(event)){
            if (event.type == Event::Closed){
                replay=false;
                window.close();
            } else if ((event.type == Event::KeyPressed) && (gameState == INTRO)){
                gameState=PLAYING;
            } else if(gameState==GAME_LOST && Keyboard::isKeyPressed(Keyboard::Return)){
                replay=true;
                window.close();
            } else if(gameState==GAME_LOST && Keyboard::isKeyPressed(Keyboard::Escape)){
                replay=false;
                window.close();
            }
        }
    }

    void update(){
        if(gameState!=PLAYING && gameState!=LEVEL){
            return;
        }

        for(int x=0;x<missiles.size();x++){
            missiles[x].travel();
        }

        for(int x=0;x<missiles.size();x++){
            if(missiles[x].y<0 || missiles[x].y>height+20){
                missiles.erase(missiles.begin()+(x));
            }
        }

        for(int x=0;x<stars.size();x++){
            stars[x].travel();
        }

        player[0].strafe(window);
        checkFire();

        for(int x=0;x<enemies.size();x++){
            enemies[x].logic();
        }

        if(enemiesLeft<=0){
            level_up();
        }

        for(int x=0;x<explosions.size();x++){
            explosions[x].update();
            if(explosions[x].particles.size()==0 && explosions[x].stillParticles.size()==0){
                explosions.erase(explosions.begin()+x);
            }
        }

        checkMusic();
    }

    void setup(){
        font.loadFromFile("bin/LCD_Solid.ttf");
        levelSplash.setFont(font);
        levelSplash.setColor(Color::White);
        levelSplash.setCharacterSize(50);

        lost.setFont(font);
        lost.setColor(Color::White);
        lost.setCharacterSize(100);
        lost.setString("Game Over");
        lost.setPosition(Vector2f(width/2-lost.getGlobalBounds().width/2,height/2-lost.getGlobalBounds().height/2));

        quit.setFont(font);
        quit.setColor(Color::Red);
        quit.setCharacterSize(20);
        quit.setString("Press Enter to Restart\nPress Escape to Quit...");
        quit.setPosition(width/2-quit.getGlobalBounds().width/2,height-quit.getGlobalBounds().height);
        quitTimer=180;
        replay=false;

        score.setFont(font);
        score.setColor(Color::Yellow);
        score.setCharacterSize(25);
        score.setString("Score: 0");
        score.setPosition(0,score.getGlobalBounds().height);

        lives.setFont(font);
        lives.setColor(Color::Yellow);
        lives.setCharacterSize(25);
        lives.setString("Lives: 3");
        lives.setPosition(width-lives.getGlobalBounds().width,lives.getGlobalBounds().height);

        levelCounter.setFont(font);
        levelCounter.setColor(Color::Red);
        levelCounter.setCharacterSize(20);
        sectorName=randomSector();
        levelCounter.setString(sectorName.c_str());
        levelCounter.setPosition(width/2-levelCounter.getGlobalBounds().width/2,levelCounter.getGlobalBounds().height);

        sectorCounter.setFont(font);
        sectorCounter.setColor(Color::Red);
        sectorCounter.setCharacterSize(20);
        sectorCounter.setString("Sectors Cleared: ");
        sectorCounter.setPosition(0,height-(sectorCounter.getGlobalBounds().height));

        enemyCounter.setFont(font);
        enemyCounter.setColor(Color::Red);
        enemyCounter.setCharacterSize(20);
        enemyCounter.setString("Enemies In Sector: 0");
        enemyCounter.setPosition(width/2-enemyCounter.getGlobalBounds().width/2,levelCounter.getGlobalBounds().height*2.5);

        explosionBuffer.loadFromFile("bin/164855__dayofdagon__bit-bomber2.ogg");
        goodLaserBuffer.loadFromFile("bin/laser.wav");

        goodLaser.setBuffer(goodLaserBuffer);
        badLaser.setBuffer(goodLaserBuffer); //temporary
        explosion.setBuffer(explosionBuffer);


        introBG.openFromFile("bin/And_-_03_-_Start_Havok.ogg");
        bg1.openFromFile("bin/Rolemusic_-_05_-_Death_on_the_battlefield.ogg");
        bg2.openFromFile("bin/Rolemusic_-_Savage_Steel_Fun_Club.ogg");
        bg3.openFromFile("bin/Skip_-_03_-_Planet_Cruizer.ogg");
        bg4.openFromFile("bin/Azureflux_-_01_-_Final_Sector.ogg");
        bg5.openFromFile("bin/Azureflux_-_03_-_Superbyte.ogg");
        bg6.openFromFile("bin/Azureflux_-_05_-_Expedition.ogg");



        SONGS=6;
        lastPlayedSong=rand() % SONGS;

        player.push_back(Player(width/2,height/2));
        for(int x=0;x<20;x++){
            stars.push_back(Star());
        }

        level=0;
        enemiesLeft=0;
        spawnsLeft=0;

        level_up();
        gameState=LEVEL;
    }

    void display(){
        window.clear(Color::Black);

        for(int x=0;x<stars.size();x++){
            stars[x].draw(window);
        }

        for(int x=0;x<missiles.size();x++){
            missiles[x].draw(window);
        }

        for(int x=0;x<enemies.size();x++){
            enemies[x].draw(window);
        }

        player[0].draw(window);

        for(int x=0;x<explosions.size();x++){
            explosions[x].draw(window);
        }

        player[0].drawHuds(window);

        if(gameState==LEVEL){
            window.draw(levelSplash);
        }

        if(gameState==GAME_LOST){
            window.draw(lost);
        }

        if(gameState==PLAYING){
            stringstream s;
            //s << "Level: " << level;
            levelCounter.setString(sectorName.c_str());

            //s.str("");
            //s.clear();

            s << "Enemies In Sector: " << enemiesLeft;
            enemyCounter.setString(s.str());

            window.draw(levelCounter);
            window.draw(enemyCounter);
            window.draw(sectorCounter);
        }

        stringstream s;
        s << "Score: " << player[0].score;
        score.setString(s.str());
        window.draw(score);

        s.str("");
        s.clear();
        s << "Lives: " << player[0].lives;
        lives.setString(s.str());
        window.draw(lives);

        if(gameState==GAME_LOST && quitTimer>0){
            quitTimer--;
        } else if (gameState==GAME_LOST && quitTimer<=0){
            window.draw(quit);
        }

        window.display();
    }

    void checkCollisions(){
        //missile with players/enemies
        //enemies with player

        for(int x=0;x<missiles.size();x++){
            if(player[0].noSprite){
                if(missiles[x].hostile){
                    if(intersects(missiles[x].placeHolder,player[0].placeHolder)){
                        explosions.push_back(Explosion(player[0].placeHolder.getPosition().x,
                                                        player[0].placeHolder.getPosition().y));
                        player[0].lives--;
                        player[0].resetPosition();
                        missiles.erase(missiles.begin()+(x));
                        explosion.play();
                    }
                }
            } else if(!missiles[x].hostile){
                for(int y=0;y<enemies.size();y++){
                    if(enemies[y].noSprite){
                        if(intersects(missiles[x].placeHolder,enemies[y].placeHolder)){
                            explosions.push_back(Explosion(enemies[y].placeHolder.getPosition().x,
                                                           enemies[y].placeHolder.getPosition().y));
                            missiles.erase(missiles.begin()+x);
                            enemies.erase(enemies.begin()+y);
                            enemiesLeft--;
                            player[0].score+=100;
                            explosion.play();
                        }
                    } else {
                        for(int z=0;z<enemies[y].parts.size();z++){
                            if(intersects(missiles[x].placeHolder,enemies[y].parts[z])){
                                explosions.push_back(Explosion(enemies[y].parts[0].getPosition().x,
                                                               enemies[y].parts[0].getPosition().y));
                                missiles.erase(missiles.begin()+x);
                                enemies.erase(enemies.begin()+y);
                                enemiesLeft--;
                                player[0].score+=100;
                                explosion.play();
                                break;
                            }
                        }
                    }
                }
            }
        }

        if(player[0].noSprite){
            for(int x=0;x<enemies.size();x++){
                if(intersects(enemies[x].placeHolder,player[0].placeHolder)){
                    explosions.push_back(Explosion(player[0].placeHolder.getPosition().x,
                                                    player[0].placeHolder.getPosition().y));
                    player[0].lives--;
                    player[0].resetPosition();
                    //enemies[x].kill=true;
                    enemies.erase(enemies.begin()+x);
                    enemiesLeft--;
                    explosion.play();
                }
            }
        }

        if(!player[0].noSprite){
            for(int x=0;x<player[0].parts.size();x++){
                for(int y=0;y<missiles.size();y++){
                    if(missiles[y].hostile){
                        if(intersects(player[0].parts[x],missiles[y].placeHolder)){
                            explosions.push_back(Explosion(player[0].parts[0].getPosition().x,
                                                            player[0].parts[0].getPosition().y));
                            player[0].lives--;
                            player[0].resetPosition();

                            missiles.erase(missiles.begin()+y);
                            explosion.play();
                        }
                    }
                }
            }

            for(int x=0;x<player[0].parts.size();x++){
                for(int y=0; y<enemies.size();y++){
                    if(enemies[y].noSprite){
                        if(intersects(enemies[y].placeHolder,player[0].parts[x])){
                            explosions.push_back(Explosion(player[0].parts[0].getPosition().x,
                                                            player[0].parts[0].getPosition().y));
                            player[0].lives--;
                            player[0].resetPosition();
                            //enemies[x].kill=true;
                            enemies.erase(enemies.begin()+y);
                            enemiesLeft--;
                            explosion.play();
                        }
                    } else {
                        for(int z=0;z<enemies[y].parts.size();z++){
                            if(intersects(enemies[y].parts[z],player[0].parts[x])){
                                explosions.push_back(Explosion(player[0].parts[0].getPosition().x,
                                                                player[0].parts[0].getPosition().y));
                                player[0].lives--;
                                player[0].resetPosition();
                                //enemies[x].kill=true;
                                enemies.erase(enemies.begin()+y);
                                enemiesLeft--;
                                explosion.play();
                            }
                        }
                    }
                }
            }
        }

        if(player[0].lives<=0){
            gameState=GAME_LOST;
        }
    }

    void checkFire(){
        if(!Keyboard::isKeyPressed(Keyboard::Space)){
            player[0].canFire=true;
        }

        if(Keyboard::isKeyPressed(Keyboard::Space) && player[0].canFire){
            missiles.push_back(Missile(player[0].parts[0].getPosition().x+(player[0].parts[0].getGlobalBounds().width/2)-5,
                                player[0].parts[0].getPosition().y, false));
            player[0].canFire=false;
            goodLaser.play();
        }
    }

    void spawnEnemies(){
        if(level<=2){
            if(enemies.size()<level && spawnsLeft>0){
                enemies.push_back( Enemy((rand() % (width-30))+30,0,1));
                spawnsLeft--;
            }
        }else{
            if (spawnsLeft>=3 && rand() % 10==0){ //fleet of 3 horizontal
                int lineStart=rand()% (width-(30*4));

                int type=rand()%3;
                if(type==0){
                    type=3;
                }else{
                    type=1;
                }

                for(int x=0;x<3;x++){
                    enemies.push_back( Enemy((float)lineStart,0,type));
                    lineStart+=45;
                    spawnsLeft--;
                }
            } else if (level>=5 && spawnsLeft>=6 && rand() % 20 ==0){ //fleet of 6 horizontal
                int lineStart=rand()% (width-(30*8));

                int type=rand()%3;
                if(type==0){
                    type=3;
                }else{
                    type=1;
                }

                for(int x=0;x<6;x++){
                    enemies.push_back( Enemy((float)lineStart,0,type));
                    lineStart+=45;
                    spawnsLeft--;
                }
            } else if(level>=7 && rand() % 20==0 && spawnsLeft>=9){ //fleet of 9 cube
                int lineStart=rand()% (width-(30*4));
                int positionReset=lineStart;

                int type=rand()%3;
                if(type==0){
                    type=2;
                }else{
                    type=1;
                }

                for(int y=30*4; y>0;y-=45){
                    for(int x=0;x<3;x++){
                        enemies.push_back( Enemy((float)lineStart,(float) y,type));
                        lineStart+=45;
                        spawnsLeft--;
                    }
                    lineStart=positionReset;
                }

            } else if(enemies.size()<(level*level+(rand() % level*2))-(level/2) && spawnsLeft>0){

                if(rand() % 3==0){ //spawn weavers
                    enemies.push_back( Enemy((rand() % (width-30))+30,0,2));
                    spawnsLeft--;
                } else if(rand() % 5==0){ //spawn strafers
                    enemies.push_back( Enemy((rand() % (width-30))+30,0,3));
                    spawnsLeft--;
                } else {
                    enemies.push_back( Enemy((rand() % (width-30))+30,0,1));
                    spawnsLeft--;
                }
            }
        }
    }

    void level_up(){
        level++;
        sectorName=randomSector();
        stringstream s;

        s << "Entering " << sectorName;
        levelSplash.setString(s.str());
        levelSplash.setPosition(Vector2f(width/2-levelSplash.getGlobalBounds().width/2,
                                    height/2-levelSplash.getGlobalBounds().height/2));

        s.str("");
        s.clear();

        s << "Sectors Cleared: " << level-1;
        sectorCounter.setString(s.str());

        enemiesLeft=level*5+(level*level/2);
        spawnsLeft=enemiesLeft;
        gameState=LEVEL;
        levelSplashTime=60*3; //5 seconds
    }

    void timer(){ //called 10 times per second
        if(gameState==LEVEL){
            levelSplashTime--;
            if (levelSplashTime<=0){
                gameState=PLAYING;
            }
        }

        if(gameState==PLAYING && rand() % 60*10==0){
            spawnEnemies();
        }

        if(gameState==PLAYING){
            for(int x=0;x<enemies.size();x++){
                if(enemies[x].shouldFire()){
                    if(enemies[x].noSprite){
                        missiles.push_back(Missile(enemies[x].placeHolder.getPosition().x+15-5,
                                                   enemies[x].placeHolder.getPosition().y+30,true));
                        badLaser.play();
                    } else {
                        missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                       enemies[x].parts[0].getPosition().y+15,true));
                        badLaser.play();
                    }
                }
            }
        }
    }

    void checkMusic(){
        bool newSong=false;
        if(lastPlayedSong==0){
            if(bg1.getStatus()==Sound::Stopped){
                newSong=true;
            }
        } else if(lastPlayedSong==1){
            if(bg2.getStatus()==Sound::Stopped){
                newSong=true;
            }
        } else if(lastPlayedSong==2){
            if(bg3.getStatus()==Sound::Stopped){
                newSong=true;
            }
        } else if(lastPlayedSong==3){
            if(bg4.getStatus()==Sound::Stopped){
                newSong=true;
            }
        } else if(lastPlayedSong==4){
            if(bg5.getStatus()==Sound::Stopped){
                newSong=true;
            }
        } else if(lastPlayedSong==5){
            if(bg6.getStatus()==Sound::Stopped){
                newSong=true;
            }
        }

        if(newSong){
            int x=lastPlayedSong;
            while(x==lastPlayedSong){
                x=rand() % SONGS;
            }

            lastPlayedSong=x;

            switch(x){
                case 0:
                    bg1.play();
                    break;
                case 1:
                    bg2.play();
                    break;
                case 2:
                    bg3.play();
                    break;
                case 3:
                    bg4.play();
                    break;
                case 4:
                    bg5.play();
                    break;
                case 5:
                    bg6.play();
                    break;
            }
        }
    }

    /*
    bool shouldReplay(){
        while(true){
            if(Keyboard::isKeyPressed(Keyboard::Enter)){
                return true;
            }else if(Keyboard::isKeyPressed(Keyboard::Escape)){
                return false;
            }
        }
    }
    */
};

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
