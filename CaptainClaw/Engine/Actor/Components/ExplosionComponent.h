#ifndef __EXPLOSION_COMPONENT_H__
#define __EXPLOSION_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "ControllerComponents/HealthComponent.h"
#include "PickupComponents/PickupComponent.h"

class ExplosionComponent : public PickupComponent
{
public:
    ExplosionComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual void VUpdate(uint32 msDiff);

    virtual bool VOnApply(Actor* pActorWhoPickedThis);

protected:
    virtual bool VDelegateInit(TiXmlElement* data);
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    int32 m_Damage;
    int32 m_ExplodingTime;
    int32 m_ActiveTime;
};

#endif