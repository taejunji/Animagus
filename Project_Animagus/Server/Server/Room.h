#pragma once


class Room
{
public:
    void Enter(PlayerRef player);
    void Leave(PlayerRef player);
    void Broadcast(SendBufferRef sendBuffer);

    uint16 GetPlayerCount() { return m_playerCount; }   // 사람 수 받을 때 동기화 작업 필요

private:
    std::mutex m_mutex;

    uint16 m_roomId;
    uint16 m_playerCount;
    std::map<uint16/*player_id*/, PlayerRef> m_players; // thread safe 한 자료구조로?
};

