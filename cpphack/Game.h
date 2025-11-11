#pragma once
#include <string>
#include <memory>

class Player;
class Map;

class Game {
private:
    std::unique_ptr<Player> player;
    std::unique_ptr<Map> map;
    int currentFloor;
    bool isRunning;
    std::string message; // メッセージ表示用

public:
    Game();
    ~Game();

    void run();
    void showTitle();
    bool loadGame();
    void saveGame();
    void newGame();

    void processInput();
    void update();
    void render();

    void nextFloor();
    void playerDeath();

    bool isGameRunning() const { return isRunning; }
    void quit() { isRunning = false; }
};