#pragma once
#include "protocol.h"

class Player
{
public:
    uint16  playerID = 0;
    int8    playerHP = 100;
    float   x, y, z;
    float   rotation;   // y축 회전각
    
    Protocol::PlayerType type = Protocol::PlayerType::RAM;     // TODO : 선택한 캐릭터로 보여지게


    std::weak_ptr<Session> ownerSession;
    std::atomic<std::weak_ptr<Room>> room;
};

class PlayerFactory
{
public:
    static PlayerRef CreatePlayer(SessionRef session);

private:
    static std::atomic<int64> s_idGenerator;    // TODO : 각 Room 별로 idGenerator 를 관리 및 설정
};

