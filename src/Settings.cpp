#include "Settings.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

void Settings::check(){
    ifstream file("Settings.ini");

    if(file.good()){
        file.close();
        return;
    }

    ofstream out;

    out.open("Settings.ini");

    out << "Fullscreen:1\n";
    out << "Explosions:1\n";
    out << "Explosion Level (1=Firecracker, 9=Nukes):9\n";

    out.close();
    return;
}

void Settings::init(){
    ifstream in;
    string temp;

    vector<string> lines;

    check();

    in.open("Settings.ini");

    while(getline(in,temp)){
        lines.push_back(temp);
    }

    in.close();

    for(int setting=0;setting<lines.size();setting++){
        bool read=false;
        for(int character=0;character<lines[setting].length();character++){
            if(lines[setting][character]==':'){
                read=true;
                character++;
            }

            if(read){
                if(setting==0){
                    if(lines[setting][character]=='1'){
                        fullscreen=true;
                    } else {
                        fullscreen=false;
                    }
                } else if (setting==1){
                    if(lines[setting][character]=='1'){
                        explosions=true;
                    }else {
                        explosions=false;
                    }
                } else if (setting==2){
                    char c=lines[setting][character];
                    explosionLevel=(c-'0');
                }
            }
        }
    }
}

Settings::Settings(){
    fullscreen=true;
    explosions=true;
    explosionLevel=9;
    init();
}
