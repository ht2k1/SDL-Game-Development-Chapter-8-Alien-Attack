#include <string>
#include "LevelParser.h"
#include "TextureManager.h"
#include "Game.h"
#include "ObjectLayer.h"
#include "TileLayer.h"
#include "GameObjectFactory.h"
#include "base64.h"
#include "zlib.h"
#include "Level.h"
#include "TileLayer.h"
#include "XMLAssetLoader.h"
#include <iostream>
#include <memory>

Level* LevelParser::parseLevel(const char *levelFile) {
    TiXmlDocument levelDocument;
    LoadXMLFromAssets(levelDocument, levelFile);

    // 1. SAFETY CHECK: Did the XML file actually load from APK assets?
    TiXmlElement* pRoot = levelDocument.RootElement();
    if (!pRoot) {
        std::cout << "CRITICAL ERROR: Failed to load or parse level file from assets: " << (levelFile ? levelFile : "NULL") << std::endl;
        return nullptr;
    }

    Level* pLevel = new Level();

    // 2. SAFE PRINTING: Use safeGetAttribute so std::cout never receives a nullptr!
    // Notice we also fixed the "tilewidth" lowercase typo here:
    std::cout << "Loading level:\n" 
              << "Version: " << safeGetAttribute(pRoot, "version", "unknown") << "\n"
              << "Width: " << safeGetAttribute(pRoot, "width", "0") 
              << " - Height: " << safeGetAttribute(pRoot, "height", "0") << "\n"
              << "Tile width: " << safeGetAttribute(pRoot, "tilewidth", "0") 
              << " - Tile Height: " << safeGetAttribute(pRoot, "tileheight", "0") << std::endl;

    pRoot->Attribute("tilewidth", &m_tileSize);
    pRoot->Attribute("width", &m_width);
    pRoot->Attribute("height", &m_height);

    // 3. SAFE PROPERTY PARSING: We loop through children safely instead of assuming 
    // that pRoot->FirstChildElement() is guaranteed to be <properties>
    for (TiXmlElement* e = pRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
        if (e->Value() == std::string("properties")) {
            for (TiXmlElement* property = e->FirstChildElement(); property != NULL; property = property->NextSiblingElement()) {
                if (property->Value() == std::string("property")) {
                    parseTextures(property);
                }
            }
        }
    }

    // Parse tilesets
    for (TiXmlElement* e = pRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
        if (e->Value() == std::string("tileset")) {
            parseTilesets(e, pLevel->getTilesets());
        }
    }

    // Parse layers and object layers safely
    for (TiXmlElement* e = pRoot->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
        if (e->Value() == std::string("objectgroup") || e->Value() == std::string("layer")) {
            if (e->FirstChildElement() && e->FirstChildElement()->Value() == std::string("object")) {
                parseObjectLayer(e, pLevel->getLayers(), pLevel);
            }
            else if (e->FirstChildElement() && (e->FirstChildElement()->Value() == std::string("data") ||
                    (e->FirstChildElement()->NextSiblingElement() != 0 && e->FirstChildElement()->NextSiblingElement()->Value() == std::string("data")))) {
                parseTileLayer(e, pLevel->getLayers(), pLevel->getTilesets(), pLevel->getCollisionLayers());
            }
        }
    }

    return pLevel;
}


void LevelParser::parseTextures(TiXmlElement* pTextureRoot) {
    std::cout << "adding texture " << pTextureRoot->Attribute("value") << " with ID " << pTextureRoot->Attribute("name") << std::endl;
    TheTextureManager::Instance()->load(pTextureRoot->Attribute("value"), pTextureRoot->Attribute("name"), TheGame::Instance()->getRenderer());
}

void LevelParser::parseTilesets(TiXmlElement* pTilesetRoot, std::vector<Tileset>* pTilesets) {
    std::string assetsTag = "assets/";

    // Safely grab the image source and tileset name
    TiXmlElement* pImageElement = pTilesetRoot->FirstChildElement();
    std::string source = safeGetAttribute(pImageElement, "source");
    std::string name = safeGetAttribute(pTilesetRoot, "name", "default_tileset");

    std::cout << "adding texture " << source << " with ID " << name << std::endl;
    
    // Append once and load safely
    TheTextureManager::Instance()->load(assetsTag.append(source), name, TheGame::Instance()->getRenderer()); 

    // Create and populate the tileset object
    Tileset tileset;
    if (pImageElement) {
        pImageElement->Attribute("width", &tileset.width);
        pImageElement->Attribute("height", &tileset.height);
    }
    pTilesetRoot->Attribute("firstgid", &tileset.firstGridID);
    pTilesetRoot->Attribute("tilewidth", &tileset.tileWidth);
    pTilesetRoot->Attribute("tileheight", &tileset.tileHeight);
    pTilesetRoot->Attribute("spacing", &tileset.spacing);
    pTilesetRoot->Attribute("margin", &tileset.margin);
    
    tileset.name = name; // Use our safely parsed name
    
    // Prevent division by zero if spacing/tileWidth failed to load
    int denom = (tileset.tileWidth + tileset.spacing);
    tileset.numColumns = (denom > 0) ? (tileset.width / denom) : 1;

    pTilesets->push_back(tileset);
}

void LevelParser::parseObjectLayer(TiXmlElement* pObjectElement, std::vector<Layer*> *pLayers, Level* pLevel) {
    ObjectLayer* pObjectLayer = new ObjectLayer();

    for (TiXmlElement* e = pObjectElement->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
        if (e->Value() == std::string("object")) {
            int x = 0, y = 0, width = 0, height = 0, numFrames = 0, callbackID = 0, animSpeed = 0;
            std::string textureID;
            std::string type;

            // Get the initial node values safely
            e->Attribute("x", &x);
            e->Attribute("y", &y);
            type = safeGetAttribute(e, "type", "DefaultType");

            GameObject* pGameObject = TheGameObjectFactory::Instance()->create(type);
            if (!pGameObject) {
                std::cout << "Error: Failed to create GameObject of type: " << type << std::endl;
                continue; // Skip if factory failed so we don't crash below
            }

            // Get the property values
            for (TiXmlElement* properties = e->FirstChildElement(); properties != NULL; properties = properties->NextSiblingElement()) {
                if (properties->Value() == std::string("properties")) {
                    for (TiXmlElement* property = properties->FirstChildElement(); property != NULL; property = property->NextSiblingElement()) {
                        if (property->Value() == std::string("property")) {
                            
                            // Safely check the property name
                            std::string propName = safeGetAttribute(property, "name");

                            if (propName == "numFrames") {
                                property->Attribute("value", &numFrames);
                            }
                            else if (propName == "textureHeight") {
                                property->Attribute("value", &height);
                            }
                            else if (propName == "textureID") {
                                // Use safeGetAttribute instead of raw pointer
                                textureID = safeGetAttribute(property, "value");
                            }
                            else if (propName == "textureWidth") {
                                property->Attribute("value", &width);
                            }
                            else if (propName == "callbackID") {
                                property->Attribute("value", &callbackID);
                            }
                            else if (propName == "animSpeed") { // Fixed: using propName instead of e->Attribute("name")
                                property->Attribute("value", &animSpeed);
                            }
                        }
                    }
                }
            }

            pGameObject->load(std::unique_ptr<LoaderParams>(new LoaderParams(x, y, width, height, textureID, numFrames, callbackID, animSpeed)));

            if (type == "Player") {
                pLevel->setPlayer(dynamic_cast<Player*>(pGameObject));
            }

            pObjectLayer->getGameObjects()->push_back(pGameObject);
        }
    }
    pLayers->push_back(pObjectLayer);
}

void LevelParser::parseTileLayer(TiXmlElement* pTileElement, std::vector<Layer*> *pLayers, const std::vector<Tileset>* pTilesets, std::vector<TileLayer*> *pCollisionLayers) {
    TileLayer* pTileLayer = new TileLayer(m_tileSize, *pTilesets);

    bool collidable = false;

    // tile data
    std::vector<std::vector<int>> data;

    std::string decodedIDs;
    TiXmlElement* pDataNode = nullptr;

    for (TiXmlElement* e = pTileElement->FirstChildElement(); e != NULL; e = e->NextSiblingElement()) {
        if (e->Value() == std::string("properties")) {
            for (TiXmlElement* property = e->FirstChildElement(); property != NULL; property = property->NextSiblingElement()) {
                if (property->Value() == std::string("property")) {
                    if (property->Attribute("name") == std::string("collidable")) {
                        collidable = true;
                    }
                }
            }
        }

        if (e->Value() == std::string("data")) {
            pDataNode = e;
        }
    }

    for (TiXmlNode* e = pDataNode->FirstChild(); e != NULL; e = e->NextSibling()) {
        TiXmlText* text = e->ToText();
        if (text) {
            std::string t = text->Value();
            decodedIDs = base64_decode(t);
        }
    }

    if (!pDataNode) {
    std::cout << "Error: No <data> node found in tile layer!" << std::endl;
    return;
    }

    // uncompress zlib compression
    uLongf sizeofids = m_width * m_height * sizeof(int);
    std::vector<int> ids(m_width * m_height);
    uncompress((Bytef*)&ids[0], &sizeofids, (const Bytef*)decodedIDs.c_str(), decodedIDs.size());

    std::vector<int> layerRow(m_width);

    for (int j = 0; j < m_height; j++) {
        data.push_back(layerRow);
    }

    for (int rows = 0; rows < m_height; rows++) {
        for (int cols = 0; cols < m_width; cols++) {
            data[rows][cols] = ids[rows * m_width + cols];
        }
    }

    pTileLayer->setTileIDs(data);
    pTileLayer->setMapWidth(m_width);

    if (collidable) {
        pCollisionLayers->push_back(pTileLayer);
    }

    pLayers->push_back(pTileLayer);
}