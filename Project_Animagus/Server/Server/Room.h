#pragma once


class Room : public std::enable_shared_from_this<Room>
{
public:
    Room();
    ~Room() = default;
 
public:
    bool Enter(PlayerRef player);
    bool Leave(uint16 playerID);
    void Broadcast(SendBufferRef sendBuffer, uint16 execptID, bool state_check = false);

public:
    bool HandleEnterPlayer(PlayerRef player);
    bool HandleEnterGame();
    bool HandleStartGame(PlayerRef player);
    bool HandleLeavePlayer(PlayerRef player);
    bool HandleMoveLocked(Protocol::CS_MOVE_PKT& pkt);
    bool HandleSkillLocked(Protocol::CS_USING_SKILL_PKT& pkt);
    bool HandleEnterAIPlayer(const Protocol::CS_AI_ENTER_PKT& pkt);
    bool HandleAIMoveLocked(const Protocol::CS_AI_MOVE_PKT& pkt, const uint16 ownerID);
    bool HandleAISkillLocked(const Protocol::CS_AI_USING_SKILL_PKT& pkt, const uint16 ownerID);
    bool HandleDamageLocked(const Protocol::CS_DAMAGE_PKT& pkt, const uint16 ownerID);
    bool HandleRoundEndLocked(const Protocol::CS_ROUND_END_PKT& pkt);
    bool HandleRoundInitLocked(const Protocol::CS_ROUND_INIT_PKT& pkt);
    bool HandleHitChangeSkill(const Protocol::CS_SKILL_CHANGE_PKT& pkt);
    bool HandleJumpEffect(Protocol::CS_JUMP_EFT_PKT& pkt);
    bool HandleItemPickedUp(Protocol::CS_ITEM_PICK_PKT& pkt, const uint16 ownerID);

public:
    uint16 GetPlayerCount() { return m_playerCount; }   // 사람 수 받을 때 동기화 작업 필요
    bool IsValid() { return m_isValid; }

public:
    void InitializeGame();
    void InitItemInfo();
    void InitAiTypes();
    void InitializeRoom();

public:
    uint16 m_roomID;
    std::atomic_bool    m_isValid = true;
    std::atomic<uint16> m_roundCount = 1;
    std::atomic<uint16> m_indexGen = 0;
    std::atomic<uint16> m_loadingOverCount = 0;

    uint64 m_gameStartTickCount = 0;

    std::unordered_map<uint16/*id*/, uint16/*score*/> accumRanking;
    std::array<Protocol::PlayerType, 8> aiPlayerTypes;

private:
    std::mutex m_mutex;

    std::atomic<uint16> m_playerCount = 0;
    std::unordered_map<uint16/*player_id*/, PlayerRef>  m_players; // thread safe 한 자료구조로?
    std::unordered_map<uint16/*ai_id*/, AIPlayerRef>    m_aiPlayers;
    std::unordered_map<uint16, std::string>             m_playerNames;
    std::atomic<uint16>                                 m_aiNameGen = 0;
    uint16 m_maxPlayerCount = 8;
    std::atomic<uint16> m_nowPlayerCount = 0;
    std::atomic<uint16> m_alivePlayerCount = 8;

    PlayerRef m_hostPlayer = nullptr;
    std::array<Protocol::SC_SPAWN_ITEM_PKT, 3> m_itemInfo;

    std::stack<uint16>                                  m_deathPlayer;
};

extern std::array<RoomRef, ROOM_COUNT> GRoom;       // 배열 or 벡터로 관리해 여러 게임룸을 생성
extern std::array<uint8, 8> scoreBoard;

