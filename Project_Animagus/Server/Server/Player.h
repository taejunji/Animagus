#pragma once


class Player
{
public:

    uint64  playerID = 0;
    int8    playerHP = 100;
    
    Protocol::PlayerType type = Protocol::PlayerType::RAM;     // TODO : 선택한 캐릭터로 보여지게


    SessionRef ownerSession;
};

