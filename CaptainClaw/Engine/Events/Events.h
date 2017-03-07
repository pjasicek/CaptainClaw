#ifndef __EVENTS_H__
#define __EVENTS_H__

#include "../SharedDefines.h"
#include "../Scene/SceneNodes.h"
#include "../Scene/HUDSceneNode.h"

#include "EventMgr.h"

// Auxiliary data decls ...
//
// data that is passed per-event in the userData parameter
// 
// ( for some, but not all, events )

//-- new object notification


//void RegisterEngineScriptEvents(void);

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define MAKE_EVENT_0_PARAM(EVENT_NAME) \
class EventData_##EVENT_NAME## : public BaseEventData \
{ \
public: \
    static const EventType sk_EventType; \
    \
    EventData_##EVENT_NAME##() { } \
    \
    virtual const EventType& VGetEventType() const { return sk_EventType; } \
    virtual IEventDataPtr VCopy() const { return IEventDataPtr(new EventData_##EVENT_NAME##()); } \
    virtual void VSerialize(std::ostrstream& out) const { } \
    virtual void VDeserialize(std::istrstream& in) { } \
    virtual const char* GetName(void) const { return ("EventData_" STRINGIFY(EVENT_NAME)); } \
}; \

#define MAKE_EVENT_1_PARAM(EVENT_NAME, PARAM1_TYPE, PARAM1_NAME) \
class EventData_##EVENT_NAME## : public BaseEventData \
{ \
public: \
    static const EventType sk_EventType; \
    \
    EventData_##EVENT_NAME##() { } \
    EventData_##EVENT_NAME##(PARAM1_TYPE PARAM1_NAME) { m_##PARAM1_NAME## = PARAM1_NAME; } \
    \
    virtual const EventType& VGetEventType() const { return sk_EventType; } \
    virtual IEventDataPtr VCopy() const { return IEventDataPtr(new EventData_##EVENT_NAME##()); } \
    virtual void VSerialize(std::ostrstream& out) const { } \
    virtual void VDeserialize(std::istrstream& in) { } \
    PARAM1_TYPE Get##PARAM1_NAME##() const { return m_##PARAM1_NAME##; } \
    virtual const char* GetName(void) const { return ("EventData_" STRINGIFY(EVENT_NAME)); } \
private: \
    PARAM1_TYPE m_##PARAM1_NAME##; \
}; \

//MAKE_EVENT_0_PARAM(yolo)

//MAKE_EVENT_1_PARAM(Stop_Jump, bool, Now)

//---------------------------------------------------------------------------------------------------------------------
// EventData_NewActor - This event is sent out when an actor is *actually* created.
//---------------------------------------------------------------------------------------------------------------------
class EventData_New_Actor : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_New_Actor(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
        m_ViewId = INVALID_GAME_VIEW_ID;
    }

    explicit EventData_New_Actor(uint32_t actorId, uint32_t viewId = INVALID_GAME_VIEW_ID)
        : m_ActorId(actorId),
        m_ViewId(viewId)
    {
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId;
        in >> m_ViewId;
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << " ";
        out << m_ViewId << " ";
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy(void) const
    {
        return IEventDataPtr(new EventData_New_Actor(m_ActorId, m_ViewId));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_New_Actor";
    }

    const uint32_t Getuint32_t(void) const
    {
        return m_ActorId;
    }

    uint32_t GetViewId(void) const
    {
        return m_ViewId;
    }

private:
    uint32_t m_ActorId;
    uint32_t m_ViewId;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Destroy_Actor - sent when actors are destroyed	
//---------------------------------------------------------------------------------------------------------------------
class EventData_Destroy_Actor : public BaseEventData
{
public:
    static const EventType sk_EventType;

    explicit EventData_Destroy_Actor(uint32_t id = INVALID_ACTOR_ID)
        : m_Id(id)
    {
        //
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy(void) const
    {
        return IEventDataPtr(new EventData_Destroy_Actor(m_Id));
    }

    virtual void VSerialize(std::ostrstream &out) const
    {
        out << m_Id;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_Id;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Destroy_Actor";
    }

    uint32_t GetActorId(void) const { return m_Id; }

private:
    uint32_t m_Id;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Move_Actor - sent when actors are moved
//---------------------------------------------------------------------------------------------------------------------
class EventData_Move_Actor : public BaseEventData
{
public:
    static const EventType sk_EventType;

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    EventData_Move_Actor(void)
    {
        m_Id = INVALID_ACTOR_ID;
    }

    EventData_Move_Actor(uint32_t id, const Point& move)
        : m_Id(id), m_Move(move)
    {
        //
    }

    virtual void VSerialize(std::ostrstream &out) const
    {
        out << m_Id << " ";
        out << m_Move.x << " ";
        out << m_Move.y << " ";
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_Id;
        in >> m_Move.x;
        in >> m_Move.y;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Move_Actor(m_Id, m_Move));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Move_Actor";
    }

    uint32_t GetActorId(void) const
    {
        return m_Id;
    }

    Point GetMove(void) const
    {
        return m_Move;
    }

private:
    uint32_t m_Id;
    Point m_Move;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_New_Render_Component - This event is sent out when an actor is *actually* created.
//---------------------------------------------------------------------------------------------------------------------
class EventData_New_Render_Component : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_New_Render_Component(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    explicit EventData_New_Render_Component(uint32_t actorId, shared_ptr<SceneNode> pSceneNode)
        : m_ActorId(actorId),
        m_pSceneNode(pSceneNode)
    {
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        LOG_ERROR(GetName() + std::string(" should not be serialzied!"));
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        LOG_ERROR(GetName() + std::string(" should not be serialzied!"));
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy(void) const
    {
        return IEventDataPtr(new EventData_New_Render_Component(m_ActorId, m_pSceneNode));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_New_Render_Component";
    }

    const uint32_t GetActorId(void) const
    {
        return m_ActorId;
    }

    shared_ptr<SceneNode> GetSceneNode(void) const
    {
        return m_pSceneNode;
    }

private:
    uint32_t m_ActorId;
    shared_ptr<SceneNode> m_pSceneNode;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Modified_Render_Component - This event is sent out when a render component is changed
//---------------------------------------------------------------------------------------------------------------------
class EventData_Modified_Render_Component : public BaseEventData
{
public:
    static const EventType sk_EventType;

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    EventData_Modified_Render_Component(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    EventData_Modified_Render_Component(uint32_t id)
        : m_ActorId(id)
    {
    }

    virtual void VSerialize(std::ostrstream &out) const
    {
        out << m_ActorId;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Modified_Render_Component(m_ActorId));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Modified_Render_Component";
    }

    uint32_t Getuint32_t(void) const
    {
        return m_ActorId;
    }
    
private:
    uint32_t m_ActorId;
};



//---------------------------------------------------------------------------------------------------------------------
// EventData_Environment_Loaded - this event is sent when a new game is started
//---------------------------------------------------------------------------------------------------------------------
class EventData_Environment_Loaded : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Environment_Loaded(void) { }
    virtual const EventType& VGetEventType(void) const	{ return sk_EventType; }
    virtual IEventDataPtr VCopy(void) const
    {
        return IEventDataPtr(new EventData_Environment_Loaded());
    }
    virtual const char* GetName(void) const  { return "EventData_Environment_Loaded"; }
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Environment_Loaded - this event is sent when a client has loaded its environment
//   This is special because we only want this event to go from client to server, and stop there. The
//   EventData_Environment_Loaded is received by server and proxy logics alike. Thy to do this with just the above 
//   event and you'll get into an endless loop of the EventData_Environment_Loaded event making infinite round trips
//   from client to server.
//
// FUTURE_WORK: It would be an interesting idea to add a "Private" type of event that is addressed only to a specific 
//              listener. Of course, that might be a really dumb idea too - someone will have to try it!
//---------------------------------------------------------------------------------------------------------------------
class EventData_Remote_Environment_Loaded : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Remote_Environment_Loaded(void) { }
    virtual const EventType& VGetEventType(void) const	{ return sk_EventType; }
    virtual IEventDataPtr VCopy(void) const
    {
        return IEventDataPtr(new EventData_Remote_Environment_Loaded());
    }
    virtual const char* GetName(void) const  { return "EventData_Remote_Environment_Loaded"; }
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Request_Start_Game - this is sent by the authoritative game logic to all views so they will load a game level.
//---------------------------------------------------------------------------------------------------------------------
class EventData_Request_Start_Game : public BaseEventData
{

public:
    static const EventType sk_EventType;

    EventData_Request_Start_Game(void) { }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Request_Start_Game());
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Request_Start_Game";
    }
};


/**** HOLY CRAP THIS ISN"T USED ANYMORE????
//---------------------------------------------------------------------------------------------------------------------
// EventData_Game_State - sent whenever the game state is changed (look for "BGS_" to see the different states)
//---------------------------------------------------------------------------------------------------------------------
class EventData_Game_State : public BaseEventData
{
BaseGameState m_gameState;
std::string m_parameter;

public:
static const EventType sk_EventType;

EventData_Game_State(void)
{
m_gameState = BGS_Invalid;
}

explicit EventData_Game_State(const BaseGameState gameState, const std::string &parameter)
: m_gameState(gameState), m_parameter(parameter)
{
}

virtual const EventType & VGetEventType( void ) const
{
return sk_EventType;
}

virtual IEventDataPtr VCopy() const
{
return IEventDataPtr( new EventData_Game_State( m_gameState, m_parameter ) );
}

virtual void VSerialize(std::ostrstream &out) const
{
const int tempVal = static_cast< int >( m_gameState );
out << tempVal << " ";
out << m_parameter;
}

virtual void VDeserialize(std::istrstream &in)
{
int tempVal;
in >> tempVal;
m_gameState = static_cast<BaseGameState>( tempVal );
in >> m_parameter;
}

virtual const char* GetName(void) const
{
return "EventData_Game_State";
}

BaseGameState GetGameState(void) const
{
return m_gameState;
}

const std::string GetParameter(void) const
{
return m_parameter;
}
};
**********************/


//---------------------------------------------------------------------------------------------------------------------
// EventData_Remote_Client						- Chapter 19, page 687
// 
//   Sent whenever a new client attaches to a game logic acting as a server				
//---------------------------------------------------------------------------------------------------------------------
class EventData_Remote_Client : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Remote_Client(void)
    {
        m_SocketId = 0;
        m_IpAddress = 0;
    }

    EventData_Remote_Client(const int socketid, const int ipaddress)
        : m_SocketId(socketid), m_IpAddress(ipaddress)
    {
    }

    virtual const EventType & VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Remote_Client(m_SocketId, m_IpAddress));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Remote_Client";
    }

    virtual void VSerialize(std::ostrstream &out) const
    {
        out << m_SocketId << " ";
        out << m_IpAddress;
    }

    virtual void VDeserialize(std::istrstream &in)
    {
        in >> m_SocketId;
        in >> m_IpAddress;
    }

    int GetSocketId(void) const
    {
        return m_SocketId;
    }

    int GetIpAddress(void) const
    {
        return m_IpAddress;
    }

private:
    int m_SocketId;
    int m_IpAddress;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Update_Tick - sent by the game logic each game tick
//---------------------------------------------------------------------------------------------------------------------
class EventData_Update_Tick : public BaseEventData
{
public:
    static const EventType sk_EventType;

    explicit EventData_Update_Tick(const int deltaMilliseconds)
        : m_DeltaMilliseconds(deltaMilliseconds)
    {
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Update_Tick(m_DeltaMilliseconds));
    }

    virtual void VSerialize(std::ostrstream & out)
    {
        LOG_ERROR("You should not be serializing update ticks!");
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Update_Tick";
    }

private:
    int m_DeltaMilliseconds;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Network_Player_Actor_Assignment - sent by the server to the clients when a network view is assigned a player number
//---------------------------------------------------------------------------------------------------------------------
class EventData_Network_Player_Actor_Assignment : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Network_Player_Actor_Assignment()
    {
        m_ActorId = INVALID_ACTOR_ID;
        m_SocketId = -1;
    }

    explicit EventData_Network_Player_Actor_Assignment(const uint32_t actorId, const int socketId)
        : m_ActorId(actorId), m_SocketId(socketId)

    {
    }

    virtual const EventType & VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Network_Player_Actor_Assignment(m_ActorId, m_SocketId));
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Network_Player_Actor_Assignment";
    }


    virtual void VSerialize(std::ostrstream &out) const
    {
        out << m_ActorId << " ";
        out << m_SocketId;
    }

    virtual void VDeserialize(std::istrstream &in)
    {
        in >> m_ActorId;
        in >> m_SocketId;
    }

    uint32_t Getuint32_t(void) const
    {
        return m_ActorId;
    }

    uint32_t GetSocketId(void) const
    {
        return m_SocketId;
    }

private:
    uint32_t m_ActorId;
    int m_SocketId;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Decompress_Request - sent to a multithreaded game event listener to decompress something in the resource file
//---------------------------------------------------------------------------------------------------------------------
class EventData_Decompress_Request : public BaseEventData
{
public:
    static const EventType sk_EventType;

    explicit EventData_Decompress_Request(std::wstring zipFileName, std::string filename)
        : m_ResourceFileName(zipFileName),
        m_FileName(filename)
    {
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Decompress_Request(m_ResourceFileName, m_FileName));
    }

    virtual void VSerialize(std::ostrstream & out)
    {
        LOG_ERROR("You should not be serializing decompression requests!");
    }

    const std::wstring& GetZipFilename(void) const
    {
        return m_ResourceFileName;
    }

    const std::string& GetFilename(void) const
    {
        return m_FileName;
    }
    virtual const char* GetName(void) const
    {
        return "EventData_Decompress_Request";
    }

private:
    std::wstring m_ResourceFileName;
    std::string m_FileName;
};


//---------------------------------------------------------------------------------------------------------------------
// EventData_Decompression_Progress - sent by the decompression thread to report progress
//---------------------------------------------------------------------------------------------------------------------
class EventData_Decompression_Progress : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Decompression_Progress(int progress, std::wstring zipFileName, std::string filename, void *buffer)
        : m_Progress(progress),
        m_ResourceFileName(zipFileName),
        m_FileName(filename),
        m_pBuffer(buffer)
    {
    }

    virtual const EventType & VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Decompression_Progress(m_Progress, m_ResourceFileName, m_FileName, m_pBuffer));
    }

    virtual void VSerialize(std::ostrstream & out)
    {
        LOG_ERROR("You should not be serializing decompression progress events!");
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Decompression_Progress";
    }

private:
    int m_Progress;
    std::wstring m_ResourceFileName;
    std::string m_FileName;
    void *m_pBuffer;
};


//---------------------------------------------------------------------------------------------------------------------
// class EventData_Request_New_Actor				
// This event is sent by a server asking Client proxy logics to create new actors from their local resources.
// It can be sent from script or via code.
// This event is also sent from the server game logic to client logics AFTER it has created a new actor. The logics will allow follow suit to stay in sync.
//---------------------------------------------------------------------------------------------------------------------
class EventData_Request_New_Actor : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Request_New_Actor()
    {
        m_ActorResource = "";
        m_HasInitialPosition = false;
        m_InitialPosition = Point(0, 0);
        m_ServerActorId = -1;
        m_ViewId = INVALID_GAME_VIEW_ID;
    }

    explicit EventData_Request_New_Actor(const std::string &actorResource, const Point *pInitialPosition = NULL, const uint32_t actorId = INVALID_ACTOR_ID, const uint32_t viewId = INVALID_GAME_VIEW_ID)
    {
        m_ActorResource = actorResource;
        if (pInitialPosition)
        {
            m_HasInitialPosition = true;
            m_InitialPosition = *pInitialPosition;
        }
        else
            m_HasInitialPosition = false;

        m_ServerActorId = actorId;
        m_ViewId = viewId;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual void VDeserialize(std::istrstream & in)
    {
        in >> m_ActorResource;
        in >> m_HasInitialPosition;
        if (m_HasInitialPosition)
        {
            in >> m_InitialPosition.x;
            in >> m_InitialPosition.y;
        }
        in >> m_ServerActorId;
        in >> m_ViewId;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Request_New_Actor(m_ActorResource, (m_HasInitialPosition) ? &m_InitialPosition : NULL, m_ServerActorId, m_ViewId));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_ActorResource << " ";
        out << m_HasInitialPosition << " ";
        if (m_HasInitialPosition)
        {
            out << m_InitialPosition.x << " ";
            out << m_InitialPosition.y << " ";
        }
        out << m_ServerActorId << " ";
        out << m_ViewId << " ";
    }

    virtual const char* GetName(void) const { return "EventData_Request_New_Actor"; }

    const std::string &GetActorResource(void) const { return m_ActorResource; }
    const Point *GetInitialTransform(void) const { return (m_HasInitialPosition) ? &m_InitialPosition : NULL; }
    const uint32_t GetServerActorId(void) const 	{ return m_ServerActorId; }
    uint32_t GetViewId(void) const { return m_ViewId; }

private:
    std::string m_ActorResource;
    bool m_HasInitialPosition;
    Point m_InitialPosition;
    uint32_t m_ServerActorId;
    uint32_t m_ViewId;
};

/*
//---------------------------------------------------------------------------------------------------------------------
// EventData_Request_Destroy_Actor - sent by any system requesting that the game logic destroy an actor	
//    FUTURE WORK - This event shouldn't really exist - subsystems should never ask the game logic to destroy something through an event, should they?
//---------------------------------------------------------------------------------------------------------------------
class EventData_Request_Destroy_Actor : public ScriptEvent
{
    uint32_t m_ActorId;

public:
    static const EventType sk_EventType;

    EventData_Request_Destroy_Actor()
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    EventData_Request_Destroy_Actor(uint32_t actorId)
    {
        m_ActorId = actorId;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual void VDeserialize(std::istrstream & in)
    {
        in >> m_ActorId;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Request_Destroy_Actor(m_ActorId));
    }

    virtual void VSerialize(std::ostrstream & out) const
    {
        out << m_ActorId;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Request_Destroy_Actor";
    }

    uint32_t Getuint32_t(void) const
    {
        return m_ActorId;
    }

    virtual bool VBuildEventFromScript(void)
    {
        if (m_eventData.IsInteger())
        {
            m_ActorId = m_eventData.GetInteger();
            return true;
        }
        return false;
    }
};
*/

//---------------------------------------------------------------------------------------------------------------------
// EventData_PlaySound - sent by any system wishing for a HumanView to play a sound
//---------------------------------------------------------------------------------------------------------------------
class EventData_PlaySound : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_PlaySound(void) { }
    EventData_PlaySound(const std::string& soundResource)
        : m_SoundResource(soundResource)
    {
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_PlaySound(m_SoundResource));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_SoundResource;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_SoundResource;
    }

    const std::string& GetResource(void) const
    {
        return m_SoundResource;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_PlaySound";
    }

    //virtual bool VBuildEventFromScript(void);

private:
    std::string m_SoundResource;
};

//=====================================================================================================================
// My events begin here
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// EventData_Attach_Actor - sent when controllable actor is loaded
//---------------------------------------------------------------------------------------------------------------------
class EventData_Attach_Actor : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Attach_Actor(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    EventData_Attach_Actor(uint32 actorId)
    {
        m_ActorId = actorId;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Attach_Actor(m_ActorId));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Attach_Actor";
    }

    //virtual bool VBuildEventFromScript(void);

private:
    uint32 m_ActorId;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Collideable_Tile_Created - sent when collidable tile has been created
//---------------------------------------------------------------------------------------------------------------------
class EventData_Collideable_Tile_Created : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Collideable_Tile_Created(void)
    {
        m_TileId = -1;
        m_PositionX = 0;
        m_PositionY = 0;
    }

    EventData_Collideable_Tile_Created(int32 tileId, int32 positionX, int32 positionY)
    {
        m_TileId = tileId;
        m_PositionX = positionX;
        m_PositionY = positionY;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Collideable_Tile_Created(m_TileId, m_PositionX, m_PositionY));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_TileId << " " << m_PositionX << " " << m_PositionY;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_TileId >> m_PositionX >> m_PositionY;
    }

    int32 GetTileId(void) const
    {
        return m_TileId;
    }

    int32 GetPositionX(void) const
    {
        return m_PositionX;
    }

    int32 GetPositionY(void) const
    {
        return m_PositionY;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Collideable_Tile_Created";
    }

    //virtual bool VBuildEventFromScript(void);

private:
    int32 m_TileId;
    int32 m_PositionX;
    int32 m_PositionY;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Start_Climb - sent when climb up or down is requested (down or up arrow is pressed)
//---------------------------------------------------------------------------------------------------------------------
class EventData_Start_Climb : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Start_Climb(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
        m_ClimbMovement = Point(0, 0);
    }

    EventData_Start_Climb(uint32 actorId, Point climbMovement)
    {
        m_ActorId = actorId;
        m_ClimbMovement = climbMovement;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Start_Climb(m_ActorId, m_ClimbMovement));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    Point GetClimbMovement() const
    {
        return m_ClimbMovement;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Start_Climb";
    }

    //virtual bool VBuildEventFromScript(void);

private:
    uint32 m_ActorId;
    Point m_ClimbMovement;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Actor_Fire - Sent when Fire button is pressed. Fires projectile like pellet / magic
//---------------------------------------------------------------------------------------------------------------------
class EventData_Actor_Fire : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Actor_Fire(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    EventData_Actor_Fire(uint32 actorId)
    {
        m_ActorId = actorId;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Actor_Fire(m_ActorId));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Actor_Fire";
    }

private:
    uint32 m_ActorId;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Actor_Attack - Sent when attack button is pressed.
//---------------------------------------------------------------------------------------------------------------------
class EventData_Actor_Attack : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Actor_Attack(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    EventData_Actor_Attack(uint32 actorId)
    {
        m_ActorId = actorId;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Actor_Attack(m_ActorId));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Actor_Attack";
    }

private:
    uint32 m_ActorId;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_New_HUD_Element
//---------------------------------------------------------------------------------------------------------------------
class EventData_New_HUD_Element : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_New_HUD_Element(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
    }

    EventData_New_HUD_Element(uint32 actorId, std::string key, shared_ptr<SDL2HUDSceneNode> pHUDSceneNode)
    {
        m_ActorId = actorId;
        m_pHUDSceneNode = pHUDSceneNode;
        m_pKey = key;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_New_HUD_Element(m_ActorId, m_pKey, m_pHUDSceneNode));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        LOG_ERROR(GetName() + std::string(" should not be serialzied!"));
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        LOG_ERROR(GetName() + std::string(" should not be serialzied!"));
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    shared_ptr<SDL2HUDSceneNode> GetHUDElement()
    {
        return m_pHUDSceneNode;
    }

    std::string GetHUDKey() { return m_pKey; }

    virtual const char* GetName(void) const
    {
        return "EventData_New_HUD_Element";
    }

private:
    uint32 m_ActorId;
    shared_ptr<SDL2HUDSceneNode> m_pHUDSceneNode;
    std::string m_pKey;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Modify_Score
//---------------------------------------------------------------------------------------------------------------------
class EventData_Modify_Player_Stat : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Modify_Player_Stat(uint32 actorId, PlayerStat stat, int32 value, bool addToExistingStat)
    {
        m_ActorId = actorId;
        m_Stat = stat;
        m_Value = value;
        m_AddToExistingStat = addToExistingStat;
    }

    virtual const EventType& VGetEventType(void) const { return sk_EventType; }
    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Modify_Player_Stat(m_ActorId, PlayerStat(m_Stat), m_Value, m_AddToExistingStat));
    }
    virtual void VSerialize(std::ostrstream& out) const { out << m_ActorId << m_Stat << m_Value << m_AddToExistingStat; }
    virtual void VDeserialize(std::istrstream& in) { in >> m_ActorId >> m_Stat >> m_Value >> m_AddToExistingStat; }

    uint32 GetActorId(void) const { return m_ActorId; }
    PlayerStat GetStatType() const { return PlayerStat(m_Stat); }
    int32 GetStatValue(void) const { return m_Value; }
    bool AddToExistingStat(void) const { return m_AddToExistingStat; }
    virtual const char* GetName(void) const { return "EventData_Modify_Player_Stat"; }

private:
    uint32 m_ActorId;
    int m_Stat;
    int32 m_Value;
    bool m_AddToExistingStat;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Updated_Score
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Score : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Score(uint32 actorId, uint32 oldScore, uint32 newScore, bool isInitialScore)
    {
        m_ActorId = actorId;
        m_OldScore = oldScore;
        m_NewScore = newScore;
        m_IsInitialScore = isInitialScore;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Score(m_ActorId, m_OldScore, m_NewScore, m_IsInitialScore));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << m_NewScore << m_OldScore << m_IsInitialScore;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId >> m_NewScore >> m_OldScore >> m_IsInitialScore;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    uint32 GetNewScore(void) const
    {
        return m_NewScore;
    }

    uint32 GetOldScore(void) const
    {
        return m_OldScore;
    }

    bool IsInitialScore(void) const
    {
        return m_IsInitialScore;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Score";
    }

private:
    uint32 m_ActorId;
    uint32 m_OldScore;
    uint32 m_NewScore;
    bool m_IsInitialScore;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_New_Life
//---------------------------------------------------------------------------------------------------------------------
class EventData_New_Life : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_New_Life(void)
    {
        m_ActorId = INVALID_ACTOR_ID;
        m_NumNewLives = 1;
    }

    EventData_New_Life(uint32 actorId, int numNewLives)
    {
        m_ActorId = actorId;
        m_NumNewLives = numNewLives;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_New_Life(m_ActorId, m_NumNewLives));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_NumNewLives;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_NumNewLives;
    }

    uint32 GetActorId() const
    {
        return m_ActorId;
    }

    int GetNumNewLives(void) const
    {
        return m_NumNewLives;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_New_Life";
    }

private:
    uint32 m_ActorId;
    int m_NumNewLives;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Updated_Lives
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Lives : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Lives(void)
    {
        m_OldLivesCount = 0;
    }

    EventData_Updated_Lives(uint32 oldLives, uint32 newLives, bool isInitialLives)
    {
        m_OldLivesCount = oldLives;
        m_NewLivesCount = newLives;
        m_IsInitialLives = isInitialLives;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Lives(m_OldLivesCount, m_NewLivesCount, m_IsInitialLives));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_OldLivesCount << m_NewLivesCount << m_IsInitialLives;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_OldLivesCount >> m_NewLivesCount >> m_IsInitialLives;
    }

    uint32 GetNewLivesCount(void) const
    {
        return m_NewLivesCount;
    }

    uint32 GetOldLivesCount(void) const
    {
        return m_OldLivesCount;
    }

    bool IsInitialLives(void) const
    {
        return m_IsInitialLives;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Lives";
    }

private:
    uint32 m_OldLivesCount;
    uint32 m_NewLivesCount;
    bool m_IsInitialLives;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Updated_Health
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Health : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Health(uint32 oldHealth, int32 newHealth, bool isInitialHealth)
    {
        m_OldHealth = oldHealth;
        m_NewHealth = newHealth;
        m_IsInitialHealth = isInitialHealth;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Health(m_OldHealth, m_NewHealth, m_IsInitialHealth));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_OldHealth << m_NewHealth << m_IsInitialHealth;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_OldHealth >> m_NewHealth >> m_IsInitialHealth;
    }

    int32 GetNewHealth(void) const
    {
        return m_NewHealth;
    }

    int32 GetOldHealth(void) const
    {
        return m_OldHealth;
    }

    bool IsInitialHealth(void) const
    {
        return m_IsInitialHealth;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Health";
    }

private:
    int32 m_OldHealth;
    int32 m_NewHealth;
    bool m_IsInitialHealth;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Update_Ammo
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Ammo : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Ammo(uint32 ammoType, uint32 ammoCount)
    {
        m_AmmoType = ammoType;
        m_AmmoCount = ammoCount;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Ammo(m_AmmoType, m_AmmoCount));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_AmmoType << m_AmmoCount;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_AmmoType >> m_AmmoCount;
    }

    uint32 GetAmmoType(void) const
    {
        return m_AmmoType;
    }

    uint32 GetAmmoCount(void) const
    {
        return m_AmmoCount;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Ammo";
    }

private:
    uint32 m_AmmoType;
    uint32 m_AmmoCount;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Request_Change_Ammo_Type
//---------------------------------------------------------------------------------------------------------------------
class EventData_Request_Change_Ammo_Type : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Request_Change_Ammo_Type(uint32 actorId, uint32 ammoType = AmmoType_Max)
    {
        m_ActorId = actorId;
        m_AmmoType = ammoType;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Request_Change_Ammo_Type(m_ActorId, m_AmmoType));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << m_AmmoType;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId >> m_AmmoType;
    }

    uint32 GetAmmoType(void) const
    {
        return m_AmmoType;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Request_Change_Ammo_Type";
    }

private:
    uint32 m_AmmoType;
    uint32 m_ActorId;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Updated_Ammo_Type
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Ammo_Type : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Ammo_Type(uint32 actorId, uint32 ammoType)
    {
        m_ActorId = actorId;
        m_AmmoType = ammoType;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Ammo_Type(m_ActorId, m_AmmoType));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << m_AmmoType;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId >> m_AmmoType;
    }

    uint32 GetAmmoType(void) const
    {
        return m_AmmoType;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Ammo_Type";
    }

private:
    uint32 m_AmmoType;
    uint32 m_ActorId;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Teleport_Actor
//---------------------------------------------------------------------------------------------------------------------
class EventData_Teleport_Actor : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Teleport_Actor(uint32 actorId, const Point& destination)
    {
        m_ActorId = actorId;
        m_Destination = destination;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Teleport_Actor(m_ActorId, m_Destination));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << m_Destination.x << m_Destination.y;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId >> m_Destination.x >> m_Destination.y;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    Point GetDestination(void) const
    {
        return m_Destination;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Teleport_Actor";
    }

private:
    uint32 m_ActorId;
    Point m_Destination;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Updated_Powerup_Time
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Powerup_Time : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Powerup_Time()
    {
        m_ActorId = INVALID_ACTOR_ID;
        m_PowerupType = PowerupType_None;
        m_SecondsRemaining = 0;
    }

    EventData_Updated_Powerup_Time(uint32 actorId, uint32 powerupType, int32 secondsRemaining)
    {
        m_ActorId = actorId;
        m_PowerupType = powerupType;
        m_SecondsRemaining = secondsRemaining;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Powerup_Time(m_ActorId, m_PowerupType, m_SecondsRemaining));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << m_PowerupType << m_SecondsRemaining;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId >> m_PowerupType >> m_SecondsRemaining;
    }

    uint32 GetPowerupType(void) const
    {
        return m_PowerupType;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    int32 GetSecondsRemaining() const
    {
        return m_SecondsRemaining;
    }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Powerup_Time";
    }

private:
    uint32 m_ActorId;
    uint32 m_PowerupType;
    int32 m_SecondsRemaining;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Updated_Powerup_Status
//---------------------------------------------------------------------------------------------------------------------
class EventData_Updated_Powerup_Status : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Updated_Powerup_Status()
    {
        m_ActorId = INVALID_ACTOR_ID;
        m_PowerupType = PowerupType_None;
        m_IsPowerupFinished = false;
    }

    EventData_Updated_Powerup_Status(uint32 actorId, PowerupType powerupType, bool isPowerupFinished)
    {
        m_ActorId = actorId;
        m_PowerupType = powerupType;
        m_IsPowerupFinished = isPowerupFinished;
    }

    virtual const EventType& VGetEventType(void) const
    {
        return sk_EventType;
    }

    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Updated_Powerup_Status(m_ActorId, PowerupType(m_PowerupType), m_IsPowerupFinished));
    }

    virtual void VSerialize(std::ostrstream& out) const
    {
        out << m_ActorId << m_PowerupType << m_IsPowerupFinished;
    }

    virtual void VDeserialize(std::istrstream& in)
    {
        in >> m_ActorId >> m_PowerupType >> m_IsPowerupFinished;
    }

    uint32 GetActorId(void) const
    {
        return m_ActorId;
    }

    bool IsPowerupFinished() const
    {
        return m_IsPowerupFinished;
    }

    PowerupType GetPowerupType() const { return PowerupType(m_PowerupType); }

    virtual const char* GetName(void) const
    {
        return "EventData_Updated_Powerup_Status";
    }

private:
    uint32 m_ActorId;
    uint32 m_PowerupType;
    bool m_IsPowerupFinished;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Checkpoint_Reached
//---------------------------------------------------------------------------------------------------------------------
class EventData_Checkpoint_Reached : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Checkpoint_Reached(uint32 actorId, Point spawnPoint, bool isSaveCheckpoint, uint32 saveCheckpointNumber)
    {
        m_ActorId = actorId;
        m_SpawnPoint = spawnPoint;
        m_IsSaveCheckpoint = isSaveCheckpoint;
        m_SaveCheckpointNumber = saveCheckpointNumber;
    }

    virtual const EventType& VGetEventType(void) const { return sk_EventType; }
    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Checkpoint_Reached(m_ActorId, m_SpawnPoint, m_IsSaveCheckpoint, m_SaveCheckpointNumber));
    }
    virtual void VSerialize(std::ostrstream& out) const { out << m_ActorId << m_SpawnPoint.x << m_SpawnPoint.y << m_IsSaveCheckpoint << m_SaveCheckpointNumber; }
    virtual void VDeserialize(std::istrstream& in) { in >> m_ActorId >> m_SpawnPoint.x >> m_SpawnPoint.y >> m_IsSaveCheckpoint >> m_SaveCheckpointNumber; }

    uint32 GetActorId(void) const { return m_ActorId; }
    Point GetSpawnPoint() const { return m_SpawnPoint; }
    bool IsSaveCheckpoint(void) const { return m_IsSaveCheckpoint; }
    uint32 GetSaveCheckpointNumber(void) const { return m_SaveCheckpointNumber; }
    virtual const char* GetName(void) const { return "EventData_Checkpoint_Reached"; }

private:
    uint32 m_ActorId;
    Point m_SpawnPoint;
    bool m_IsSaveCheckpoint;
    uint32 m_SaveCheckpointNumber;
};

//---------------------------------------------------------------------------------------------------------------------
// EventData_Claw_Died
//---------------------------------------------------------------------------------------------------------------------
class EventData_Claw_Died : public BaseEventData
{
public:
    static const EventType sk_EventType;

    EventData_Claw_Died(uint32 actorId)
    {
        m_ActorId = actorId;
    }

    virtual const EventType& VGetEventType(void) const { return sk_EventType; }
    virtual IEventDataPtr VCopy() const
    {
        return IEventDataPtr(new EventData_Claw_Died(m_ActorId));
    }
    virtual void VSerialize(std::ostrstream& out) const { out << m_ActorId; }
    virtual void VDeserialize(std::istrstream& in) { in >> m_ActorId; }

    uint32 GetActorId(void) const { return m_ActorId; }
    virtual const char* GetName(void) const { return "EventData_Claw_Died"; }

private:
    uint32 m_ActorId;
};

#endif