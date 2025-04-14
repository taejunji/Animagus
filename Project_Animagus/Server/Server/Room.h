#pragma once


class Room : public std::enable_shared_from_this<Room>
{
public:
    bool Enter(PlayerRef player);
    bool Leave(uint16 playerID);
    void Broadcast(SendBufferRef sendBuffer, uint16 execptID);

public:
    bool HandleEnterPlayer(PlayerRef player);
    bool HandleLeavePlayer(PlayerRef player);
    bool HandleMoveLocked(Protocol::CS_MOVE_PKT& pkt);
    bool HandleSkillLocked(Protocol::CS_USING_SKILL_PKT& pkt);
    bool HandleEnterAIPlayer(Protocol::CS_AI_ENTER_PKT& pkt);
    bool HandleAIMoveLocked(Protocol::CS_AI_MOVE_PKT& pkt, const uint16 ownerID);


public:
    uint16 GetPlayerCount() { return m_playerCount; }   // 사람 수 받을 때 동기화 작업 필요

private:
    std::mutex m_mutex;

    uint16 m_roomID;
    uint16 m_playerCount;
    std::unordered_map<uint16/*player_id*/, PlayerRef> m_players; // thread safe 한 자료구조로?
    std::unordered_map<uint16/*ai_id*/, AIPlayerRef> m_aiPlayers;

    PlayerRef m_hostPlayer = nullptr;
};

extern RoomRef GRoom;       // 배열 or 벡터로 관리해 여러 게임룸을 생성
