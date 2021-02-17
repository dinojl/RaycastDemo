#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <sstream>



class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "Example";
	}

	// var declaration
	float playerX;
	float playerY;
	float playerA;
	float FOV;
	float maxDepth;

	int mapWidth = 16;
	int mapHeight = 16;
	std::string map;

public:
	bool OnUserCreate() override
	{
		playerX = 8.0f;
		playerY = 8.0f;
		//playerA = 0.0f;
		playerA = 3.14159 / 4;
		FOV = 3.14159 / 4;
		maxDepth = 16.0f;

		map += "################";
		map += "#              #";
		map += "#              #";
		map += "#              #";
		map += "#          #   #";
		map += "#          #   #";
		map += "#              #";
		map += "#              #";
		map += "#              #";
		map += "#              #";
		map += "#              #";
		map += "#              #";
		map += "#         ######";
		map += "#              #";
		map += "#              #";
		map += "################";
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::A).bHeld)
			playerA -= 1.5 * fElapsedTime;
		if (GetKey(olc::D).bHeld)
			playerA += 1.5 * fElapsedTime;
		if (GetKey(olc::W).bHeld) {
			playerX += sinf(playerA) * 5 * fElapsedTime;
			playerY += cosf(playerA) * 5 * fElapsedTime;
			// Collision detection
			if (map[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX -= sinf(playerA) * 5 * fElapsedTime;
				playerY -= cosf(playerA) * 5 * fElapsedTime;
			}
		}
		if (GetKey(olc::S).bHeld) {
			playerX -= sinf(playerA) * 5 * fElapsedTime;
			playerY -= cosf(playerA) * 5 * fElapsedTime;
			// Collision detection
			if (map[(int)playerY * mapWidth + (int)playerX] == '#') {
				playerX += sinf(playerA) * 5 * fElapsedTime;
				playerY += cosf(playerA) * 5 * fElapsedTime;
			}
		}

		// Ray cast rendering
		for (int x = 0; x < ScreenWidth(); x++) {
			float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)ScreenWidth()) * FOV;
			
			float distanceToWall = 0;
			bool hitWall = false;
			bool boundary = false;
			
			float eyeX = sinf(rayAngle);
			float eyeY = cosf(rayAngle);

			while (!hitWall && distanceToWall < maxDepth) {
				distanceToWall += 0.1f;

				int testX = (int)(playerX + eyeX * distanceToWall);
				int testY = (int)(playerY + eyeY * distanceToWall);

				// test if ray is out of bounds
				if (testX < 0 || testX > mapWidth || testY < 0 || testY > mapHeight) {
					hitWall = true;            // Set ray to max depth
					distanceToWall = maxDepth;
				}
				else {
					// Ray is in bounds so test to see if it hit a wall
					if (map[testY * mapWidth + testX] == '#') {
						hitWall = true;
					}
				}
			}

			// calculate distance to ceiling and floor
			int ceiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceToWall);
			int floor = ScreenHeight() - ceiling;

			// Shading
			olc::Pixel Shade = (0, 0, 0);

			if (distanceToWall <= maxDepth / 4)       Shade = olc::WHITE;
			else if (distanceToWall < maxDepth / 3)   Shade = olc::GREY;
			else if (distanceToWall < maxDepth / 2)   Shade = olc::DARK_GREY;
			else if (distanceToWall < maxDepth)       Shade = olc::VERY_DARK_GREY;
			else                                      Shade = olc::BLACK;

			for (int y = 0; y < ScreenHeight(); y++) {
				if (y < ceiling)
					Draw(x, y, olc::BLACK);
				else if (y > ceiling && y < floor)
					Draw(x, y, Shade);
				else {
					// Shade floor based on distance
					olc::Pixel ShadeF = (0, 0, 0);
					float b = 1.0f - (((float)y - ScreenHeight() / 2.0f) / ((float)ScreenHeight() / 2.0f));
					if (b < 0.25)                      ShadeF = olc::GREEN;
					else if (b < 0.5)                  ShadeF = olc::DARK_GREEN;
					else if (b < 0.75)                 ShadeF = olc::VERY_DARK_GREEN;
					else if (b < 0.9)                  ShadeF = olc::VERY_DARK_CYAN;
					else                               ShadeF = olc::BLACK;
					Draw(x, y, ShadeF);
				}
			}

		}
		return true;
	}
};

int main()
{
	Game game;
	const bool HR = true;
	if (!HR) {
		if (game.Construct(120, 40, 8, 16))
			game.Start();
	}
	else {
		if (game.Construct(480, 320, 2, 2))
			game.Start();
	}
	return 0;
}
