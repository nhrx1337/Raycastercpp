#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL2/SDL.h>

class Raycaster {
private:
    double playerX, playerY;      // Position
    double dirX, dirY;            // Direction vector
    double planeX, planeY;        // Camera plane for FOV
    int screenWidth, screenHeight;

    SDL_Color getWallColor(int wallType, int side);
    void drawMinimap(SDL_Renderer* renderer);

public:
    Raycaster(int w, int h);
    void rotate(double angle);
    void handleInput(const Uint8* keyboardState, double moveSpeed, double rotSpeed);
    void render(SDL_Renderer* renderer, int w, int h);
};

#endif // RAYCASTER_H
