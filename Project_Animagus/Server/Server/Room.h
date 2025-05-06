#pragma once


class Room : public std::enable_shared_from_this<Room>
{
public:
    Room();
    ~Room() = default;

public:
    bool Enter(PlayerRef player);
    bool Leave(uint16 playerID);
    void Broadcast(SendBufferRef sendBuffer, uint16 execptID);

public:
    bool HandleEnterPlayer(PlayerRef player);
    bool HandleEnterGame();
    bool HandleStartGame(PlayerRef player);
    bool HandleLeavePlayer(PlayerRef player);
    bool HandleMoveLocked(Protocol::CS_MOVE_PKT& pkt);
    bool HandleSkillLocked(Protocol::CS_USING_SKILL_PKT& pkt);
    bool HandleEnterAIPlayer(Protocol::CS_AI_ENTER_PKT& pkt);
    bool HandleAIMoveLocked(Protocol::CS_AI_MOVE_PKT& pkt, const uint16 ownerID);
    bool HandleAISkillLocked(Protocol::CS_AI_USING_SKILL_PKT& pkt, const uint16 ownerID);
    bool HandleDamageLocked(Protocol::CS_DAMAGE_PKT& pkt, const uint16 ownerID);
    bool HandleTimeOverLocked(Protocol::CS_TIME_OVER_PKT& pkt);

public:
    uint16 GetPlayerCount() { return m_playerCount; }   // 사람 수 받을 때 동기화 작업 필요

public:
    void InitializeGame();
    void InitItemInfo();

public:
    uint16 m_roomID;
    std::atomic<uint16> m_roundCount = 1;
    std::atomic<uint16> m_indexGen = 0;
    std::atomic<uint16> m_loadingOverCount = 0;

    uint64 m_gameStartTickCount = 0;

private:
    std::mutex m_mutex;

    std::atomic<uint16> m_playerCount;
    std::unordered_map<uint16/*player_id*/, PlayerRef> m_players; // thread safe 한 자료구조로?
    std::unordered_map<uint16/*ai_id*/, AIPlayerRef> m_aiPlayers;
    uint16 m_maxPlayerCount = 2;

    PlayerRef m_hostPlayer = nullptr;
    std::array<Protocol::SC_SPAWN_ITEM_PKT, 3> m_itemInfo;
};

extern std::array<RoomRef, ROOM_COUNT> GRoom;       // 배열 or 벡터로 관리해 여러 게임룸을 생성
