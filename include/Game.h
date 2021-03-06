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
#include "Settings.h"
#include "Store.h"



using namespace std;
using namespace sf;

extern int width, height;
extern Settings setting;

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
    Text gotoStore;
    int quitTimer;

    Text killCount;
    Text money;

    Store store;

    //Settings setting;

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

    enum states {INTRO, PLAYING, LEVEL, GAME_LOST, STORE, PRESTORE};
    int gameState;
    int level;
    int enemiesLeft;
    int spawnsLeft;
    int levelSplashTime;
    int storeCounter;
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
        if(setting.fullscreen){
            //width=VideoMode::getDesktopMode().width;
            //height=VideoMode::getDesktopMode().height;
            VideoMode videoMode(width, height);
            window.create(videoMode, "Bit Wing", Style::Fullscreen);
        } else {
            //width=800;
            //height=640;
            VideoMode videoMode(width,height);
            window.create(videoMode, "Bit Wing");
        }
        //window.setVerticalSyncEnabled(true);
        window.setFramerateLimit(FRAMES_PER_SECOND);
        window.setMouseCursorVisible(false);

        setup();
        return true;
    }

    int exec(){
        Clock renderClock, updateClock;

        while(window.isOpen()){
            time=renderClock.getElapsedTime();
            //float fFps=1000000/time.asMicroseconds();

            //std::stringstream s;
            //s<<"fps: "<<fFps;
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
            } else if(gameState==PRESTORE && Keyboard::isKeyPressed(Keyboard::S)){
                gameState=STORE;
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

        if(enemiesLeft<=0 && gameState==PLAYING){
            player[0].sectorClearedPop(level);
            gameState=PRESTORE;
            //level_up();
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

        for(int x=0;x<explosions.size();x++){
            //explosions[x].update();
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

        money.setFont(font);
        money.setColor(Color::Green);
        money.setCharacterSize(25);
        money.setString("$000");
        money.setPosition(0,money.getGlobalBounds().height);

        killCount.setFont(font);
        killCount.setColor(Color::Red);
        killCount.setCharacterSize(25);
        killCount.setString("Enemies Killed: 0");
        killCount.setPosition(0,money.getGlobalBounds().height+killCount.getGlobalBounds().height);

        gotoStore.setFont(font);
        gotoStore.setColor(Color::White);
        gotoStore.setCharacterSize(24);
        gotoStore.setString("Press S to go to the store...");
        gotoStore.setPosition(width/2-gotoStore.getGlobalBounds().width/2,height-gotoStore.getGlobalBounds().height);

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

        player.push_back(Player(width/2,height/2, font));
        store.link(player[0]);
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
        storeCounter=60*2;
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
            explosions[x].draw(window);
        }

        if(gameState==PLAYING || gameState==LEVEL || gameState==GAME_LOST || gameState==PRESTORE){
            player[0].drawHuds(window);
        }

        if(/*gameState==PLAYING ||*/ gameState==STORE){
            stringstream s;
            s << "$" << player[0].money;
            money.setString(s.str());

            window.draw(money);
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
            levelCounter.setString(sectorName.c_str());

            s << "Enemies In Sector: " << enemiesLeft;
            enemyCounter.setString(s.str());

            s.str("");
            s.clear();

            s << "Enemies Killed: " << player[0].kills;
            killCount.setString(s.str());

            //window.draw(levelCounter);
            //window.draw(enemyCounter);
            //window.draw(sectorCounter);
            //window.draw(killCount);
        }

        if(/*gameState==PLAYING || gameState==LEVEL || gameState==GAME_LOST || */gameState==STORE){
            stringstream s;
            s << "Lives: " << player[0].lives;
            lives.setString(s.str());
            window.draw(lives);
        }

        if(gameState==STORE){
            store.logic(window, level);
            if(store.quit){
                store.quit=false;
                level_up();
            }
        }

        if(gameState==PRESTORE){
            window.draw(gotoStore);
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
                    if(setting.explosions){
                        explosions.push_back(Explosion(missiles[x].placeHolder.getPosition().x,
                                                       missiles[x].placeHolder.getPosition().y));
                    }
                    missiles.erase(missiles.begin()+x);
                    explosion.play();

                    if(destroyTitle){
                        titleDestructionTimer-=300;
                    }
                }
            }
        }

        for(int m=0;m<missiles.size();m++){
            if(missiles[m].hostile){
                for(int p=0;p<player[0].parts.size();p++){
                    if(intersects(missiles[m].placeHolder,player[0].parts[p])){
                        if(setting.explosions){
                            explosions.push_back(Explosion(player[0].parts[p].getPosition().x,
                                                            player[0].parts[p].getPosition().y));
                        }
                        player[0].lives--;
                        player[0].deathPop();
                        player[0].resetPosition();
                        missiles.erase(missiles.begin()+m);
                        explosion.play();

                        if(player[0].lives<=0){
                            break;
                        }

                        for(int x=0;x<enemies.size();x++){
                            enemies[x].resetPosition();
                        }

                        missiles.clear();
                        break;
                    }
                }
            } else {
                for(int e=0;e<enemies.size();e++){
                    for(int p=0;p<enemies[e].parts.size();p++){
                        if(intersects(missiles[m].placeHolder,enemies[e].parts[p]) && enemies[e].shields<=0){
                            if(setting.explosions){
                                explosions.push_back(Explosion(enemies[e].parts[p].getPosition().x,
                                                               enemies[e].parts[p].getPosition().y));
                            }
                            player[0].money+=100;
                            player[0].kills++;
                            player[0].moneyPop(enemies[e].parts[p].getPosition().x,enemies[e].parts[p].getPosition().y);
                            player[0].killPop();

                            enemiesLeft--;
                            player[0].enemiesRemainingPop(enemies[e].parts[p].getPosition().x,
                                                          enemies[e].parts[p].getPosition().y+25,
                                                          enemiesLeft);

                            missiles.erase(missiles.begin()+m);
                            enemies.erase(enemies.begin()+e);


                            explosion.play();
                            break;
                        } else if(intersects(missiles[m].placeHolder,enemies[e].parts[p])){
                            enemies[e].hit();
                            missiles.erase(missiles.begin()+m);
                            //insert dull metal hit sound
                        }
                    }
                }
            }
        }

        for(int e=0;e<enemies.size();e++){
            for(int ep=0;ep<enemies[e].parts.size();ep++){
                for(int pp=0;pp<player[0].parts.size();pp++){
                    if(intersects(enemies[e].parts[ep],player[0].parts[pp])){
                        if(setting.explosions){
                            explosions.push_back(Explosion(player[0].parts[pp].getPosition().x,
                                                            player[0].parts[pp].getPosition().y));
                            explosions.push_back(Explosion(enemies[e].parts[ep].getPosition().x,
                                                            enemies[e].parts[ep].getPosition().y));
                        }
                        player[0].lives--;
                        player[0].deathPop();
                        player[0].resetPosition();
                        //enemies[x].kill=true;

                        enemies.erase(enemies.begin()+e);
                        enemiesLeft--;
                        explosion.play();

                        if(player[0].lives<=0){
                            break;
                        }
                        for(int x=0;x<enemies.size();x++){
                            enemies[x].resetPosition();
                        }

                        missiles.clear();
                        break;
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
            if(player[0].currentWeapon==1){ //single shot
                for(int x=0;x<player[0].laserStrength;x++){
                    missiles.push_back(Missile(player[0].parts[0].getPosition().x+
                                               (player[0].parts[0].getGlobalBounds().width/2)-5,
                                        player[0].parts[0].getPosition().y, false,1));
                }

                goodLaser.play();
                player[0].canFire=false;
            }else if(player[0].currentWeapon==2 && player[0].dualAmmo>0){ //double shot
                for(int x=0;x<player[0].laserStrength;x++){
                    missiles.push_back(Missile(player[0].parts[0].getPosition().x+
                                               (player[0].parts[0].getGlobalBounds().width/2)-5,
                                        player[0].parts[0].getPosition().y, false,2));
                    missiles.push_back(Missile(player[0].parts[0].getPosition().x+
                                               (player[0].parts[0].getGlobalBounds().width/2)-5,
                                        player[0].parts[0].getPosition().y, false,3));
                }

                player[0].dualAmmo--;

                goodLaser.play();
                player[0].canFire=false;
            } else if(player[0].currentWeapon==3 && player[0].triAmmo>0){

                for(int x=0;x<player[0].laserStrength;x++){
                    missiles.push_back(Missile(player[0].parts[0].getPosition().x+
                                               (player[0].parts[0].getGlobalBounds().width/2)-5,
                                        player[0].parts[0].getPosition().y, false,2));
                    missiles.push_back(Missile(player[0].parts[0].getPosition().x+
                                               (player[0].parts[0].getGlobalBounds().width/2)-5,
                                        player[0].parts[0].getPosition().y, false,3));
                    missiles.push_back(Missile(player[0].parts[0].getPosition().x+
                                               (player[0].parts[0].getGlobalBounds().width/2)-5,
                                        player[0].parts[0].getPosition().y, false,1));
                }

                player[0].triAmmo--;

                goodLaser.play();
                player[0].canFire=false;
            } else if(player[0].currentWeapon==4 && player[0].shieldAmmo>0){
                int maxX=player[0].parts[0].getPosition().x+45;
                int startY=player[0].parts[0].getPosition().y-15;

                for(int y=0;y<player[0].laserStrength;y++){
                    for(int x=player[0].parts[0].getPosition().x-45; x<maxX;x+=15){
                        missiles.push_back(Missile(x,startY,false,1));
                    }
                }

                player[0].shieldAmmo--;

                goodLaser.play();
                player[0].canFire=false;
            }
        }
    }

    void spawnEnemies(){
        if(level<=2){
            if(enemies.size()<level && spawnsLeft>0){
                enemies.push_back( Enemy((rand() % (width-30))+30,0,1, level));
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
                    enemies.push_back( Enemy((float)lineStart,0,type, level));
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
                    enemies.push_back( Enemy((float)lineStart,0,type, level));
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
                        enemies.push_back( Enemy((float)lineStart,(float) y,type, level));
                        lineStart+=45;
                        spawnsLeft--;
                    }
                    lineStart=positionReset;
                }

            } else if(enemies.size()<(level*level+(rand() % level*2))-(level/2) && spawnsLeft>0){

                if(rand() % 3==0){ //spawn weavers
                    enemies.push_back( Enemy((rand() % (width-30))+30,0,2, level));
                    spawnsLeft--;
                } else if(rand() % 5==0){ //spawn strafers
                    enemies.push_back( Enemy((rand() % (width-30))+30,0,3, level));
                    spawnsLeft--;
                } else {
                    enemies.push_back( Enemy((rand() % (width-30))+30,0,1, level));
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
                                                   enemies[x].placeHolder.getPosition().y+30,true,1));
                        badLaser.play();
                    } else {
                        if(enemies[x].fireType==1){
                            missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                           enemies[x].parts[0].getPosition().y+15,true,1));
                        } else if(enemies[x].fireType==2){
                            missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                           enemies[x].parts[0].getPosition().y+15,true,2));
                            missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                           enemies[x].parts[0].getPosition().y+15,true,3));
                        } else if(enemies[x].fireType==3){
                            missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                           enemies[x].parts[0].getPosition().y+15,true,1));
                            missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                           enemies[x].parts[0].getPosition().y+15,true,2));
                            missiles.push_back(Missile(enemies[x].parts[0].getPosition().x+10-5,
                                                           enemies[x].parts[0].getPosition().y+15,true,3));
                        } else if(enemies[x].fireType==4){
                            int startX=enemies[x].parts[0].getPosition().x-45;
                            int startY=enemies[x].parts[0].getPosition().y;

                            int maxX=enemies[x].parts[0].getPosition().x+45;

                            for(int m=startX;m<maxX;m+=15){
                                missiles.push_back(Missile(m,startY,true,1));
                            }
                        }

                        badLaser.play();
                    }
                }
            }
        }

        if(gameState==PRESTORE){
            storeCounter--;
            if(storeCounter<0){
                storeCounter=60*2;
                level_up();
            }
        }

        if(gameState==PLAYING || gameState==LEVEL || gameState==INTRO){
            for(int x=0;x<explosions.size();x++){
                explosions[x].update(time);
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

            if(setting.explosions){
                explosions.push_back(Explosion(randomFloat(minX,maxX), randomFloat(minY,maxY)));
            }
            explosion.play();
        }

        if(titleDestructionTimer<=0){
            float minX=title.sprite.getPosition().x;
            float maxX=minX+title.sprite.getGlobalBounds().width;

            float minY=title.sprite.getPosition().y;
            float maxY=minY+title.sprite.getGlobalBounds().height;
            for(int x=0;x<(rand() % 5) + 5;x++){
                if(setting.explosions){
                    explosions.push_back(Explosion(randomFloat(minX,maxX), randomFloat(minY,maxY)));
                }
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
