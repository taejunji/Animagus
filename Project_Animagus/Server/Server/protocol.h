#pragma once

namespace Protocol {

    constexpr char MAX_NAME_LEN = 30;
    constexpr char MAX_CHAT_LEN = 50;

    constexpr char LOGIN_ERR = 0;
    constexpr char LOGIN_USING = 1;
    constexpr char LOGIN_NOEX = 2;
    constexpr char LOGIN_BUSY = 3;

    enum class PlayerType : uint16
    {
        NONE,
        //TIGER,
        RAM,
        MONKEY,
        KOALA,
        FOX,
        SLOTH,
        ELEPHANT,
        RACCOON,
        DEER,
        COW,
        UNICORN,
        ZEBRA,
        DONKEY,

        COUNT
    };

    enum class SkillType : uint16
    {
        NONE,
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
        BOUNCE,         // 탱탱볼
        MAGICMISSILE,   // 유도미사일
        SMOKE,          // 연막
        STUN,           // 기절
        RADIAL,         // 방사형
        CHANGE,         // 초동역학위치변환기
        HASTE,          // 헤이스트

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
        CS_SIGN_UP,
        CS_ENTER_ROOM,
        CS_START_GAME,
        CS_ENTER_GAME,
        CS_LEAVE,
        CS_SPAWN,
        CS_MOVE,
        CS_SELECT_CHARACTER,
        CS_USING_SKILL,
        CS_AI_ENTER,
        CS_AI_MOVE,
        CS_AI_USING_SKILL,
        CS_DAMAGE,
        CS_TIME_OVER,
        CS_SKILL_CHANGE,
        CS_ROUND_END,
        CS_ROUND_INIT,

        SC_LOGIN_FAIL,
        SC_UR_HOST,
        SC_START_GAME,
        SC_ENTER_GAME,
        SC_AI_SPAWN,
        SC_LEAVE,
        SC_SPAWN,
        SC_SPAWN_ITEM,
        SC_UPDATE_HP,
        SC_GAME_INIT,
        SC_ROUND_END,
        SC_ROUND_INIT,
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
    /* 패킷 헤더는 PacketHandler::MakeSendBuffer 에서 버퍼 앞에 붙여서 나감 */
    /* 가변 길이 데이터는 담지 않기 (ex: vector, string) */
#pragma pack (push, 1)
    struct PlayerInfo
    {
        uint16 player_id;
        float x, y, z;
        float rotation;
        PlayerState player_state;
        float speed_2d;
        float speed_z;
    };

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
        char login_id[MAX_NAME_LEN];
        char login_pwd[MAX_NAME_LEN];
        uint8 id_len;
        uint8 pwd_len;
    };
    struct CS_SIGN_UP_PKT
    {
        char sign_id[MAX_NAME_LEN];
        char sign_pwd[MAX_NAME_LEN];
        char sign_name[MAX_NAME_LEN];
        uint8 id_len;
        uint8 pwd_len;
        uint8 name_len;
    };
    struct CS_ENTER_ROOM_PKT
    {
        uint16 room_id;
    };
    struct CS_START_GAME_PKT
    {
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
    struct CS_AI_ENTER_PKT
    {
        uint16 player_id;
        uint16 room_id;
        uint16 ai_id;
        PlayerType p_type;
        float x, y, z;
        float rotation;
    };
    struct CS_MOVE_PKT
    {
        PlayerInfo player_info;
        uint64 server_time;
    };
    struct CS_SELECT_CHARACTER_PKT
    {
        int16 player_id;
        PlayerType p_type;
    };
    struct CS_USING_SKILL_PKT
    {
        uint16 player_id;
        uint16 room_id;
        SkillType s_type;
        float x, y, z;          // 위치, 근데, 필요한가? 이미 클라이언트에서도 player_id 로 해당 플레이어의 위치 접근 가능하지 않나?
        float pitch, yaw, roll;       
    };
    struct CS_AI_MOVE_PKT
    {
        PlayerInfo player_info;
    };
    struct CS_AI_USING_SKILL_PKT
    {
        uint16 ai_id;
        uint16 room_id;
        SkillType s_type;
        float x, y, z;
        float pitch, yaw, roll;
    };
    struct CS_DAMAGE_PKT
    {
        uint16 player_id;
        uint16 room_id;
        float hp;
        bool isAlive;
    };
    struct CS_ROUND_END_PKT     // round over by time out (host)
    {
    };
    //struct CS_TIME_OVER_PKT
    //{
    //};
    struct CS_ROUND_INIT_PKT    // host request init round
    {
    };
    struct CS_SKILL_CHANGE_PKT
    {
        uint16 hit_player_id;
        uint16 shooter_player_id;
    };

    struct SC_LOGIN_FAIL
    {
        int8 reason;
    };
    struct SC_LOGIN_SUCC
    {
        char player_name[MAX_NAME_LEN];
        // 더 넣을거 있나?
    };
    struct SC_UR_HOST_PKT
    {
    };
    struct SC_START_GAME_PKT
    {
    };
    struct SC_ENTER_GAME_PKT
    {
        uint16 player_id;
        //float x, y, z;
        //float rotation;
        bool host;
        int16 spawn_index;
        uint64 server_time;
    };
    struct SC_AI_SPAWN_PKT
    {
        int16 player_count;
        PlayerType types[8];
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
        //char name[MAX_NAME_LEN];
    };
    struct SC_SPAWN_ITEM_PKT
    {
        uint16 zone_index;
        uint16 item_count;
        char spawn_index[30];
        char item_level[30];
    };
    struct SC_UPDATE_HP_PKT
    {
        uint16 player_id;
        uint16 room_id;
        float hp;
        bool isAlive;
    };
    struct SC_GAME_INIT_PKT
    {
        char name[8][MAX_NAME_LEN]; // sorted by own score
        char ranking[8];            // 일단 id 넘기는 중
        char score[8];
    };
    struct SC_ROUND_END_PKT
    {
        char name[8][MAX_NAME_LEN]; // sorted by own score
        char ranking[8];            // 일단 id 넘기는 중
        char score[8];
    };
    struct SC_ROUND_INIT_PKT
    {
    };

#pragma pack(pop)

}
