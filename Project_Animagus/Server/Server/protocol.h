#pragma once

namespace Protocol {

    enum class PlayerType : uint16
    {
        NONE,
        TIGER,
        RAM,
        MONKEY,
        KOALA,
    };

    enum class SkillType : uint16
    {
        HEAL,           // 힐
        ENERGYWAVE,     // 에너지파
        BLACKHOLE,      // 블랙홀
        SILENCE,        // 침묵
        GLACIALPATH,    // 빙판길
        FIREBALL,       // 화염구
        INVISIBILITY,   // 은신
        TELEPORT,       // 텔레포트
        SHIELD,         // 방어막
        LIFEDRAIN,      // 흡혈
        SHOCKWAVE,      // 기폭발
        THUNDERSTRIKE,  // 천둥
    };

    enum class StatusType : uint16
    {
        Health,         // 체력
        AttackDamage,   // 공격력
        MovementSpeed,  // 이동속도
        AbilityHaste,   // 스킬가속
    };

    enum class PlayerState : uint16
    {
        MOVE_STATE_NONE,
        MOVE_STATE_IDLE,
        MOVE_STATE_RUN,
        MOVE_STATE_JUMP,
        MOVE_STATE_STUN,
        MOVE_STATE_HIT,
        MOVE_STATE_SKILL,
        MOVE_STATE_DEAD,
    };

    enum class PacketID : uint16
    {
        DCS_TEST,
        CS_LOGIN,
        CS_ENTER_GAME,
        CS_LEAVE,
        SC_ENTER_GAME,
        SC_LEAVE,
        CS_SPAWN,
        SC_SPAWN,
        CS_MOVE,
        CS_SELECT,
        CS_USING_SKILL,
    };


    struct PlayerInfo
    {
        uint16 player_id;
        float x, y, z;
        float rotation;
        PlayerType player_type;
        PlayerState player_state;
    };

//#pragma pack(push, 1)
//    struct PacketHeader {
//        PacketID id;
//        uint16 size;
//
//        PacketHeader(PacketID pktID) : id(pktID), size(0) {}
//    };
//#pragma pack(pop)


    /* 패킷이름 뒤에 _PKT 붙이고 반드시 패킷ID 넣어주기 */
    /* 패킷 헤더는 PacketHandler::MakeSendBuffer 에서 버퍼 앞에 붙여서 나감*/
    /* 가변 길이 데이터는 담지 않기 (ex: vector, string) */
#pragma pack (push, 1)
    struct DCS_TEST_PKT
    {
        uint16 player_id;
        //std::string msg;
        char msg[128];
        int8 len;
    };
    struct CS_LOGIN_PKT
    {
        int16 player_id;
        //std::string name;
    };
    struct CS_ENTER_GAME_PKT
    {
        uint16 room_id;
    };
    struct CS_LEAVE_PKT
    {
        uint16 player_id;
        uint16 room_id;
    };
    struct SC_ENTER_GAME_PKT
    {
        uint16 player_id;
        float x, y, z;
        float rotation;
    };
    struct SC_LEAVE_PKT
    {   // 로비로 보내기
    };
    struct SC_SPAWN_PKT
    {
        uint16 player_id;
        PlayerType p_type;
        float x, y, z;
        float rotation;
    };
    struct CS_MOVE_PKT
    {
        PlayerInfo player_info;
    };
    struct CS_SELECT_CHARACTER_PKT
    {
        int16 player_id;
        PlayerType p_type;
    };
    struct CS_USING_SKILL_PKT
    {
        int16 player_id;
        SkillType s_type;
        float x, y, z;      // 위치
        float vx, vy, vz;   // 방향 벡터
        
    };
#pragma pack(pop)

}
