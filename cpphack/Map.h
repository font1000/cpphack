#pragma once
#include <vector>
#include <random>

enum class TileType {
    Floor,
    Wall,
    StairsDown
};

struct Room {
    int x, y, width, height;

    Room(int _x, int _y, int _w, int _h) : x(_x), y(_y), width(_w), height(_h) {}

    int centerX() const { return x + width / 2; }
    int centerY() const { return y + height / 2; }

    bool intersects(const Room& other) const {
        return x < other.x + other.width &&
            x + width > other.x &&
            y < other.y + other.height &&
            y + height > other.y;
    }
};

class Map {
private:
    int width, height;
    std::vector<std::vector<TileType>> tiles;
    std::vector<Room> rooms;
    std::mt19937 rng;

    void createRoom(const Room& room);
    void createHorizontalTunnel(int x1, int x2, int y);
    void createVerticalTunnel(int y1, int y2, int x);

public:
    Map(int w, int h);

    void generate(int floor);
    bool isWalkable(int x, int y) const;
    TileType getTile(int x, int y) const;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

    char getTileChar(int x, int y) const;

    // プレイヤーの初期位置を最初の部屋の中心に
    void getStartPosition(int& outX, int& outY) const;
};