#include "raycaster.h"
#include "gameMap.h"
#include <cmath>
#include <algorithm>

Raycaster::Raycaster(int w, int h) : 
    screenWidth(w), screenHeight(h) {
    playerX = 15.0; playerY = 15.0;
    dirX = -1.0;    dirY = 0.0;
    // planeX/Y are perpendicular to the direction; 
    // length determines the field of view
    planeX = 0.0;   planeY = 0.66; 
}

void Raycaster::rotate(double angle) {
    // Standard 2D rotation matrix formula:
    // x' = x*cos(a) - y*sin(a)
    // y' = x*sin(a) + y*cos(a)
    double oldDirX = dirX;
    dirX = dirX * cos(angle) - dirY * sin(angle);
    dirY = oldDirX * sin(angle) + dirY * cos(angle);
    
    double oldPlaneX = planeX;
    planeX = planeX * cos(angle) - planeY * sin(angle);
    planeY = oldPlaneX * sin(angle) + planeY * cos(angle);
}

void Raycaster::handleInput(const Uint8* state, 
        double moveSpeed, double rotSpeed) {
    // Simple AABB collision check: 
    // only move if the target map cell is 0 (empty)
    if (state[SDL_SCANCODE_W]) {
        if (worldMap[int(playerX + dirX * moveSpeed)][int(playerY)] == 0) 
            playerX += dirX * moveSpeed;
        if (worldMap[int(playerX)][int(playerY + dirY * moveSpeed)] == 0) 
            playerY += dirY * moveSpeed;
    }
    if (state[SDL_SCANCODE_S]) {
        if (worldMap[int(playerX - dirX * moveSpeed)][int(playerY)] == 0) 
            playerX -= dirX * moveSpeed;
        if (worldMap[int(playerX)][int(playerY - dirY * moveSpeed)] == 0) 
            playerY -= dirY * moveSpeed;
    }
    // Strafe: move perpendicular to direction using the camera plane vector
    if (state[SDL_SCANCODE_D]) {
        if (worldMap[int(playerX + planeX * moveSpeed)][int(playerY)] == 0) 
            playerX += planeX * moveSpeed;
        if (worldMap[int(playerX)][int(playerY + planeY * moveSpeed)] == 0) 
            playerY += planeY * moveSpeed;
    }
    if (state[SDL_SCANCODE_A]) {
        if (worldMap[int(playerX - planeX * moveSpeed)][int(playerY)] == 0) 
            playerX -= planeX * moveSpeed;
        if (worldMap[int(playerX)][int(playerY - planeY * moveSpeed)] == 0) 
            playerY -= planeY * moveSpeed;
    }

    int mouseX;
    SDL_GetRelativeMouseState(&mouseX, NULL);
    if (mouseX != 0) rotate(-mouseX * 0.002);
    if (state[SDL_SCANCODE_LEFT])  rotate(rotSpeed);
    if (state[SDL_SCANCODE_RIGHT]) rotate(-rotSpeed);
}

SDL_Color Raycaster::getWallColor(int wallType, int side) {
    SDL_Color c;
    switch(wallType) {
        case 1:  c = {200, 0, 0, 255};   break; 
        case 2:  c = {0, 200, 0, 255};   break; 
        case 3:  c = {0, 0, 200, 255};   break; 
        case 4:  c = {150, 150, 150, 255}; break;
        case 5:  c = {200, 200, 0, 255};   break; 
        default: c = {100, 100, 100, 255}; break;
    }
    // Simple shading: darken the color if it's a Y-side wall
    if (side == 1) { c.r /= 1.5; c.g /= 1.5; c.b /= 1.5; } 
    return c;
}

void Raycaster::render(SDL_Renderer* renderer, int w, int h) {
    screenWidth = w; screenHeight = h;

    // Draw static background (Ceiling/Floor)
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_Rect ceilRect = {0, 0, screenWidth, screenHeight / 2};
    SDL_RenderFillRect(renderer, &ceilRect);
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_Rect floorRect = {0, screenHeight / 2, screenWidth, screenHeight / 2};
    SDL_RenderFillRect(renderer, &floorRect);

    // Loop through every vertical strip of the screen
    for (int x = 0; x < screenWidth; x++) {
        // Calculate ray position and direction based on FOV
        
        // x-coordinate in camera space (-1 to +1)
        double cameraX = 2 * x / (double)screenWidth - 1; 
        double rayDirX = dirX + planeX * cameraX;
        double rayDirY = dirY + planeY * cameraX;

        int mapX = int(playerX);
        int mapY = int(playerY);

        // deltaDist: distance the ray has to travel to go from 1 x-side to next x-side
        double deltaDistX = (rayDirX == 0) ? 1e30 : std::abs(1 / rayDirX);
        double deltaDistY = (rayDirY == 0) ? 1e30 : std::abs(1 / rayDirY);

        double sideDistX, sideDistY;
        int stepX, stepY, hit = 0, side;

        // sideDist: distance from ray start to the first grid intersection
        if (rayDirX < 0) {
            stepX = -1;
            sideDistX = (playerX - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - playerX) * deltaDistX;
        }
        if (rayDirY < 0) {
            stepY = -1;
            sideDistY = (playerY - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - playerY) * deltaDistY;
        }

        // DDA loop: jumps to the next grid square
        while (hit == 0) {
            if (sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0; // Hit North/South wall
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1; // Hit East/West wall
            }
            if (worldMap[mapX][mapY] > 0) hit = 1;
        }

        // Calculate distance projected on camera direction (prevents fisheye effect)
        double perpWallDist = (side == 0) ? 
            (sideDistX - deltaDistX) : (sideDistY - deltaDistY);
        
        // Calculate height of line to draw on screen
        int lineHeight = (int)(screenHeight / perpWallDist);

        // Calculate lowest and highest pixel to fill in current stripe
        int drawStart = std::max(0, -lineHeight / 2 + screenHeight / 2);
        int drawEnd = std::min(screenHeight - 1, lineHeight / 2 + screenHeight / 2);

        SDL_Color color = getWallColor(worldMap[mapX][mapY], side);
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
    }
    drawMinimap(renderer);
}

void Raycaster::drawMinimap(SDL_Renderer* renderer) {
    int blockSize = 3; 
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            if (worldMap[x][y] > 0) {
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
                SDL_Rect block = {y * blockSize + 10, 
                    x * blockSize + 10, blockSize, blockSize};
                SDL_RenderFillRect(renderer, &block);
            }
        }
    }
    // Player indicator
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_Rect p = {(int)(playerY * blockSize) + 9, 
        (int)(playerX * blockSize) + 9, 3, 3};
    SDL_RenderFillRect(renderer, &p);
}
