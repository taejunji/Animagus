#pragma once
#include "protocol.h"


class PlayerFactory
{
public:
    static PlayerRef CreatePlayer(SessionRef session);

private:
    static std::atomic<int64> s_idGenerator;    // TODO : 각 Room 별로 idGenerator 를 관리 및 설정
};


enum class PlayerRoomState {INGAME, WAITING, LOBBY};
class Player
{
public:
    Player() : x(0), y(0), z(-1000.0f), rotation(0) {}
    Player(float x, float y, float z, float rotation)
        : x(x), y(y), z(z), rotation(rotation) {
    }

public:
    uint16  playerID = 0;
    PlayerRoomState player_state = PlayerRoomState::LOBBY;
    std::mutex s_mutex;

    int16    playerHP = 100;
    float   x, y, z;
    float   rotation;   // y축 회전각
    bool    isAlive = true;
    std::string name;

    std::atomic<int32> powerUpLevel = 0;
    
    Protocol::PlayerType type = Protocol::PlayerType::KOALA;     // TODO : 선택한 캐릭터로 보여지게


    std::weak_ptr<Session> ownerSession;
    std::atomic<std::weak_ptr<Room>> room;
};



class AIPlayer : public Player
{
public:
    AIPlayer(float x, float y, float z, float rotation)
        : Player(x, y, z, rotation) {
    }

public:
    uint16 aiID = 0;
};


