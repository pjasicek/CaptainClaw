#ifndef __FLOOR_SPIKE_COMPONENT_H__
#define __FLOOR_SPIKE_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"
#include "AuraComponents/AuraComponent.h"

class DamageAuraComponent;
class FloorSpikeComponent : public ActorComponent, public AnimationObserver
{
public:
    FloorSpikeComponent();

    static const char* g_Name;
    virtual const char* VGetName() const override { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;

    virtual TiXmlElement* VGenerateXml() override { assert(false && "Unimplemented"); return nullptr; }

    void VOnAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame) override;

private:
    // XML Data
    int m_ActiveFrameIdx;
    int m_StartDelay;
    int m_TimeOn;
    std::string m_ActivateSound;
    std::string m_DeactivateSound;

    // Internal state
    DamageAuraComponent* m_pDamageAuraComponent;
};

#endif