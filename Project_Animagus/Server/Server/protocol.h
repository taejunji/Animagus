#pragma once


enum class PlayerType
{
    Tiger,
    Ram,
    Monkey,
    Koala,
};

enum class SkillType
{
    Heal,           // 힐
    EnergyWave,     // 에너지파
    BlackHole,      // 블랙홀
    Silence,        // 침묵
    GlacialPath,    // 빙판길
    Fireball,       // 화염구
    Invisibility,   // 은신
    Teleport,       // 텔레포트
    Shield,         // 방어막
    LifeDrain,      // 흡혈
    Shockwave,      // 기폭발
    Thunderstrike,  // 천둥
};

enum class StatusType
{
    Health,         // 체력
    AttackDamage,   // 공격력
    MovementSpeed,  // 이동속도
    AbilityHaste,   // 스킬가속
};


#pragma pack (push, 1)
struct CS_MOVE_PKT
{
    unsigned char size;
    unsigned char type;
    int	p_id;
    float Max_speed;
    float x, y, z;
    float vx, vy, vz;
    float yaw, pitch;
};
struct CS_SELECT_CHARACTER
{
    unsigned char size;
    unsigned char type;
    int id;
    float x, y, z;
    PlayerType p_type;
};
#pragma pack(pop)
