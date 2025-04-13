#pragma once
#include "protocol.h"

class AIPlayer
{
public:
    AIPlayer(float x, float y, float z, float rotation)
        : x(x), y(y), z(z), rotation(rotation) {}

public:
    uint16  aiID;
    float   x, y, z;
    float   rotation;   // y축 회전각
    Protocol::PlayerType type = Protocol::PlayerType::RAM;     // TODO : type 지정



    std::atomic<std::weak_ptr<Room>> room;
};

