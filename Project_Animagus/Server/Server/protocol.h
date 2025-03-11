#pragma once

namespace Protocol {

    enum class PlayerType
    {
        NONE,
        TIGER,
        RAM,
        MONKEY,
        KOALA,
    };

    enum class SkillType
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

    enum class StatusType
    {
        Health,         // 체력
        AttackDamage,   // 공격력
        MovementSpeed,  // 이동속도
        AbilityHaste,   // 스킬가속
    };

    enum class PacketID : uint16
    {
        CS_MOVE,
        CS_SELECT,
        CS_LOGIN,
    };

#pragma pack(push, 1)
    struct PacketHeader {
        PacketID id;
        uint16 len;
    };
#pragma pack(pop)


/* 패킷이름 뒤에 _PKT 붙이고 반드시 패킷ID 넣어주기 */
#pragma pack (push, 1)
    struct CS_MOVE_PKT
    {
        static constexpr PacketID pkt_id = PacketID::CS_MOVE;

        unsigned char size;
        unsigned char type;
        int64	p_id;
        float Max_speed;
        float x, y, z;
        float vx, vy, vz;
        float yaw, pitch;
    };
    struct CS_SELECT_CHARACTER_PKT
    {
        static constexpr PacketID pkt_id = PacketID::CS_SELECT;

        BYTE size;
        BYTE type;
        int id;
        float x, y, z;
        PlayerType p_type;
    };
    struct CS_USING_SKILL_PKT
#pragma pack(pop)

}