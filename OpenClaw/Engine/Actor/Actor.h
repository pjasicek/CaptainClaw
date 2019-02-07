#ifndef ACTOR_H_
#define ACTOR_H_

#include <string>
#include <stdint.h>
#include <memory>
#include <map>

#include "../SharedDefines.h"

#include "ActorComponent.h"

typedef std::map<uint32, StrongActorComponentPtr> ActorComponentsMap;

class PositionComponent;
class TiXmlElement;
class Actor
{
public:
    Actor(uint32_t actorGuid);
    ~Actor();

    bool Init(TiXmlElement* data);
    void PostInit();
    void PostPostInit();
    void Destroy();
    void Update(uint32_t msDiff);

    std::string ToXML();

    inline uint32_t GetGUID() const { return _GUID; }
    inline std::string GetName() const { return _name; }

    // Retrieves component from given ID or nullptr if component not found
    template <class ComponentType>
    weak_ptr<ComponentType> GetComponent(uint32 id)
    {
        ActorComponentsMap::iterator findIter = _components.find(id);
        if (findIter != _components.end())
        {
            StrongActorComponentPtr base(findIter->second);
            shared_ptr<ComponentType> sub(std::static_pointer_cast<ComponentType>(base));  // cast to subclass version of the pointer
            weak_ptr<ComponentType> weakSub(sub);  // convert strong pointer to weak pointer
            return weakSub;  // return the weak pointer
        }

        return weak_ptr<ComponentType>();
    }

    // Retrieves component from given name or nullptr if component not found
    template <class ComponentType>
    weak_ptr<ComponentType> GetComponent(const char *name)
    {
        uint32 id = ActorComponent::GetIdFromName(name);
        ActorComponentsMap::iterator findIter = _components.find(id);
        if (findIter != _components.end())
        {
            StrongActorComponentPtr base(findIter->second);
            shared_ptr<ComponentType> sub(static_pointer_cast<ComponentType>(base));  // cast to subclass version of the pointer
            weak_ptr<ComponentType> weakSub(sub);  // convert strong pointer to weak pointer
            return weakSub;  // return the weak pointer
        }
        else
        {
            return weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    weak_ptr<ComponentType> GetComponent()
    {
        uint32 id = ActorComponent::GetIdFromName(ComponentType::g_Name);
        ActorComponentsMap::iterator findIter = _components.find(id);
        if (findIter != _components.end())
        {
            StrongActorComponentPtr base(findIter->second);
            shared_ptr<ComponentType> sub(static_pointer_cast<ComponentType>(base));  // cast to subclass version of the pointer
            weak_ptr<ComponentType> weakSub(sub);  // convert strong pointer to weak pointer
            return weakSub;  // return the weak pointer
        }
        else
        {
            return weak_ptr<ComponentType>();
        }
    }

    template <class ComponentType>
    ComponentType* GetRawComponent(bool bAssertNotNull = false)
    {
        uint32 id = ActorComponent::GetIdFromName(ComponentType::g_Name);
        ActorComponentsMap::iterator findIter = _components.find(id);
        if (findIter != _components.end())
        {
            StrongActorComponentPtr base(findIter->second);
            shared_ptr<ComponentType> sub(static_pointer_cast<ComponentType>(base));  // cast to subclass version of the pointer
            return sub.get();
        }
        
        if (bAssertNotNull)
        {
            LOG_ASSERT("Failed to get component from actor: " + GetName());
        }

        return nullptr;
    }

    const ActorComponentsMap* GetComponents() { return &_components; }

    void AddComponent(StrongActorComponentPtr pComponent);

    void OnWorldFinishedLoading();

    //=========================================================================
    // Some components are accessed REALLY often so it makes sense to just
    // put them here and dont use the templated GetComponent method
    //=========================================================================

    inline shared_ptr<PositionComponent> GetPositionComponent() { return m_pPositionComponent; }

private:
    friend class ActorFactory;

    uint32_t _GUID;
    std::string _name;

    ActorComponentsMap _components;

    // Resource from which this actor was loaded
    std::string _resource;

    shared_ptr<PositionComponent> m_pPositionComponent;
};

#endif