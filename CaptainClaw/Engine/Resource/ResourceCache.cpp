#include <algorithm>
#include <cctype>

#include <libwap.h>

#include "ResourceCache.h"
#include "../Util/StringUtil.h"

//
// Resource::Resource
//
Resource::Resource(const std::string& name)
{
    _name = name;
    // Transform resource name into lower case characters
    std::transform(_name.begin(), _name.end(), _name.begin(), (int(*)(int)) std::tolower);
}

//=================================================================================================
// class ResourceRezArchive
//
//     This class implements the IResourceFile interface with RezArchive
//

ResourceRezArchive::ResourceRezArchive(const std::string rezArchiveFileName)
{
    _rezArchiveFileName = rezArchiveFileName;
    _rezArchive = NULL;
}

ResourceRezArchive::~ResourceRezArchive()
{
    // This pointer is owned by libwap library
    if (_rezArchive)
    {
        WAP_DestroyRezArchive(_rezArchive);
    }
}

bool ResourceRezArchive::VOpen()
{
    _rezArchive = WAP_LoadRezArchive(_rezArchiveFileName.c_str());
    if (_rezArchive == NULL)
    {
        LOG_ERROR("Could not load Rez archive: " + _rezArchiveFileName);
        return false;
    }
    
    return true;
}

int32 ResourceRezArchive::VGetRawResourceSize(Resource* r)
{
    RezFile* rezFile = WAP_GetRezFileFromRezArchive(_rezArchive, r->GetName().c_str());
    if (rezFile == NULL)
    {
        LOG_ERROR("Could not locate: " + r->GetName() + " in rezArchive: " + _rezArchiveFileName);
        return -1;
    }

    return rezFile->size;
}

// Remark: outBuffer is valid heap pointer
int32 ResourceRezArchive::VGetRawResource(Resource* r, char* outBuffer)
{
    assert(outBuffer != NULL);

    RezFile* rezFile = WAP_GetRezFileFromRezArchive(_rezArchive, r->GetName().c_str());
    if (rezFile == NULL)
    {
        LOG_ERROR("Could not locate: " + r->GetName() + " in rezArchive: " + _rezArchiveFileName);
        return -1;
    }

    // This this buffer is owner by libwap => we need to create our own
    char* data = WAP_GetRezFileData(rezFile);
    //LOG_WARNING("rezFile->fullName = " + std::string(rezFile->fullPathAndName));
    if (data == NULL)
    {
        LOG_ERROR("Could not load buffer for rez file: " + r->GetName() + " in rezArchive: " + _rezArchiveFileName);
        return -1;
    }

    // 
    memcpy(outBuffer, data, rezFile->size);

    // We dont need libwaps data anymore
    WAP_FreeFileData(rezFile);

    return rezFile->size;
}

int32 ResourceRezArchive::VGetNumResources() const
{
    return WAP_GetRezFilesCount(_rezArchive);
}

std::string ResourceRezArchive::VGetResourceName(int32 num) const
{
    RezFile* rezFile = WAP_GetRezFileFromFileIdx(_rezArchive, num);
    if (rezFile == NULL)
    {
        return "NOT_FOUND";
    }

    return rezFile->fullPathAndName;
}

std::vector<std::string> ResourceRezArchive::GetAllFilesInDirectory(const char* directoryPath)
{
    std::vector<std::string> filesInDirectory;

    RezDirectory* rezDirectory = WAP_GetRezDirectoryFromRezArchive(_rezArchive, directoryPath);
    if (rezDirectory == NULL || rezDirectory->directoryContents == NULL)
    {
        return filesInDirectory;
    }
    
    RezDirectoryContents* pDirectoryContents = rezDirectory->directoryContents;
    uint32 filesCount = pDirectoryContents->rezFilesCount;
    for (uint32 fileIdx = 0; fileIdx < filesCount; ++fileIdx)
    {
        filesInDirectory.push_back(pDirectoryContents->rezFiles[fileIdx]->fullPathAndName);
    }

    return filesInDirectory;
}

//=================================================================================================
// class ResourceHandle
//

ResourceHandle::ResourceHandle(Resource& resource, char* buffer, uint32 size, ResourceCache* resCache)
    : _resource(resource)
{
    _buffer = buffer;
    _size = size;
    _extraData = NULL;
    _resourceCache = resCache;
}

ResourceHandle::~ResourceHandle()
{
    SAFE_DELETE_ARRAY(_buffer);

    _resourceCache->MemoryHasBeenFreed(_size);
}

//=================================================================================================
// class ResourceCache
//

ResourceCache::ResourceCache(const uint32 sizeInMB, IResourceFile* resourceFile)
{
    _cacheSize = sizeInMB * 1024 * 1024;
    _allocated = 0;
    _resourceFile = resourceFile;
}

ResourceCache::~ResourceCache()
{
    while (!_lruList.empty())
    {
        FreeOneResource();
    }

    SAFE_DELETE(_resourceFile);
}

bool ResourceCache::Init()
{
    if (_resourceFile->VOpen())
    {
        return true;
    }

    return false;
}

void ResourceCache::RegisterLoader(std::shared_ptr<IResourceLoader> loader)
{
    _resourceLoaderList.push_front(loader);
}

std::shared_ptr<ResourceHandle> ResourceCache::GetHandle(Resource* r)
{
    std::shared_ptr<ResourceHandle> handle(Find(r));
    if (handle == NULL)
    {
        handle = Load(r);
        assert(handle);
    }
    else
    {
        Update(handle);
    }

    return handle;
}

std::shared_ptr<ResourceHandle> ResourceCache::Load(Resource* r)
{
    std::shared_ptr<IResourceLoader> loader;
    std::shared_ptr<ResourceHandle> handle;

    for (auto resourceLoader : _resourceLoaderList)
    {
        std::shared_ptr<IResourceLoader> testLoader = resourceLoader;

        if (WildcardMatch(testLoader->VGetPattern().c_str(), r->GetName().c_str()))
        {
            loader = testLoader;
            break;
        }
    }

    if (!loader)
    {
        LOG_ERROR("Default resource loader for resource: " + r->GetName() + " not found");
        return NULL;
    }

    int32 rawSize = _resourceFile->VGetRawResourceSize(r);
    if (rawSize < 0)
    {
        LOG_ERROR("Resource size return -1 => Resource not found. Resource: " + r->GetName());
        return NULL;
    }

    int32 allocSize = rawSize + ((loader->VAddNullZero()) ? (1) : (0));
    char* rawBuffer = loader->VUseRawFile() ? Allocate(allocSize) : new char[allocSize];
    if (rawBuffer == NULL)
    {
        LOG_ERROR("Could not allocate enough memory for resource: " + r->GetName() + 
            " in resource file: " + _resourceFile->VGetName());
        return NULL;
    }
    memset(rawBuffer, 0, allocSize);

    if (_resourceFile->VGetRawResource(r, rawBuffer) < 0)
    {
        LOG_ERROR("Could not retrieve data buffer from resource: " + r->GetName() + 
            " in resource file: " + _resourceFile->VGetName());
        return NULL;
    }

    char* buffer = NULL;
    uint32 size = 0;

    // Just store binary data + size in handle
    if (loader->VUseRawFile())
    {
        buffer = rawBuffer;
        handle = std::shared_ptr<ResourceHandle>(new ResourceHandle(*r, buffer, rawSize, this));
    }
    else // Or store meaningful arbitrary file format
    {
        size = loader->VGetLoadedResourceSize(rawBuffer, rawSize);
        buffer = Allocate(size);
        if (buffer == NULL)
        {
            LOG_ERROR("Could not allocate enough memory for resource: " + r->GetName() +
                " in resource file: " + _resourceFile->VGetName());
            return shared_ptr<ResourceHandle>();
        }
         
        handle = std::shared_ptr<ResourceHandle>(new ResourceHandle(*r, buffer, size, this));
        bool success = loader->VLoadResource(rawBuffer, rawSize, handle);

        if (loader->VDiscardRawBufferAfterLoad())
        {
            SAFE_DELETE_ARRAY(rawBuffer);
        }

        if (!success)
        {
            LOG_ERROR("Could not load resource from raw data");
            return NULL;
        }
    }

    if (!handle)
    {
        LOG_ERROR("Could not load any handle from resource: " + r->GetName() +
            " in resource file: " + _resourceFile->VGetName());
        return NULL;
    }

    _lruList.push_front(handle);
    _resourceMap[r->GetName()] = handle;

    return handle;
}

std::shared_ptr<ResourceHandle> ResourceCache::Find(Resource* r)
{
    return _resourceMap[r->GetName()];
}

void ResourceCache::Update(std::shared_ptr<ResourceHandle> handle)
{
    _lruList.remove(handle);
    _lruList.push_front(handle);
}

char* ResourceCache::Allocate(uint32 size)
{
    if (!MakeRoom(size))
    {
        LOG_WARNING("Out of memory in resource cache");
        return NULL;
    }

    char* mem = new char[size];
    if (mem)
    {
        _allocated += size;
    }

    return mem;
}

void ResourceCache::FreeOneResource()
{
    //LOG("FreeOneResource");
    ResourceHandleList::iterator gonner = _lruList.end();
    gonner--;

    shared_ptr<ResourceHandle> handle = *gonner;

    _lruList.pop_back();
    _resourceMap.erase(handle->GetName());
}

void ResourceCache::Flush()
{
    while (!_lruList.empty())
    {
        std::shared_ptr<ResourceHandle> handle = *(_lruList.begin());
        Free(handle);
        _lruList.pop_front();
    }
}

bool ResourceCache::MakeRoom(uint32 size)
{
    if (size > _cacheSize)
    {
        return false;
    }

    int64 x = _cacheSize - _allocated;
    //LOG("Room left: " + ToStr(x));

    // Return NULL if there is no possibility to allocate memory
    while (size > (_cacheSize - _allocated))
    {
        if (_lruList.empty())
        {
            return false;
        }

        FreeOneResource();
    }

    return true;
}

void ResourceCache::Free(std::shared_ptr<ResourceHandle> gonner)
{
    _lruList.remove(gonner);
    _resourceMap.erase(gonner->GetName());
}

void ResourceCache::MemoryHasBeenFreed(uint32 size)
{
    _allocated -= size;
}

std::vector<std::string> ResourceCache::Match(const std::string pattern)
{
    std::vector<std::string> matchingNames;

    if (_resourceFile == NULL)
    {
        return matchingNames;
    }

    // Everything is converted into lower case so maintain consistency
    std::string patternCopy = pattern;
    std::transform(patternCopy.begin(), patternCopy.end(), patternCopy.begin(), (int(*)(int)) std::tolower);

    uint32 numFiles = _resourceFile->VGetNumResources();
    for (uint32 fileIdx = 0; fileIdx < numFiles; ++fileIdx)
    {
        std::string fileNamePath = _resourceFile->VGetResourceName(fileIdx);
        // Everything is converted into lower case so maintain consistency
        std::transform(fileNamePath.begin(), fileNamePath.end(), fileNamePath.begin(), (int(*)(int)) std::tolower);

        //std::cout << "Filename: " << fileNamePath << ", Pattern: " << pattern << std::endl;

        if (WildcardMatch(patternCopy.c_str(), fileNamePath.c_str()))
        {
            matchingNames.push_back(fileNamePath);
        }
    }

    return matchingNames;
}
using namespace std;
int32 ResourceCache::Preload(const std::string pattern, void(*progressCallback)(int32, bool &))
{
    if (_resourceFile == NULL)
    {
        return 0;
    }
        
    int32 numFiles = _resourceFile->VGetNumResources();
    int32 loaded = 0;
    bool cancel = false;

    // Everything is converted into lower case so maintain consistency
    std::string patternCopy = pattern;
    std::transform(patternCopy.begin(), patternCopy.end(), patternCopy.begin(), (int(*)(int)) std::tolower);

    for (int32 fileIdx = 0; fileIdx < numFiles; ++fileIdx)
    {
        Resource resource(_resourceFile->VGetResourceName(fileIdx));
        //cout << "Checking pattern for resource: " << resource.GetName() << endl;

        if (WildcardMatch(patternCopy.c_str(), resource.GetName().c_str()))
        {
            // This loads unloaded resource and skips loaded ones
            GetHandle(&resource);
            ++loaded;
        }

        if (progressCallback != NULL)
        {
            progressCallback((fileIdx / numFiles) * 100, cancel);
        }
    }

    return loaded;
}

std::vector<std::string> ResourceCache::GetAllFilesInDirectory(const char* directoryPath)
{
    return _resourceFile->GetAllFilesInDirectory(directoryPath);
}