#pragma once


class Room
{
public:
    void Enter(PlayerRef player);
    void Leave(PlayerRef player);
    void Broadcast(SendBufferRef sendBuffer);

private:
    std::mutex m_mutex;

    uint16 m_roomId;
    std::map<uint16/*player_id*/, PlayerRef> m_players; // thread safe 한 자료구조로?

};

