#ifndef GAME_H
#define GAME_H

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>

#include "SpriteEntity.h"
#include "Star.h"
#include "Smoke.h"
#include "Explosion.h"
#include "Missile.h"
#include "Enemy.h"
#include "Player.h"
#include "Asteroid.h"
#include "TextEntity.h"


using namespace std;
using namespace sf;

extern const int width, height;

float randomFloat(float, float);
char randomLetter();
string randomSector();
bool intersects(const RectangleShape &, const RectangleShape &);

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

    bool destroyTitle;
    int titleDestructionTimer;
    int titleAnimationTimer;

    Time time;
    Int32 updateTime;

    unsigned int playerScore;
    unsigned int playerLives;

    vector<Enemy> enemies;
    vector<Missile> missiles;
    vector<Player> player;
    vector<Star> stars;
    vector<Explosion> explosions;

    //shared_ptr<SpriteEntity> title;
    SpriteEntity title;

    Game(){}

    bool init(){
        VideoMode videoMode(width, height);
        window.create(videoMode, "Bit Wing", Style::Fullscreen);
        //window.setVerticalSyncEnabled(true);
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
            } else if(gameState==GAME_LOST && Keyboard::isKeyPressed(Keyboard::Return)){
                replay=true;
                window.close();
            } else if(Keyboard::isKeyPressed(Keyboard::Escape)){
                replay=false;
                window.close();
            }
        }
    }

    void update(){
        if(destroyTitle){
            titleDestructionSequence();
        }

        if(gameState==INTRO){
            titleAnimation();
        }
        if(gameState!=PLAYING && gameState!=LEVEL && gameState!=INTRO){
            return;
        }

        for(int x=0;x<missiles.size();x++){
            missiles[x].travel(time);
        }

        for(int x=0;x<missiles.size();x++){
            if(missiles[x].y<0 || missiles[x].y>height+20){
                missiles.erase(missiles.begin()+(x));
            }
        }

        for(int x=0;x<stars.size();x++){
            stars[x].travel(time);
        }

        player[0].strafe(window, time);
        checkFire();

        for(int x=0;x<enemies.size();x++){
            enemies[x].logic(time);
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
        enemiesLeft=1;
        spawnsLeft=0;

        destroyTitle=false;
        //titleAnimationTimer=50;
        titleDestructionTimer=6000;

        title.init(0,0,"bin/logo.png");
        title.rescale(2.5,2.5);
        title.warp(width/2-title.sprite.getGlobalBounds().width/2,-title.sprite.getGlobalBounds().height);

        //level_up();
        gameState=INTRO;
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
            explosions[x].draw(window,time);
        }

        if(gameState==PLAYING || gameState==LEVEL || gameState==GAME_LOST){
            player[0].drawHuds(window);
        }

        if(gameState==INTRO){
            title.draw(window);
        }

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

        if(gameState==PLAYING || gameState==LEVEL || gameState==GAME_LOST){
            stringstream s;
            s << "Score: " << player[0].score;
            score.setString(s.str());
            window.draw(score);

            s.str("");
            s.clear();
            s << "Lives: " << player[0].lives;
            lives.setString(s.str());
            window.draw(lives);
        }

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

        if(gameState==INTRO){
            for(int x=0;x<missiles.size();x++){
                if(intersects(missiles[x].placeHolder,title.collision)){
                    if(rand() % 10==0){
                            destroyTitle=true;
                    }
                    explosions.push_back(Explosion(missiles[x].placeHolder.getPosition().x,
                                                   missiles[x].placeHolder.getPosition().y));
                    missiles.erase(missiles.begin()+x);
                    explosion.play();
                }
            }
        }

        for(int m=0;m<missiles.size();m++){
            if(missiles[m].hostile){
                for(int p=0;p<player[0].parts.size();p++){
                    if(intersects(missiles[m].placeHolder,player[0].parts[p])){
                        explosions.push_back(Explosion(player[0].parts[p].getPosition().x,
                                                        player[0].parts[p].getPosition().y));
                        player[0].lives--;
                        player[0].resetPosition();
                        missiles.erase(missiles.begin()+m);
                        explosion.play();
                    }
                }
            } else {
                for(int e=0;e<enemies.size();e++){
                    for(int p=0;p<enemies[e].parts.size();p++){
                        if(intersects(missiles[m].placeHolder,enemies[e].parts[p])){
                            explosions.push_back(Explosion(enemies[e].parts[p].getPosition().x,
                                                           enemies[e].parts[p].getPosition().y));
                            missiles.erase(missiles.begin()+m);
                            enemies.erase(enemies.begin()+e);
                            enemiesLeft--;
                            player[0].score+=100;
                            explosion.play();
                            break;
                        }
                    }
                }
            }
        }

        for(int e=0;e<enemies.size();e++){
            for(int ep=0;ep<enemies[e].parts.size();ep++){
                for(int pp=0;pp<player[0].parts.size();pp++){
                    if(intersects(enemies[e].parts[ep],player[0].parts[pp])){
                        explosions.push_back(Explosion(player[0].parts[pp].getPosition().x,
                                                        player[0].parts[pp].getPosition().y));
                        explosions.push_back(Explosion(enemies[e].parts[ep].getPosition().x,
                                                        enemies[e].parts[ep].getPosition().y));
                        player[0].lives--;
                        player[0].resetPosition();
                        //enemies[x].kill=true;
                        enemies.erase(enemies.begin()+e);
                        enemiesLeft--;
                        explosion.play();
                        break;
                    }
                }
            }
        }
        /*
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


        */
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

        if(gameState==INTRO){
            if(introBG.getStatus()==Sound::Stopped){
                introBG.play();
            }
            return;
        }
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

    void titleDestructionSequence(){
        title.travel(randomFloat(-1,1),randomFloat(-1,1));
        titleDestructionTimer--;

        if(introBG.getVolume()>0.3){
            introBG.setVolume(introBG.getVolume()-0.03);
        }

        if(rand() % 200==0){
            float minX=title.sprite.getPosition().x;
            float maxX=minX+title.sprite.getGlobalBounds().width;

            float minY=title.sprite.getPosition().y;
            float maxY=minY+title.sprite.getGlobalBounds().height;

            explosions.push_back(Explosion(randomFloat(minX,maxX), randomFloat(minY,maxY)));
            explosion.play();
        }

        if(titleDestructionTimer<=0){
            float minX=title.sprite.getPosition().x;
            float maxX=minX+title.sprite.getGlobalBounds().width;

            float minY=title.sprite.getPosition().y;
            float maxY=minY+title.sprite.getGlobalBounds().height;
            for(int x=0;x<(rand() % 5) + 5;x++){
                explosions.push_back(Explosion(randomFloat(minX,maxX), randomFloat(minY,maxY)));
                explosion.play();
            }

            destroyTitle=false;
            gameState=LEVEL;
            introBG.stop();
            level_up();
        }
    }

    void titleAnimation(){

        if((title.sprite.getPosition().y+
            title.sprite.getGlobalBounds().height/2)<=height/2){
            title.travel(0,.1);
        }
    }
};

#endif // GAME_H
