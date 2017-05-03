#ifndef __ACTOR_DEFINITIONS_H__
#define __ACTOR_DEFINITIONS_H__

#include "Interfaces.h"

#ifndef cond_assert
#define cond_assert(enabled, body) { if (enabled) { assert(body); } else { body; } }
#endif

//
// Helper maps and functions for converting from actor prototype to string and back
//

// TODO: This should be the perfect candidate for code-generating macro

inline std::string ActorPrototypeEnumToString(ActorPrototype actorProto)
{
    static std::map<ActorPrototype, std::string> actorPrototypeEnumToStringMap =
    {
        // Actor enemies
        { ActorPrototype_Level1_Soldier, "ActorPrototype_Level1_Soldier" },
        { ActorPrototype_Level1_Officer, "ActorPrototype_Level1_Officer" },
        { ActorPrototype_Level1_Rat, "ActorPrototype_Level1_Rat" },
        { ActorPrototype_Level2_Soldier, "ActorPrototype_Level2_Soldier" },
        { ActorPrototype_Level2_Officer, "ActorPrototype_Level2_Officer" },
        { ActorPrototype_Level2_PunkRat, "ActorPrototype_Level2_PunkRat" },

        // Elevators
        { ActorPrototype_BaseElevator, "ActorPrototype_BaseElevator" },

        { ActorPrototype_Level1_Elevator, "ActorPrototype_Level1_Elevator" },
        { ActorPrototype_Level2_Elevator, "ActorPrototype_Level2_Elevator" },

        // Toggle pegs
        { ActorPrototype_BaseTogglePeg, "ActorPrototype_BaseTogglePeg" },

        { ActorPrototype_Level1_TogglePeg, "ActorPrototype_Level1_TogglePeg" },
        { ActorPrototype_Level2_TogglePeg, "ActorPrototype_Level2_TogglePeg" },

        // Projectiles
        { ActorPrototype_Level2_CannonBall, "ActorPrototype_Level2_CannonBall" },

        // Projectile spawners
        { ActorPrototype_BaseProjectileSpawner, "ActorPrototype_BaseProjectileSpawner" },
        { ActorPrototype_Level2_LeftCannonTower, "ActorPrototype_Level2_LeftCannonTower" },

        //=======================================
        // General
        //=======================================

        { ActorPrototype_BaseProjectile, "ActorPrototype_BaseProjectile" },
        { ActorPrototype_FireSwordProjectile, "ActorPrototype_FireSwordProjectile" },
        { ActorPrototype_FrostSwordProjectile, "ActorPrototype_FrostSwordProjectile" },
        { ActorPrototype_LightningSwordProjectile, "ActorPrototype_LightningSwordProjectile" },
    };

    auto findIt = actorPrototypeEnumToStringMap.find(actorProto);
    if (findIt == actorPrototypeEnumToStringMap.end())
    {
        LOG_ERROR("Could not find actor enum: " + ToStr((int)actorProto));
        assert(false && "Could not convert ActorPrototype enum to string");
    }

    return findIt->second;
}

inline ActorPrototype ActorPrototypeStringToEnum(std::string actorProtoStr)
{
    static std::map<std::string, ActorPrototype> actorPrototypeStringToEnumMap =
    {
        // Actor enemies
        { "ActorPrototype_Level1_Soldier", ActorPrototype_Level1_Soldier },
        { "ActorPrototype_Level1_Officer", ActorPrototype_Level1_Officer },
        { "ActorPrototype_Level1_Rat", ActorPrototype_Level1_Rat },
        { "ActorPrototype_Level2_Soldier", ActorPrototype_Level2_Soldier },
        { "ActorPrototype_Level2_Officer", ActorPrototype_Level2_Officer },
        { "ActorPrototype_Level2_PunkRat", ActorPrototype_Level2_PunkRat },

        // Elevators
        { "ActorPrototype_BaseElevator", ActorPrototype_BaseElevator },

        { "ActorPrototype_Level1_Elevator", ActorPrototype_Level1_Elevator },
        { "ActorPrototype_Level2_Elevator", ActorPrototype_Level2_Elevator },

        // Toggle pegs
        { "ActorPrototype_BaseTogglePeg", ActorPrototype_BaseTogglePeg },

        { "ActorPrototype_Level1_TogglePeg", ActorPrototype_Level1_TogglePeg },
        { "ActorPrototype_Level2_TogglePeg", ActorPrototype_Level2_TogglePeg },

        // Projectiles
        { "ActorPrototype_Level2_CannonBall", ActorPrototype_Level2_CannonBall },

        // Projectile spawners
        { "ActorPrototype_BaseProjectileSpawner", ActorPrototype_BaseProjectileSpawner },
        { "ActorPrototype_Level2_LeftCannonTower", ActorPrototype_Level2_LeftCannonTower },

        //=======================================
        // General
        //=======================================

        { "ActorPrototype_BaseProjectile", ActorPrototype_BaseProjectile },
        { "ActorPrototype_FireSwordProjectile", ActorPrototype_FireSwordProjectile },
        { "ActorPrototype_FrostSwordProjectile", ActorPrototype_FrostSwordProjectile },
        { "ActorPrototype_LightningSwordProjectile", ActorPrototype_LightningSwordProjectile },
    };

    auto findIt = actorPrototypeStringToEnumMap.find(actorProtoStr);
    if (findIt == actorPrototypeStringToEnumMap.end())
    {
        LOG_ERROR("Could not find actor enum: " + actorProtoStr);
        assert(false && "Could not convert ActorPrototype enum to string");
    }

    return findIt->second;
}

//---------------------------------------------------------------------------------------------------------------------
// This class represents a single point in 2D space
//---------------------------------------------------------------------------------------------------------------------
class Point
{
public:
    double x, y;

    // construction
    Point(void) { x = y = 0; }
    Point(const double newX, const double newY) { x = newX; y = newY; }
    Point(const Point& newPoint) { x = newPoint.x; y = newPoint.y; }
    Point(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; }

    // assignment
    Point& operator=(const Point& newPoint) { x = newPoint.x; y = newPoint.y; return (*this); }
    Point& operator=(const Point* pNewPoint) { x = pNewPoint->x; y = pNewPoint->y; return (*this); }

    // addition/subtraction
    Point& operator+=(const Point& newPoint) { x += newPoint.x; y += newPoint.y; return (*this); }
    Point& operator-=(const Point& newPoint) { x -= newPoint.x; y -= newPoint.y; return (*this); }
    Point& operator+=(const Point* pNewPoint) { x += pNewPoint->x; y += pNewPoint->y; return (*this); }
    Point& operator-=(const Point* pNewPoint) { x -= pNewPoint->x; y -= pNewPoint->y; return (*this); }
    Point operator+(const Point& other) { Point temp(this); temp += other; return temp; }
    Point operator-(const Point& other) { Point temp(this); temp -= other; return temp; }

    // comparison
    bool operator==(const Point& other) const { return ((x == other.x) && (y == other.y)); }
    bool operator!=(const Point& other) const { return (!((x == other.x) && (y == other.y))); }

    // accessors (needed for Lua)
    double GetX() const { return x; }
    double GetY() const { return y; }
    void SetX(const double newX) { x = newX; }
    void SetY(const double newY) { y = newY; }
    void Set(const double newX, const double newY) { x = newX; y = newY; }

    // somewhat hacky vector emulation (maybe I should just write my own vector class)
    float Length() const { return sqrt((float)(x*x + y*y)); }

    bool IsZero() { return (std::fabs(x) < DBL_EPSILON || std::fabs(y) < DBL_EPSILON); }
    bool IsZeroXY() { return (std::fabs(x) < DBL_EPSILON && std::fabs(y) < DBL_EPSILON); }

    std::string ToString() { return ("[X: " + ToStr(x) + ", Y: " + ToStr(y) + "]"); }
};

inline Point operator-(const Point& left, const Point& right) { Point temp(left); temp -= right; return temp; }

//-------------------------------------------------------------------------------------------------
// ActorFixtureDef - Physics
//-------------------------------------------------------------------------------------------------

struct ActorFixtureDef
{
    ActorFixtureDef()
    {
        fixtureType = FixtureType_None;
        collisionShape = "Rectangle";
        isSensor = false;
        size = Point(0, 0);
        offset = Point(0, 0);

        friction = 0.0f;
        density = 0.0f;
        float restitution = 0.0f;

        collisionFlag = CollisionFlag_None;
        collisionMask = 0x0;
    }

    FixtureType fixtureType;
    std::string collisionShape;
    bool isSensor;
    Point size;
    Point offset;

    float friction;
    float density;
    float restitution;

    CollisionFlag collisionFlag;
    uint32 collisionMask;
};

//-------------------------------------------------------------------------------------------------
// ActorBodyDef - Physics
//-------------------------------------------------------------------------------------------------

struct ActorBodyDef
{
    ActorBodyDef()
    {
        bodyType = b2_dynamicBody;
        addFootSensor = false;
        makeCapsule = false;
        makeBullet = false;
        makeSensor = true;
        fixtureType = FixtureType_None;
        position = Point(0, 0);
        positionOffset = Point(0, 0);
        collisionShape = "Rectangle";
        size = Point(0, 0);
        gravityScale = 1.0f;
        setInitialSpeed = false;
        setInitialImpulse = false;
        initialSpeed = Point(0, 0);
        collisionFlag = CollisionFlag_None;
        collisionMask = 0x0;
        prefabType = "";

        friction = 0.0f;
        density = 0.0f;
        restitution = 0.0f;

        isActive = true;
    }

    WeakActorPtr pActor;
    b2BodyType bodyType;
    bool addFootSensor;
    bool makeCapsule;
    bool makeBullet;
    bool makeSensor;
    FixtureType fixtureType;
    Point position;
    Point positionOffset;
    std::string collisionShape;
    Point size;
    float gravityScale;
    bool setInitialSpeed;
    bool setInitialImpulse;
    Point initialSpeed;
    CollisionFlag collisionFlag;
    uint32 collisionMask;
    float friction;
    float density;
    float restitution;
    std::string prefabType;
    // TODO: This is a bit hacky - used for ducking 
    bool isActive;

    std::vector<ActorFixtureDef> fixtureList;
};

//-------------------------------------------------------------------------------------------------
// ElevatorDef - KinematicComponent
//-------------------------------------------------------------------------------------------------

struct ElevatorDef
{
    ElevatorDef()
    {
        hasTriggerBehaviour = false;
        hasStartBehaviour = false;
        hasStopBehaviour = false;
        hasOneWayBehaviour = false;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pElevatorComponent = new TiXmlElement("KinematicComponent");

        XML_ADD_2_PARAM_ELEMENT("Speed", "x", ToStr(speed.x).c_str(), "y", ToStr(speed.y).c_str(), pElevatorComponent);
        XML_ADD_2_PARAM_ELEMENT("MinPosition", "x", ToStr(minPosition.x).c_str(), "y", ToStr(minPosition.y).c_str(), pElevatorComponent);
        XML_ADD_2_PARAM_ELEMENT("MaxPosition", "x", ToStr(maxPosition.x).c_str(), "y", ToStr(maxPosition.y).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasTriggerBehaviour", ToStr(hasTriggerBehaviour).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasStartBehaviour", ToStr(hasStartBehaviour).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasStopBehaviour", ToStr(hasStopBehaviour).c_str(), pElevatorComponent);
        XML_ADD_TEXT_ELEMENT("HasOneWayBehaviour", ToStr(hasOneWayBehaviour).c_str(), pElevatorComponent);

        return pElevatorComponent;
    }

    static ElevatorDef CreateFromXml(TiXmlElement* pElem)
    {
        ElevatorDef def;
        def.LoadFromXml(pElem);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem)
    {
        SetPointIfDefined(&speed, pElem->FirstChildElement("Speed"), "x", "y");
        SetPointIfDefined(&minPosition, pElem->FirstChildElement("MinPosition"), "x", "y");
        SetPointIfDefined(&maxPosition, pElem->FirstChildElement("MaxPosition"), "x", "y");
        ParseValueFromXmlElem(&hasTriggerBehaviour, pElem->FirstChildElement("HasTriggerBehaviour"));
        ParseValueFromXmlElem(&hasStartBehaviour, pElem->FirstChildElement("HasStartBehaviour"));
        ParseValueFromXmlElem(&hasStopBehaviour, pElem->FirstChildElement("HasStopBehaviour"));
        ParseValueFromXmlElem(&hasOneWayBehaviour, pElem->FirstChildElement("HasOneWayBehaviour"));
    }

    Point speed;
    Point minPosition;
    Point maxPosition;

    bool hasTriggerBehaviour;
    bool hasStartBehaviour;
    bool hasStopBehaviour;
    bool hasOneWayBehaviour;
};

//-------------------------------------------------------------------------------------------------
// TogglePegDef - TogglePegAIComponent
//-------------------------------------------------------------------------------------------------

struct TogglePegDef
{
    TogglePegDef()
    {
        isAlwaysOn = false;
        timeOff = 0;
        timeOn = 0;
        toggleFrameIdx = 0;
        delay = 0;
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pTogglePegComponent = new TiXmlElement("TogglePegAIComponent");

        XML_ADD_TEXT_ELEMENT("AlwaysOn", ToStr(isAlwaysOn).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("TimeOff", ToStr(timeOff).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("TimeOn", ToStr(timeOn).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("ToggleFrameIdx", ToStr(toggleFrameIdx).c_str(), pTogglePegComponent);
        XML_ADD_TEXT_ELEMENT("Delay", ToStr(delay).c_str(), pTogglePegComponent);

        return pTogglePegComponent;
    }

    static TogglePegDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        TogglePegDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&isAlwaysOn, pElem->FirstChildElement("AlwaysOn")));
        cond_assert(strict, ParseValueFromXmlElem(&timeOff, pElem->FirstChildElement("TimeOff")));
        cond_assert(strict, ParseValueFromXmlElem(&timeOn, pElem->FirstChildElement("TimeOn")));
        cond_assert(strict, ParseValueFromXmlElem(&toggleFrameIdx, pElem->FirstChildElement("ToggleFrameIdx")));
        cond_assert(strict, ParseValueFromXmlElem(&delay, pElem->FirstChildElement("Delay")));
    }

    // All time values in miliseconds (ms)

    bool isAlwaysOn;
    int timeOff;
    int timeOn;
    int toggleFrameIdx;
    int delay;
};

//-------------------------------------------------------------------------------------------------
// ProjectileDef - ProjectileAIComponent
//-------------------------------------------------------------------------------------------------

struct ProjectileDef
{
    ProjectileDef()
    {
        damage = 0;
        damageTypeStr = "DamageType_None";
    }

    TiXmlElement* ToXml()
    {
        TiXmlElement* pProjectileComponent = new TiXmlElement("ProjectileAIComponent");

        XML_ADD_TEXT_ELEMENT("Damage", ToStr(damage).c_str(), pProjectileComponent);
        XML_ADD_TEXT_ELEMENT("ProjectileType", damageTypeStr.c_str(), pProjectileComponent);
        XML_ADD_2_PARAM_ELEMENT("ProjectileSpeed", "x", ToStr(projectileSpeed.x).c_str(), "y", ToStr(projectileSpeed.y).c_str(), pProjectileComponent);

        return pProjectileComponent;
    }

    static ProjectileDef CreateFromXml(TiXmlElement* pElem, bool strict)
    {
        ProjectileDef def;
        def.LoadFromXml(pElem, strict);
        return def;
    }

    void LoadFromXml(TiXmlElement* pElem, bool strict)
    {
        assert(pElem != NULL);

        cond_assert(strict, ParseValueFromXmlElem(&damage, pElem->FirstChildElement("Damage")));
        cond_assert(strict, ParseValueFromXmlElem(&damageTypeStr, pElem->FirstChildElement("ProjectileType")));
        cond_assert(strict, ParseValueFromXmlElem(&projectileSpeed, pElem->FirstChildElement("ProjectileSpeed"), "x", "y"));
    }

    int damage;
    std::string damageTypeStr;
    Point projectileSpeed;
};

//-------------------------------------------------------------------------------------------------
// CollisionInfo - Contains collisonFlag an collisionMask for PhysicsComponent
//-------------------------------------------------------------------------------------------------

struct CollisionInfo
{
    CollisionInfo()
    {
        collisionFlag = CollisionFlag_None;
        collisionMask = 0xFFFFFFFF;
    }

    CollisionInfo(CollisionFlag colFlag, uint32 colMask)
    {
        collisionFlag = colFlag;
        collisionMask = colMask;
    }

    CollisionFlag collisionFlag;
    uint32 collisionMask;
};

//-------------------------------------------------------------------------------------------------
// EnemyAttackAction - EnemyAIComponent, EnemyAIStateComponent
//-------------------------------------------------------------------------------------------------

struct EnemyAttackAction
{
    EnemyAttackAction()
    {
        attackAnimFrameIdx = 0;
        attackDamageType = DamageType_None;
        spawnedAttackPrototype = ActorPrototype_None;
        attackFxImageSet = "NONE";
    }

    // ActorTemplate will always have precedence... Later I should get rid of everything else
    ActorPrototype spawnedAttackPrototype;

    std::string animation;
    uint32 attackAnimFrameIdx;
    std::string attackFxImageSet;

    DamageType attackDamageType;

    // Used to spawn area damage

    // Considering enemy is looking left
    Point attackSpawnPositionOffset;

    // Applicable only to melee attacks
    Point attackAreaSize;

    // Damage which the attack will deal to enemies
    uint32 damage;

    // Enemy agro range bound to this attack action - attack action
    // can only be performed when enemy is within it
    ActorFixtureDef agroSensorFixture;
};

//-------------------------------------------------------------------------------------------------
// PredefinedMove - EnemyAIComponent, EnemyAIStateComponent
//-------------------------------------------------------------------------------------------------

struct PredefinedMove
{
    PredefinedMove()
    {
        msDuration = 0;
        pixelsPerSecond = Point(0, 0);
        soundToPlay = "";
    }

    uint32 msDuration;
    Point pixelsPerSecond;
    std::string soundToPlay;
};

//-------------------------------------------------------------------------------------------------
// SoundInfo - Used as definition of sound being played
//-------------------------------------------------------------------------------------------------

struct SoundInfo
{
    SoundInfo()
    {
        isMusic = false;
        soundVolume = 100;
        loops = 0;
        isAttentuated = false;
        attentuationFactor = 1.0f;
    }

    SoundInfo(std::string sound) : SoundInfo()
    {
        soundToPlay = sound;
    }

    std::string soundToPlay;
    bool isMusic;
    int soundVolume;
    int loops;
    bool isAttentuated;
    float attentuationFactor;
    Point soundSourcePosition;
};

#endif