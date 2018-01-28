cmake_minimum_required(VERSION 3.0)

project(tentaishow)

INCLUDE(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL_TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL_MIX REQUIRED SDL2_mixer)
INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL_TTF_INCLUDE_DIRS} ${SDL_MIX_INCLUDE_DIRS})

set(SRCS
    src/GSpriteButton.c
    src/GSpriteMainMenu.c
    src/GSprite.c
    src/GSpriteNull.c
    src/GGame.c
    src/GSpriteCore.c
    src/GSpritePopup.c
    src/GGraphics.c
    src/GSpriteCredits.c
    src/GSpriteTile.c
    src/GPrefs.c
    src/GSpriteEditor.c
    src/GSolver.c
    src/GSpriteGalaxies.c
    src/GSpriteBoard.c
    src/GSpriteLabel.c
    src/GSpriteBoardGrid.c
    src/GSpriteLevelSelect.c
    src/GSpriteProgress.c
    src/GSpriteOptions.c
    src/main.c
    src/GAudio.c
    )

set(HEADERS
    src/GEvent.h
    src/GSpriteBoard.h
    src/GSpriteLabel.h
    src/GGame.h
    src/GSpriteButton.h
    src/GSpriteLevelSelect.h
    src/GGraphics.h
    src/GSpriteCore.h
    src/GSpriteMainMenu.h
    src/GPrefs.h
    src/GSpriteCredits.h
    src/GSpriteNull.h
    src/GResources.h
    src/GSpriteEditor.h
    src/GSpritePopup.h
    src/GSolver.h
    src/GSpriteGalaxies.h
    src/GSpriteTile.h
    src/GSpriteBoardGrid.h
    src/GSprite.h
    src/GAudio.h
    )

add_executable(tentaishow ${SRCS} ${HEADERS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL_TTF_LIBRARIES} ${SDL_MIX_LIBRARIES})
