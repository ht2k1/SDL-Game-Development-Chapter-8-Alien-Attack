/*
#include "CollisionManager.h"
#include "Collision.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletHandler.h"
#include "TileLayer.h"
#include "GameObject.h"

void CollisionManager::checkPlayerEnemyBulletCollision(Player* pPlayer) {
    SDL_Rect* pRect1 = new SDL_Rect();
    pRect1->x = pPlayer->getPosition().getX();
    pRect1->y = pPlayer->getPosition().getY();
    pRect1->w = pPlayer->getWidth();
    pRect1->h = pPlayer->getHeight();

    for (int i = 0; i < TheBulletHandler::Instance()->getEnemyBullets().size(); i++) {
        EnemyBullet* pEnemyBullet = TheBulletHandler::Instance()->getEnemyBullets()[i];

        SDL_Rect* pRect2 = new SDL_Rect();
        pRect2->x = pEnemyBullet->getPosition().getX();
        pRect2->y = pEnemyBullet->getPosition().getY();

        pRect2->w = pEnemyBullet->getWidth();
        pRect2->h = pEnemyBullet->getHeight();

        if (RectRect(pRect1, pRect2)) {
            if (!pPlayer->dying() && !pEnemyBullet->dying()) {
                pEnemyBullet->collision();
                pPlayer->collision();
            }
        }

        delete pRect2;
    }
    delete pRect1;
}

void CollisionManager::checkEnemyPlayerBulletCollision(const std::vector<GameObject *> &objects) {
    for (int i = 0; i < objects.size(); i++) {
        GameObject* pObject = objects[i];

        for (int j = 0; j < TheBulletHandler::Instance()->getPlayerBullets().size(); j++) {
            if (pObject->type() != std::string("Enemy") || !pObject->updating()) {
                continue;
            }

            SDL_Rect* pRect1 = new SDL_Rect();
            pRect1->x = pObject->getPosition().getX();
            pRect1->y = pObject->getPosition().getY();
            pRect1->w = pObject->getWidth();
            pRect1->h = pObject->getHeight();

            PlayerBullet* pPlayerBullet = TheBulletHandler::Instance()->getPlayerBullets()[j];

            SDL_Rect* pRect2 = new SDL_Rect();
            pRect2->x = pPlayerBullet->getPosition().getX();
            pRect2->y = pPlayerBullet->getPosition().getY();
            pRect2->w = pPlayerBullet->getWidth();
            pRect2->h = pPlayerBullet->getHeight();

            if (RectRect(pRect1, pRect2)) {
                if (!pObject->dying() && !pPlayerBullet->dying()) {
                    pPlayerBullet->collision();
                    pObject->collision();
                }
            }

            delete pRect1;
            delete pRect2;
        }
    }
}

void CollisionManager::checkPlayerEnemyCollision(Player* pPlayer, const std::vector<GameObject*> &objects) {
    SDL_Rect* pRect1 = new SDL_Rect();
    pRect1->x = pPlayer->getPosition().getX();
    pRect1->y = pPlayer->getPosition().getY();
    pRect1->w = pPlayer->getWidth();
    pRect1->h = pPlayer->getHeight();

    for (int i = 0; i < objects.size(); i++) {
        if (objects[i]->type() != std::string("Enemy") || !objects[i]->updating()) {
            continue;
        }

        SDL_Rect* pRect2 = new SDL_Rect();
        pRect2->x = objects[i]->getPosition().getX();
        pRect2->y = objects[i]->getPosition().getY();
        pRect2->w = objects[i]->getWidth();
        pRect2->h = objects[i]->getHeight();

        if (RectRect(pRect1, pRect2)) {
            if (!objects[i]->dead() && !objects[i]->dying()) {
                pPlayer->collision();
            }
        }
        delete pRect2;
    }
    delete pRect1;
}

void CollisionManager::checkPlayerTileCollision(Player* pPlayer, const std::vector<TileLayer*>& collisionLayers) {
    // iterate through collision layers
    for (std::vector<TileLayer*>::const_iterator it = collisionLayers.begin(); it != collisionLayers.end(); ++it) {
        TileLayer* pTileLayer = (*it);
        std::vector<std::vector<int>> tiles = pTileLayer->getTileIDs();

        // get this layers position
        Vector2D layerPos = pTileLayer->getPosition();

        int x, y, tileColumn, tileRow, tileid = 0;

        // calculate position on tile map
        x = layerPos.getX() / pTileLayer->getTileSize();
        y = layerPos.getY() / pTileLayer->getTileSize();

        // if moving forward or upwards
        if (pPlayer->getVelocity().getX() >= 0 || pPlayer->getVelocity().getY() >= 0) {
            tileColumn = ((pPlayer->getPosition().getX() + pPlayer->getWidth()) / pTileLayer->getTileSize());
            tileRow = ((pPlayer->getPosition().getY() + pPlayer->getHeight()) / pTileLayer->getTileSize());
            tileid = tiles[tileRow + y][tileColumn + x];
        }
        else if (pPlayer->getVelocity().getX() < 0 || pPlayer->getVelocity().getY() < 0) {
            tileColumn = pPlayer->getPosition().getX() / pTileLayer->getTileSize();
            tileRow = pPlayer->getPosition().getY() / pTileLayer->getTileSize();
            tileid = tiles[tileRow + y][tileColumn + x];
        }

        if (tileid != 0) {
            pPlayer->collision();
        }
    }
}
*/

#include "CollisionManager.h"
#include "Collision.h"
#include "Player.h"
#include "Enemy.h"
#include "BulletHandler.h"
#include "TileLayer.h"
#include "GameObject.h"

void CollisionManager::checkPlayerEnemyBulletCollision(Player* pPlayer) {
    // Allocation optimization: Create SDL_Rect on the stack instead of heap
    SDL_Rect rect1;
    rect1.x = pPlayer->getPosition().getX();
    rect1.y = pPlayer->getPosition().getY();
    rect1.w = pPlayer->getWidth();
    rect1.h = pPlayer->getHeight();

    for (size_t i = 0; i < TheBulletHandler::Instance()->getEnemyBullets().size(); i++) {
        EnemyBullet* pEnemyBullet = TheBulletHandler::Instance()->getEnemyBullets()[i];

        SDL_Rect rect2;
        rect2.x = pEnemyBullet->getPosition().getX();
        rect2.y = pEnemyBullet->getPosition().getY();
        rect2.w = pEnemyBullet->getWidth();
        rect2.h = pEnemyBullet->getHeight();

        if (RectRect(&rect1, &rect2)) {
            if (!pPlayer->dying() && !pEnemyBullet->dying()) {
                pEnemyBullet->collision();
                pPlayer->collision();
            }
        }
    }
}

void CollisionManager::checkEnemyPlayerBulletCollision(const std::vector<GameObject *> &objects) {
    for (size_t i = 0; i < objects.size(); i++) {
        GameObject* pObject = objects[i];

        for (size_t j = 0; j < TheBulletHandler::Instance()->getPlayerBullets().size(); j++) {
            if (pObject->type() != std::string("Enemy") || !pObject->updating()) {
                continue;
            }

            SDL_Rect rect1;
            rect1.x = pObject->getPosition().getX();
            rect1.y = pObject->getPosition().getY();
            rect1.w = pObject->getWidth();
            rect1.h = pObject->getHeight();

            PlayerBullet* pPlayerBullet = TheBulletHandler::Instance()->getPlayerBullets()[j];

            SDL_Rect rect2;
            rect2.x = pPlayerBullet->getPosition().getX();
            rect2.y = pPlayerBullet->getPosition().getY();
            rect2.w = pPlayerBullet->getWidth();
            rect2.h = pPlayerBullet->getHeight();

            if (RectRect(&rect1, &rect2)) {
                if (!pObject->dying() && !pPlayerBullet->dying()) {
                    pPlayerBullet->collision();
                    pObject->collision();
                }
            }
        }
    }
}

void CollisionManager::checkPlayerEnemyCollision(Player* pPlayer, const std::vector<GameObject*> &objects) {
    SDL_Rect rect1;
    rect1.x = pPlayer->getPosition().getX();
    rect1.y = pPlayer->getPosition().getY();
    rect1.w = pPlayer->getWidth();
    rect1.h = pPlayer->getHeight();

    for (size_t i = 0; i < objects.size(); i++) {
        if (objects[i]->type() != std::string("Enemy") || !objects[i]->updating()) {
            continue;
        }

        SDL_Rect rect2;
        rect2.x = objects[i]->getPosition().getX();
        rect2.y = objects[i]->getPosition().getY();
        rect2.w = objects[i]->getWidth();
        rect2.h = objects[i]->getHeight();

        if (RectRect(&rect1, &rect2)) {
            if (!objects[i]->dead() && !objects[i]->dying()) {
                pPlayer->collision();
            }
        }
    }
}

void CollisionManager::checkPlayerTileCollision(Player* pPlayer, const std::vector<TileLayer*>& collisionLayers) {
    // iterate through collision layers
    for (std::vector<TileLayer*>::const_iterator it = collisionLayers.begin(); it != collisionLayers.end(); ++it) {
        TileLayer* pTileLayer = (*it);
        std::vector<std::vector<int>> tiles = pTileLayer->getTileIDs();

        // get this layers position
        Vector2D layerPos = pTileLayer->getPosition();

        int x, y, tileColumn, tileRow, tileid = 0;

        // calculate position on tile map
        x = layerPos.getX() / pTileLayer->getTileSize();
        y = layerPos.getY() / pTileLayer->getTileSize();

        // if moving forward or upwards
        if (pPlayer->getVelocity().getX() >= 0 || pPlayer->getVelocity().getY() >= 0) {
            tileColumn = ((pPlayer->getPosition().getX() + pPlayer->getWidth()) / pTileLayer->getTileSize());
            tileRow = ((pPlayer->getPosition().getY() + pPlayer->getHeight()) / pTileLayer->getTileSize());
        }
        else { // if moving backward or downwards
            tileColumn = pPlayer->getPosition().getX() / pTileLayer->getTileSize();
            tileRow = pPlayer->getPosition().getY() / pTileLayer->getTileSize();
        }

        // --- CRASH PREVENTION ---
        int finalRow = tileRow + y;
        int finalCol = tileColumn + x;

        // Ensure row index is valid and within vector bounds
        if (finalRow >= 0 && finalRow < static_cast<int>(tiles.size())) {
            // Ensure column index is valid within that row
            if (finalCol >= 0 && finalCol < static_cast<int>(tiles[finalRow].size())) {
                tileid = tiles[finalRow][finalCol];
            }
        }

        if (tileid != 0) {
            pPlayer->collision();
        }
    }
}