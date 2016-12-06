#ifndef __POWERUP_SPARKLE_COMPONENT_H__
#define __POWERUP_SPARKLE_COMPONENT_H__

#include "../../SharedDefines.h"
#include "../ActorComponent.h"
#include "AnimationComponent.h"

class PositionComponent;
class PowerupSparkleAIComponent : public ActorComponent, public AnimationObserver
{
public:
    PowerupSparkleAIComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit() override;
    virtual TiXmlElement* VGenerateXml() override;

    void SetTargetPositionComponent(PositionComponent* pTarget);
    void SetTargetSize(const Point& targetSize) { m_TargetSize = targetSize; }

    virtual void VOnAnimationLooped(Animation* pAnimation) override;

private:
    PositionComponent* m_pTargetPositionComponent;
    PositionComponent* m_pPositonComponent;
    Point m_TargetSize;
};

#endif