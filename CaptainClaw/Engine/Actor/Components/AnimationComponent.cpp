#include <Tinyxml\tinyxml.h>
#include "AnimationComponent.h"
#include "../../GameApp/BaseGameApp.h"
#include "../../Resource/Loaders/AniLoader.h"
#include "RenderComponent.h"
#include "PositionComponent.h"

#include "RenderComponent.h"

const char* AnimationComponent::g_Name = "AnimationComponent";

AnimationComponent::AnimationComponent()
    :
    _currentAnimation(NULL),
    m_PauseOnStart(false)
{ }

AnimationComponent::~AnimationComponent()
{
    _animationMap.clear();
}

bool AnimationComponent::VInit(TiXmlElement* data)
{
    assert(data != NULL);

    // Loop through all anim paths elements
    for (TiXmlElement* animPathElem = data->FirstChildElement("AnimationPath");
        animPathElem != NULL; animPathElem = animPathElem->NextSiblingElement("AnimationPath"))
    {
        const char* animationsPath = animPathElem->GetText();

        // TODO: Rework. Consult with RenderComponent.cpp for proper fast implementation.
        // Take the algo from there and make it general purpose, dont copy-paste stuff
        std::vector<std::string> matchingAnimNames =
            g_pApp->GetResourceCache()->Match(animationsPath);

        for (std::string animPath : matchingAnimNames)
        {
            WapAni* wapAni = AniResourceLoader::LoadAndReturnAni(animPath.c_str());
            std::string animNameKey = StripPathAndExtension(animPath);

            // Check if we dont already have the animation loaded
            if (_animationMap.count(animNameKey) > 0)
            {
                LOG_WARNING("Trying to load existing animation: " + animPath);
                continue;
            }
            
            Animation* animation = Animation::CreateAnimation(wapAni, animNameKey.c_str(), this);
            if (!animation)
            {
                LOG_ERROR("Could not create animation: " + animPath);
                return false;
            }

            _animationMap.insert(std::make_pair(animNameKey, animation));
        }
    }

    for (TiXmlElement* animElem = data->FirstChildElement("Animation");
        animElem != NULL; animElem = animElem->NextSiblingElement("Animation"))
    {
        if (!animElem->Attribute("type"))
        {
            LOG_WARNING("Animation element is missing type attribute.");
            continue;
        }

        std::string animType = animElem->Attribute("type");
        
        m_SpecialAnimationRequestList.push_back(animType);
    }

    if (TiXmlElement* pElem = data->FirstChildElement("PauseOnStart"))
    {
        m_PauseOnStart = std::string(pElem->GetText()) == "true";
    }

    if (_animationMap.empty() && m_SpecialAnimationRequestList.empty())
    {
        LOG_WARNING("Animation map for animation component is empty. Actor type: " + std::string(data->Parent()->ToElement()->Attribute("Type")));
    }

    return true;
}

void AnimationComponent::VPostInit()
{
    for (std::string animType : m_SpecialAnimationRequestList)
    {
        if (animType.find("cycle") != std::string::npos)
        {
            std::string cycleDurationStr = animType;
            cycleDurationStr.erase(0, 5);
            int cycleDuration = std::stoi(cycleDurationStr);

            shared_ptr<ActorRenderComponent> pRenderComponent = MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
            if (!pRenderComponent)
            {
                pRenderComponent = MakeStrongPtr(_owner->GetComponent<HUDRenderComponent>(HUDRenderComponent::g_Name));
            }
            if (!pRenderComponent)
            {
                LOG_ERROR("Actor has existing animation component but not render component. Actor: " + _owner->GetName());
                continue;
            }

            // If there is only 1 or 0 frames, we do not need to animate it
            if (pRenderComponent->GetImagesCount() <= 1)
            {
                //LOG("Skipping creating " + animType + " animation for: " + _owner->GetName());
                continue;
            }

            Animation* pCycleAnim = Animation::CreateAnimation(pRenderComponent->GetImagesCount(), cycleDuration, animType.c_str(), this);
            if (!pCycleAnim)
            {
                LOG_ERROR("Failed to create " + animType + " animation.");
                continue;
            }

            // Set delay according to X coord
            shared_ptr<PositionComponent> pPositionComponent =
                MakeStrongPtr(_owner->GetComponent<PositionComponent>(PositionComponent::g_Name));
            if (!pPositionComponent)
            {
                LOG_ERROR("Actor is missing position component. Actor: " + _owner->GetName());
                continue;
            }
            
            // HACK: TODO: Make special animation creation somewhat more general
            // so I dont have to use these hacks
            // This hack is specific to Toggle pegs which set their on delay
            if (cycleDuration != 75 && cycleDuration != 50 && cycleDuration != 99)
            {
                srand(pPositionComponent->GetX());
                pCycleAnim->SetDelay(rand() % 1000);
            }

            _animationMap.insert(std::make_pair(animType, pCycleAnim));
        }
        else
        {
            LOG_WARNING("Unknown special animation type: " + animType);
        }
    }

    if (!_animationMap.empty())
    {
        _currentAnimation = _animationMap.begin()->second;
    }

    if (m_PauseOnStart)
    {
        _currentAnimation->Pause();
    }

    /*else
    {
        assert(false && "Animation component is without any animation");
    }*/
}

TiXmlElement* AnimationComponent::VGenerateXml()
{
    TiXmlElement* baseElement = new TiXmlElement(VGetName());

    //

    return baseElement;
}

void AnimationComponent::VUpdate(uint32 msDiff)
{
    if (_currentAnimation)
    {
        _currentAnimation->Update(msDiff);
    }
}

bool AnimationComponent::SetAnimation(std::string animationName)
{
    /*if (animationName == "advance")
    {
        for (auto a : _animationMap)
        {
            LOG(a.second->GetName());
        }
    }*/

    if (animationName == _currentAnimation->GetName())
    {
        //LOG("Trying to set the same animation: " + animationName);
        return true;
    }
    //LOG("Setting animation: " + animationName);

    auto findIt = _animationMap.find(animationName);
    if (findIt == _animationMap.end())
    {
        LOG_WARNING("Could not find animation: " + animationName + " for actor: " + _owner->GetName());
        return false;
    }

    //LOG("Setting anim: " + animationName);

    Animation* animation = findIt->second;

    animation->Reset();
    _currentAnimation = animation;
    OnAnimationFrameStarted(_currentAnimation->GetCurrentAnimationFrame());

    return true;
}

bool AnimationComponent::HasAnimation(std::string& animName)
{ 
    return (_animationMap.count(animName) > 0);
}

void AnimationComponent::PauseAnimation()
{
    _currentAnimation->Pause();
    NotifyAnimationPaused(_currentAnimation);
}

void AnimationComponent::ResumeAnimation()
{
    _currentAnimation->Resume();
    NotifyAnimationResumed(_currentAnimation);
}

void AnimationComponent::ResetAnimation()
{
    _currentAnimation->Reset();
}

std::string AnimationComponent::GetCurrentAnimationName() const
{
    return _currentAnimation->GetName();
}

//-------------------------------------------------------------------------------------------------
// Animation listeners
//

void AnimationComponent::OnAnimationFrameFinished(AnimationFrame* frame)
{
    if (!frame->eventName.empty())
    {
        // Raise event
    }
}

void AnimationComponent::OnAnimationFrameStarted(AnimationFrame* frame)
{
    if (!frame->eventName.empty())
    {
        // Raise event
    }

    // Notify render component to change frame image
    shared_ptr<ActorRenderComponent> renderComponent =
        MakeStrongPtr(_owner->GetComponent<ActorRenderComponent>(ActorRenderComponent::g_Name));
    if (renderComponent)
    {
        renderComponent->SetImage(frame->imageName);
    }
    else if (renderComponent = MakeStrongPtr(_owner->GetComponent<HUDRenderComponent>(HUDRenderComponent::g_Name)))
    {
        renderComponent->SetImage(frame->imageName);
    }
    else
    {
        LOG_WARNING("Could not find render component for actor: " + _owner->GetName());
    }
}

void AnimationComponent::OnAnimationFinished()
{

}

void AnimationComponent::OnAnimationFrameChanged(AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    NotifyAnimationFrameChanged(_currentAnimation, pLastFrame, pNewFrame);
}

void AnimationComponent::OnAnimationLooped()
{
    NotifyAnimationLooped(_currentAnimation);
}

void AnimationComponent::SetDelay(uint32 msDelay)
{
    _currentAnimation->SetDelay(msDelay);
}

void AnimationComponent::SetReverseAnimation(bool reverse)
{
    _currentAnimation->SetReverseAnim(reverse);
}

//=====================================================================================================================
// AnimationSubject implementation
//=====================================================================================================================

void AnimationSubject::NotifyAnimationLooped(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        pSubject->VOnAnimationLooped(pAnimation);
    }
}

void AnimationSubject::NotifyAnimationStarted(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        pSubject->VOnAnimationStarted(pAnimation);
    }
}

void AnimationSubject::NotifyAnimationFrameChanged(Animation* pAnimation, AnimationFrame* pLastFrame, AnimationFrame* pNewFrame)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        pSubject->VOnAnimationFrameChanged(pAnimation, pLastFrame, pNewFrame);
    }
}

void AnimationSubject::NotifyAnimationPaused(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        pSubject->VOnAnimationPaused(pAnimation);
    }
}

void AnimationSubject::NotifyAnimationResumed(Animation* pAnimation)
{
    for (AnimationObserver* pSubject : m_AnimationObservers)
    {
        pSubject->VOnAnimationResumed(pAnimation);
    }
}

void AnimationSubject::AddObserver(AnimationObserver* pObserver)
{
    m_AnimationObservers.push_back(pObserver);
}

void AnimationSubject::RemoveObserver(AnimationObserver* pObserver)
{
    m_AnimationObservers.erase(std::remove(m_AnimationObservers.begin(), m_AnimationObservers.end(), pObserver), m_AnimationObservers.end());
}