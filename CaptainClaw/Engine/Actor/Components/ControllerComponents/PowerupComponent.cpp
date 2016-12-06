#include "PowerupComponent.h"
#include "../PositionComponent.h"
#include "../RenderComponent.h"
#include "../PowerupSparkleAIComponent.h"
#include "../PhysicsComponent.h"
#include "../../ActorTemplates.h"

#include "../../../Events/EventMgr.h"
#include "../../../Events/Events.h"

const char* PowerupComponent::g_Name = "PowerupComponent";

PowerupComponent::PowerupComponent()
    :
    m_RemainingPowerupTime(0),
    m_ActivePowerup(PowerupType_None)
{ }

PowerupComponent::~PowerupComponent()
{
    for (auto pSparkle : m_PowerupSparkles)
    {
        pSparkle->Destroy();
    }

    m_PowerupSparkles.clear();
}

bool PowerupComponent::VInit(TiXmlElement* pData)
{
    return true;
}

void PowerupComponent::VPostInit()
{
    if (HasPowerup())
    {
        BroadcastPowerupStatusUpdated(_owner->GetGUID(), m_ActivePowerup, false);
        BroadcastPowerupTimeUpdated(_owner->GetGUID(), m_ActivePowerup, m_RemainingPowerupTime);
    }

    // Create powerup sparkles. Is this a good place ?
    for (int i = 0; i < 30; i++)
    {
        StrongActorPtr pPowerupSparkle = ActorTemplates::CreatePowerupSparkleActor();
        assert(pPowerupSparkle);

        shared_ptr<PositionComponent> pPositionComponent =
            MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
        assert(pPositionComponent);

        shared_ptr<PhysicsComponent> pPhysicsComponent =
            MakeStrongPtr(_owner->GetComponent<PhysicsComponent>(PhysicsComponent::g_Name));
        assert(pPhysicsComponent);

        shared_ptr<PowerupSparkleAIComponent> pPowerupSparkleAIComponent =
            MakeStrongPtr(pPowerupSparkle->GetComponent<PowerupSparkleAIComponent>(PowerupSparkleAIComponent::g_Name));
        assert(pPowerupSparkleAIComponent);

        pPowerupSparkleAIComponent->SetTargetPositionComponent(pPositionComponent.get());
        pPowerupSparkleAIComponent->SetTargetSize(pPhysicsComponent->GetBodySize());

        m_PowerupSparkles.push_back(pPowerupSparkle);
    }
}

TiXmlElement* PowerupComponent::VGenerateXml()
{
    // TODO:
    return NULL;
}

void PowerupComponent::VUpdate(uint32 msDiff)
{
    if (HasPowerup())
    {
        int32 oldSecsRemaining = m_RemainingPowerupTime / 1000;
        m_RemainingPowerupTime -= msDiff;
        int32 currentSecsRemainig = m_RemainingPowerupTime / 1000;

        if (m_RemainingPowerupTime <= 0)
        {
            BroadcastPowerupStatusUpdated(_owner->GetGUID(), m_ActivePowerup, true);
            m_ActivePowerup = PowerupType_None;
            m_RemainingPowerupTime = 0;

            SetPowerupSparklesVisibility(false);
        }
        else if (oldSecsRemaining != currentSecsRemainig)
        {
            BroadcastPowerupTimeUpdated(_owner->GetGUID(), m_ActivePowerup, currentSecsRemainig);
        }
    }
}

void PowerupComponent::ApplyPowerup(PowerupType powerupType, int32 msDuration)
{
    if (msDuration > 0 && powerupType != PowerupType_None)
    {
        m_ActivePowerup = powerupType;
        m_RemainingPowerupTime = msDuration;
        int32 secondsRemaining = m_RemainingPowerupTime / 1000;

        BroadcastPowerupStatusUpdated(_owner->GetGUID(), m_ActivePowerup, false);
        BroadcastPowerupTimeUpdated(_owner->GetGUID(), m_ActivePowerup, secondsRemaining);

        if (m_ActivePowerup == PowerupType_Catnip)
        {
            SetPowerupSparklesVisibility(true);
        }
    }
}

void PowerupComponent::SetPowerupSparklesVisibility(bool visible)
{
    for (auto pSparkle : m_PowerupSparkles)
    {
        shared_ptr<ActorRenderComponent> pRenderComponent =
            MakeStrongPtr(pSparkle->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
        assert(pRenderComponent);

        pRenderComponent->SetVisible(visible);
    }
}

void PowerupComponent::BroadcastPowerupTimeUpdated(uint32 actorId, PowerupType powerupType, int32 secondsRemaining)
{
    shared_ptr<EventData_Updated_Powerup_Time> pEvent(new EventData_Updated_Powerup_Time(actorId, powerupType, secondsRemaining));
    IEventMgr::Get()->VQueueEvent(pEvent);
}

void PowerupComponent::BroadcastPowerupStatusUpdated(uint32 actorId, PowerupType powerupType, bool isPowerupFinished)
{
    shared_ptr<EventData_Updated_Powerup_Status> pEvent(new EventData_Updated_Powerup_Status(actorId, powerupType, isPowerupFinished));
    IEventMgr::Get()->VQueueEvent(pEvent);
}