#ifndef __LevelParser__
#define __LevelParser__

#include <iostream>
#include <vector>
#include "tinyxml.h"

class Level;
struct Tileset;
class Layer;
class TileLayer;

class LevelParser {
    public:
        Level* parseLevel(const char* stateFile);

    private:
        void parseTextures(TiXmlElement* pTextureRoot);
        void parseTilesets(TiXmlElement* pTilesetRoot, std::vector<Tileset>* pTilesets);
        void parseObjectLayer(TiXmlElement* pObjectElement, std::vector<Layer*> *pLayers, Level* pLevel);
        void parseTileLayer(TiXmlElement* pTileElement, std::vector<Layer*> *pLayers, const std::vector<Tileset>* pTilesets, std::vector<TileLayer*> *pCollisionLayers);
        int m_tileSize;
        int m_width;
        int m_height;

        std::string safeGetAttribute(TiXmlElement* element, const char* attributeName, const std::string& defaultValue = "") {
            if (!element) return defaultValue;
            const char* val = element->Attribute(attributeName);
            return (val != nullptr) ? std::string(val) : defaultValue;
        }
};

#endif