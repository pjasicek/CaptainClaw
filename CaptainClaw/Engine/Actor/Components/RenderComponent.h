#ifndef RENDERCOMPONENT_H_
#define RENDERCOMPONENT_H_

#include "../../SharedDefines.h"
#include "../ActorComponent.h"

class Image;
typedef std::map<std::string, shared_ptr<Image>> ImageMap;

//=================================================================================================
// BaseRenderComponent Declaration
//

class SceneNode;
class BaseRenderComponent : public ActorComponent
{
public:
    virtual bool VInit(TiXmlElement* data) override;
    virtual TiXmlElement* VGenerateXml() override;
    virtual void VPostInit() override;
    virtual void VOnChanged() override;

    weak_ptr<Image> GetImage(std::string imageName);
    weak_ptr<Image> GetImage(uint32 imageId);
    bool HasImage(std::string imageName);
    bool HasImage(int32 imageId);

    uint32 GetImagesCount() const { return m_ImageMap.size(); }

    void SetHidden(bool hidden) { m_Hidden = hidden; }
    bool IsHidden() { return m_Hidden; }

    // Gets actor's X-Y-W-H
    virtual SDL_Rect VGetPositionRect() const = 0;

    shared_ptr<SceneNode> GetScneNodePublicTest() { return GetSceneNode(); }

protected:
    // loads the SceneNode specific data (represented in the <SceneNode> tag)
    virtual bool VDelegateInit(TiXmlElement* pData) { return true; }
    virtual shared_ptr<SceneNode> VCreateSceneNode(void) = 0;  // factory method to create the appropriate scene node

    // editor stuff
    virtual TiXmlElement* VCreateBaseElement(void) { return NULL; /*return new TiXmlElement(VGetName());*/ }
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement) = 0;

    ImageMap m_ImageMap;

    shared_ptr<SceneNode> m_pSceneNode;
    bool m_Hidden;

private:
    shared_ptr<SceneNode> GetSceneNode();
};

//=================================================================================================


//=================================================================================================
// ActorRenderComponent Declaration
//

class ActorRenderComponent : public BaseRenderComponent
{
public:
    ActorRenderComponent();

    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pXmlData) override;

    virtual SDL_Rect VGetPositionRect() const override;

    weak_ptr<Image> GetCurrentImage() { return m_CurrentImage; }
    void SetImage(std::string imageName);

    void SetMirrored(bool mirrored) { m_IsMirrored = mirrored; }

    inline bool IsVisible() { return m_IsVisible; }
    inline bool IsMirrored() { return m_IsMirrored; }
    inline bool IsInverted() { return m_IsInverted; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode() override;

    // Editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

    shared_ptr<Image> m_CurrentImage;

private:
    bool m_IsVisible;
    bool m_IsMirrored;
    bool m_IsInverted;
};

//=================================================================================================


//=================================================================================================
// TilePlaneRenderComponent Declaration
//

enum TilePlaneRenderPosition
{
    TilePlaneRenderPosition_Background,
    TilePlaneRenderPosition_Action,
    TilePlaneRenderPosition_Foreground
};

typedef std::vector<Image*> TileImageList;

struct TilePlaneProperties
{
    std::string name;

    int tilePixelWidth;
    int tilePixelHeight;

    int tilesOnAxisX;
    int tilesOnAxisY;

    int planePixelWidth;
    int planePixelHeight;

    int movementPercentX;
    int movementPercentY;

    int fillColor;

    int zCoord;

    bool isMainPlane;
    bool isDrawable;
    bool isWrappedX;
    bool isWrappedY;
    bool isTileAutosized;
};

class TilePlaneRenderComponent : public BaseRenderComponent
{
public:
    static const char* g_Name;
    virtual const char* VGetName() const { return g_Name; }

    virtual bool VDelegateInit(TiXmlElement* pXmlData) override;

    virtual SDL_Rect VGetPositionRect() const override;

    const TilePlaneProperties* const GetTilePlaneProperties() const { return &m_PlaneProperties; }
    const TileImageList* const GetTileImageList() const { return &m_TileImageList; }

protected:
    virtual shared_ptr<SceneNode> VCreateSceneNode() override;

    // Editor stuff
    virtual void VCreateInheritedXmlElements(TiXmlElement* pBaseElement);

private:
    // Background, action, foreground
    TilePlaneRenderPosition m_RenderLocation;

    std::string m_PlaneName;

    TilePlaneProperties m_PlaneProperties;

    // This is 1D array representing all tiles from top left to bottom right corner
    TileImageList m_TileImageList;

    // How far does this plane span
    SDL_Rect m_PositionRect;
};

//=================================================================================================

#endif