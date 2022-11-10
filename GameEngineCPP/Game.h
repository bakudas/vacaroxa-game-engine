#pragma once
#include "include/SDL_stdinc.h"

struct Vector2
{
	float x;
	float y;
};

class Game
{

public:
	
	Game();

	bool Initialize();

	void RunLoop();

	void Shutdown();

private:

	void ProcessInput();

	void UpdateGame();

	void GenerateOutput();

	class SDL_Window* mWindow;
		
	class SDL_Renderer* mRenderer;

	bool mIsRunning;

	const int thickness = 15;

	Vector2 mBallPos;

	Vector2 mBallVel;

	Vector2 mPaddlePos;

	Uint8 mTicksCount;

	int mPaddleDir;

	float mPaddleVel;

	float paddleH;
	
};

