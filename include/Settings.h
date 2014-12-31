#ifndef SETTINGS_H
#define SETTINGS_H


class Settings{
public:
    bool fullscreen;
    bool explosions;
    int explosionLevel;

    void init();
    void check();

    Settings();
};

#endif // SETTINGS_H
