# Bullet Hell

This project is a top-down shooter game written in C++. It consists of two main projects, the game and the asset packer for the game.

## Asset Packer

This application processes assets from the Assets folder, processes some of them (like models, animations, materials, and textures) into a format that is 
compact and faster to load into memory, and zips everything up into a single asset file.

## Bullet Hell

This is all of the game logic and rendering code for the game. It includes things like the main game loop, a 3D OpenGL deferred rendering pipeline,
event system, and a resource cache.

## Building

The game is compiled using visual studio and only supports Windows. To build and run the game, please follow the below steps.

1. Run either `./regenerate-project.bat` or `./regenerate-project.sh` from the command line
2. Open the generated solution located at `bin/BulletHell.sln`
3. Build all (Ctrl+Shift+B)
4. Copy the assets.zip file from the latest release into the appropriate `bin/Debug` or `bin/Release` directory
5. Run the game

The game requires specific assets, which are excluded from git due to size and frequency of changes, so it should compile but won't run out of the repo.
The latest zip file can be found on the latest release in GitHub. It can also be rebuilt using the provided source assets.
