#pragma once


class Room : public std::enable_shared_from_this<Room>
{
public:
    bool Enter(PlayerRef player);
    bool Leave(uint16 playerID);
    void Broadcast(SendBufferRef sendBuffer, uint16 execptID);

    uint16 GetPlayerCount() { return m_playerCount; }   // 사람 수 받을 때 동기화 작업 필요

private:
    std::mutex m_mutex;

    uint16 m_roomID;
    uint16 m_playerCount;
    std::unordered_map<uint16/*player_id*/, PlayerRef> m_players; // thread safe 한 자료구조로?
};

extern RoomRef GRoom;       // 배열 or 벡터로 관리해 여러 게임룸을 생성
