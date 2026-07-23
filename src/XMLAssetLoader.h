#ifndef __XMLAssetLoader__
#define __XMLAssetLoader__

// TiXmlDocument::LoadFile() calls plain fopen() under the hood (see
// TiXmlFOpen in tinyxml.cpp). fopen() has no knowledge of an Android
// APK's asset store, so on Android it will always fail to find files
// like "assets/map1.tmx", even though the exact same relative path
// works fine for IMG_Load/Mix_LoadMUS/Mix_LoadWAV (those go through
// SDL_RWFromFile, which SDL redirects to the Android AssetManager for
// relative paths).
//
// This helper reads the file through SDL_RWops instead, then hands the
// buffer to TiXmlDocument::Parse(). That makes XML loading work
// identically on desktop and Android without touching the game logic
// that follows.

#include "tinyxml.h"
#include "SDL.h"
#include <vector>
#include <iostream>

inline bool LoadXMLFromAssets(TiXmlDocument &doc, const char *filename) {
    SDL_RWops *pFile = SDL_RWFromFile(filename, "rb");

    if (pFile == nullptr) {
        std::cerr << "Could not open " << filename << ": " << SDL_GetError() << "\n";
        return false;
    }

    Sint64 size = SDL_RWsize(pFile);

    if (size <= 0) {
        std::cerr << "Could not determine size of " << filename << "\n";
        SDL_RWclose(pFile);
        return false;
    }

    std::vector<char> buffer(static_cast<size_t>(size) + 1);

    Sint64 bytesRead = SDL_RWread(pFile, buffer.data(), 1, static_cast<size_t>(size));
    SDL_RWclose(pFile);

    if (bytesRead != size) {
        std::cerr << "Short read on " << filename << "\n";
        return false;
    }

    buffer[static_cast<size_t>(size)] = '\0';

    doc.Parse(buffer.data());

    if (doc.Error()) {
        std::cerr << doc.ErrorDesc() << "\n";
        return false;
    }

    return true;
}

#endif
