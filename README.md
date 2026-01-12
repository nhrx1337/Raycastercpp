# Raycastercpp

## 🚀 The Engine
Raycastercpp is a lightweight, performance-oriented 3D rendering engine written in **C++** using **SDL2**. It implements the **DDA (Digital Differential Analyzer)** algorithm to project a 2D grid-based map into a pseudo-3D environment. The project is designed to be a clean, dependency-light educational tool for understanding retrostyle graphics and 2D-to-3D projection mathematics.

## 📥 Installation
1. Clone the repository:
```bash
git clone https://github.com/nhrx1337/raycastercpp.git
cd raycastercpp/
```
2. Install build dependencies:
### Debian-based
```bash
sudo apt-get install libsdl2-dev
```
### Arch-based
```bash
sudo pacman -S sdl2
```
3. Build:
```bash
make
```
## 🎮 How to Play
Run the engine:
```bash
./raycastercpp
```
- W / S: Move forward and backward.
- A / D: Strafe left and right (sliding perpendicular to direction).
- Mouse / Left-Right Arrows: Rotate the camera view.
- ESC: Exit
The program displays a real-time minimap in the top-left corner to visualize the player's position.
## ⚙️Technical Deep Dive
- DDA Algorithm: Uses efficient grid-stepping to calculate wall hits, ensuring high performance even at higher resolutions.
- Fisheye Correction: Uses perpendicular wall distance projection to prevent wall distortion at the edges of the FOV.
- Shading: Implements a simple shadow effect by darkening walls based on their orientation (North/South vs. East/West).
- Collision Detection: Basic AABB collision checks prevent the player from walking through wall tiles.
## 📦 Requirements
- SDL2 (Simple DirectMedia Layer)
- C++11 or higher
## 📚 References
- [Lodev's Raycasting Tutorial](https://lodev.org/cgtutor/raycasting.html)
