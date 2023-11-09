# Bullet Hell

This project is a top-down shooter game written in C++. It consists of two main projects, the game and the asset packer for the game.

## Asset Packer

This application processes assets from the Assets folder, processes some of them (like models, animations, materials, and textures) into a format that is 
compact and faster to load into memory, and zips everything up into a single asset file.

## Bullet Hell

This is all of the game logic and rendering code for the game. It includes things like the main game loop, a 3D OpenGL deferred rendering pipeline,
event system, and a resource cache.

## Building

The games are built using visual studio's compiler. If changes are made to third party libraries, there are scripts for building all of the third party 
libaries and exporting to the library folders for various targets.

The game requires specific assets, which are excluded from git due to size and frequency of changes, so it should compile but won't run out of the repo.
Once the game is stable the asset zips may be made available somewhere, but for now only the source code is provided.
