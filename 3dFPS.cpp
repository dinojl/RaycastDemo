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

	int mapWidth = 32;
	int mapHeight = 32;
	std::string map;
	olc::Sprite *spriteWall;

public:
	bool OnUserCreate() override
	{
		playerX = 8.0f;
		playerY = 8.0f;
		playerA = 0.0f;
		FOV = 3.14159 / 4;
		maxDepth = 16.0f;

		map += "################################";
		map += "#...............#..............#";
		map += "#.......#########.......########";
		map += "#..............##..............#";
		map += "#......##......##......##......#";
		map += "#......##..............##......#";
		map += "#..............##..............#";
		map += "###............####............#";
		map += "##.............###.............#";
		map += "#............####............###";
		map += "#..............................#";
		map += "#..............##..............#";
		map += "#..............##..............#";
		map += "#...........#####...........####";
		map += "#..............................#";
		map += "###..####....########....#######";
		map += "####.####.......######.........#";
		map += "#...............#..............#";
		map += "#.......#########.......##..####";
		map += "#..............##..............#";
		map += "#......##......##.......#......#";
		map += "#......##......##......##......#";
		map += "#..............##..............#";
		map += "###............####............#";
		map += "##.............###.............#";
		map += "#............####............###";
		map += "#..............................#";
		map += "#..............................#";
		map += "#..............##..............#";
		map += "#...........##..............####";
		map += "#..............##..............#";
		map += "################################";

		spriteWall = new olc::Sprite("./gfx/wall_sprite.png");
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Movement
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
		if (GetKey(olc::E).bHeld)
		{
			playerX += cosf(playerA) * 5 * fElapsedTime;
			playerY -= sinf(playerA) * 5 * fElapsedTime;
			if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#')
			{
				playerX -= cosf(playerA) * 5 * fElapsedTime;
				playerY += sinf(playerA) * 5 * fElapsedTime;
			}
		}
		if (GetKey(olc::Q).bHeld)
		{
			playerX -= cosf(playerA) * 5 * fElapsedTime;
			playerY += sinf(playerA) * 5 * fElapsedTime;
			if (map.c_str()[(int)playerX * mapWidth + (int)playerY] == '#')
			{
				playerX += cosf(playerA) * 5 * fElapsedTime;
				playerY -= sinf(playerA) * 5 * fElapsedTime;
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
			float sampleX = 0.0f;

			while (!hitWall && distanceToWall < maxDepth) {
				distanceToWall += 0.01f;

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

						// Determine where ray hit wall
						float blockMidX = (float)testX + 0.5f;
						float blockMidY = (float)testY + 0.5f;

						float testPointX = playerX + eyeX * distanceToWall;
						float testPointY = playerY + eyeY * distanceToWall;
						
						float testAngle = atan2f((testPointY - blockMidY), (testPointX = blockMidX));

						if (testAngle >= -3.14159f * 0.25f && testAngle < 3.14159f * 0.25f)
							sampleX = testPointY - (float)testY;
						if (testAngle >= 3.14159f * 0.25f && testAngle < 3.14159f * 0.75f)
							sampleX = testPointX - (float)testX;
						if (testAngle < -3.14159f * 0.25f && testAngle >= -3.14159f * 0.75f)
							sampleX = testPointX - (float)testX;
						if (testAngle >= 3.14159f * 0.75f || testAngle < -3.14159f * 0.75f)
							sampleX = testPointY - (float)testY;
					}
				}
			}

			// calculate distance to ceiling and floor
			int ceiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)distanceToWall);
			int floor = ScreenHeight() - ceiling;




			// Draw screen with 3D effect
			for (int y = 0; y < ScreenHeight(); y++) {
				if (y < ceiling)
					Draw(x, y, olc::BLACK);
				else if (y >= ceiling && y < floor) {
					if (distanceToWall < maxDepth) {
						float sampleY = ((float)y - (float)ceiling) / ((float)floor - (float)ceiling);
						Draw(x, y, spriteWall->olc::Sprite::Sample(sampleX, sampleY));
					}
					else
						Draw(x, y, olc::BLACK);
				}
				else {
					Draw(x, y, olc::DARK_GREEN);
					//// Shade floor based on distance
					//olc::Pixel ShadeF = (0, 0, 0);
					//float b = 1.0f - (((float)y - ScreenHeight() / 2.0f) / ((float)ScreenHeight() / 2.0f));
					//if (b < 0.25)                      ShadeF = olc::GREEN;
					//else if (b < 0.5)                  ShadeF = olc::DARK_GREEN;
					//else if (b < 0.75)                 ShadeF = olc::VERY_DARK_GREEN;
					//else if (b < 0.9)                  ShadeF = olc::VERY_DARK_CYAN;
					//else                               ShadeF = olc::BLACK;
					//Draw(x, y, ShadeF);
				}
			}

		}
		return true;
	}
};

int main()
{
	// Enables High Resolution mode
	Game game;
	const bool HR = false;
	if (!HR) {
		if (game.Construct(320, 240, 4, 4))
			game.Start();
	}
	else {
		if (game.Construct(1280, 720, 1, 1))
			game.Start();
	}
	return 0;
}
