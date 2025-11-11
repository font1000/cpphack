#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "Display.h"
#include <iostream>
#include <fstream>
#include <conio.h>

Game::Game() : currentFloor(1), isRunning(true) {
    map = std::make_unique<Map>(80, 24);
    int startX, startY;
    map->getStartPosition(startX, startY);
    player = std::make_unique<Player>(startX, startY);
}

Game::~Game() {}

void Game::run() {
    showTitle();

    while (isRunning) {
        render();
        processInput();
        update();

        // 毎ターン自動セーブ
        saveGame();
    }
}

void Game::showTitle() {
    Display::clear();
    Display::println("=== NetHack風ローグライク ===");
    Display::println("");

    // セーブデータの確認
    std::ifstream saveFile("savegame.dat", std::ios::binary);
    bool hasSaveData = saveFile.good();
    saveFile.close();

    if (hasSaveData) {
        Display::println("1. コンティニュー");
        Display::println("2. ニューゲーム（セーブデータ削除）");
        Display::println("");
        Display::print("選択: ");

        char choice = Display::getKey();
        if (choice == '1') {
            loadGame();
        }
        else if (choice == '2') {
            remove("savegame.dat");
            newGame();
        }
        else {
            newGame();
        }
    }
    else {
        Display::println("セーブデータがありません。");
        Display::println("");
        Display::println("何かキーを押してスタート...");
        Display::waitForKey();
        newGame();
    }
}

void Game::newGame() {
    map = std::make_unique<Map>(80, 24);
    currentFloor = 1;
    map->generate(currentFloor);

    // プレイヤーを最初の部屋に配置
    int startX, startY;
    map->getStartPosition(startX, startY);
    player = std::make_unique<Player>(startX, startY);
}

bool Game::loadGame() {
    std::ifstream file("savegame.dat", std::ios::binary);
    if (!file) return false;

    // 簡易的なロード（後で拡張）
    int px, py, hp, maxHp, level;
    file.read(reinterpret_cast<char*>(&px), sizeof(px));
    file.read(reinterpret_cast<char*>(&py), sizeof(py));
    file.read(reinterpret_cast<char*>(&hp), sizeof(hp));
    file.read(reinterpret_cast<char*>(&maxHp), sizeof(maxHp));
    file.read(reinterpret_cast<char*>(&level), sizeof(level));
    file.read(reinterpret_cast<char*>(&currentFloor), sizeof(currentFloor));

    file.close();

    map = std::make_unique<Map>(80, 24);
    map->generate(currentFloor);
    player = std::make_unique<Player>(px, py);

    return true;
}

void Game::saveGame() {
    std::ofstream file("savegame.dat", std::ios::binary);
    if (!file) return;

    // 簡易的なセーブ（後で拡張）
    int px = player->getX();
    int py = player->getY();
    int hp = player->getHP();
    int maxHp = player->getMaxHP();
    int level = player->getLevel();

    file.write(reinterpret_cast<const char*>(&px), sizeof(px));
    file.write(reinterpret_cast<const char*>(&py), sizeof(py));
    file.write(reinterpret_cast<const char*>(&hp), sizeof(hp));
    file.write(reinterpret_cast<const char*>(&maxHp), sizeof(maxHp));
    file.write(reinterpret_cast<const char*>(&level), sizeof(level));
    file.write(reinterpret_cast<const char*>(&currentFloor), sizeof(currentFloor));

    file.close();
}

void Game::processInput() {
    if (!_kbhit()) return;

    char key = _getch();
    key = tolower(key);

    int dx = 0, dy = 0;
    bool validMove = true;

    switch (key) {
    case 'q': dx = -1; dy = -1; break; // 左上
    case 'w': dx = 0; dy = -1; break; // 上
    case 'e': dx = 1; dy = -1; break; // 右上
    case 'a': dx = -1; dy = 0; break; // 左
    case 's': player->rest(); return;   // 足踏み
    case 'd': dx = 1; dy = 0; break; // 右
    case 'z': dx = -1; dy = 1; break; // 左下
    case 'x': dx = 0; dy = 1; break; // 下
    case 'c': dx = 1; dy = 1; break; // 右下
    case 27:  quit(); return;           // ESC
    default: validMove = false;
    }

    if (validMove && (dx != 0 || dy != 0)) {
        int newX = player->getX() + dx;
        int newY = player->getY() + dy;

        if (map->isWalkable(newX, newY)) {
            player->move(dx, dy);

            // 階段チェック
            if (map->getTile(newX, newY) == TileType::StairsDown) {
                nextFloor();
            }
        }
    }
}

void Game::update() {
    // プレイヤーが死んだかチェック
    if (!player->isAlive()) {
        playerDeath();
    }
}

void Game::render() {
    Display::clear();

    // マップ描画
    for (int y = 0; y < map->getHeight(); y++) {
        for (int x = 0; x < map->getWidth(); x++) {
            if (x == player->getX() && y == player->getY()) {
                std::cout << '@';
            }
            else {
                std::cout << map->getTileChar(x, y);
            }
        }
        std::cout << '\n';
    }

    // UI情報
    std::cout << "\n";
    std::cout << "Floor: " << currentFloor << " | ";
    std::cout << "HP: " << player->getHP() << "/" << player->getMaxHP() << " | ";
    std::cout << "Lv: " << player->getLevel() << "\n";
    std::cout << "ATK: " << player->getAttack() << " | ";
    std::cout << "DEF: " << player->getDefense() << "\n";
    std::cout << "\n";
    std::cout << "操作: QWEASDZXC=移動 S=足踏み ESC=終了\n";
}

void Game::nextFloor() {
    currentFloor++;
    map->generate(currentFloor);

    // 次のフロアの最初の部屋に配置
    int startX, startY;
    map->getStartPosition(startX, startY);
    player->setPosition(startX, startY);
}

void Game::playerDeath() {
    Display::clear();
    Display::println("あなたは死んでしまった...");
    Display::println("");
    Display::println("何かキーを押してタイトルへ...");
    Display::waitForKey();

    remove("savegame.dat");
    newGame();
}