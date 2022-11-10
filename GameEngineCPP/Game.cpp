#include "Game.h"
#include "Actor.h"
#include "include/SDL.h"


Game::Game() 
{
	mWindow = NULL;

	mIsRunning = true;
	
	mBallPos.x = 1024/2;
	mBallPos.y = 768/2;
	mBallVel.x = -80.0f;
	mBallVel.y = 95.0f;
	
	mPaddlePos.x = 20;
	mPaddlePos.y = 768/2;
	mPaddleVel = 300.0f;
	paddleH = thickness * 8;

	mTicksCount = 0;
}


bool Game::Initialize()
{
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);

	if (sdlResult != 0)
	{
		SDL_Log("Não foi possível inicialiar SDL: %s", SDL_GetError());

		return false;
	}

	mWindow = SDL_CreateWindow(
		"VacaRoxa GameEngine", // titulo da janela
		100, // canto superior esquerdo x-coord 
		100, // canto superior esquerdo y-coord
		1024, // largura da janela
		768, // altura da janela
		0 // flags
	);

	if (!mWindow)
	{
		SDL_Log("Falha ao criar a janela de jogo: %s", SDL_GetError());

		return false;
	}

	
	mRenderer = SDL_CreateRenderer(
		mWindow,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	
	
	return true;
}


void Game::Shutdown()
{
	while (!mActors.empty())
	{
		delete mActors.back();
	}
	
	SDL_DestroyWindow(mWindow);

	SDL_DestroyRenderer(mRenderer);

	SDL_Quit();
}

void Game::AddActor(Actor* actor)
{
	// se ao atualizar os actors, for preciso add para pendente
	if (mUpdatingActors)
	{
		mPendindActors.emplace_back(actor);
	}
	else
	{
		mActors.emplace_back(actor);
	}
}


void Game::ProcessInput()
{

	SDL_Event event;

	// rode enquanto houver eventos na fila

	while (SDL_PollEvent(&event))
	{
		// lidar com os variados tipos de eventos
		switch (event.type)
		{
			case SDL_QUIT:
				mIsRunning = false;
				break;

		}
	}

	// Captura de teclas e estados do teclado

	// array de entradas
	const Uint8* state = SDL_GetKeyboardState(NULL);

	// apertar ESC para fechar o jogo
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}

	// paddle 01 input
	mPaddleDir = 0;

	if (state[SDL_SCANCODE_W])
	{
		mPaddleDir -= 1;
	}
	
	if (state[SDL_SCANCODE_S])
	{
		mPaddleDir += 1;
	}
}


void Game::UpdateGame()
{
	// Garante que haja uma espera de até 16ms desde o último frame 
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16));
	
	// Deltatime é a diferença em ticks desde o último frame renderizado
	// (convertido em segundos)
	float deltatime = (SDL_GetTicks() - mTicksCount) / 1000.0f;

	// restringe o valor máximo de deltatime
	// para evitar valores muito altos em caso de debug, por ex
	if (deltatime > 0.05f) deltatime = 0.05f;
	
	// Atualiza a contagem de tick (para o próximo frame)
	mTicksCount = SDL_GetTicks();

	
	// Atualiza todos os Actors
	mUpdatingActors = true;

	for (auto actor : mActors)
	{
		actor->Update(deltatime);
	}

	mUpdatingActors = false;

	// Mover qualquer Actor pendente para vector mActors
	for (auto pending:mPendindActors)
	{
		mActors.emplace_back(pending);
	}

	mPendindActors.clear();

	// Move qualquer Actor morto para um vector temporário
	std::vector<Actor*> deadActors;

	for (auto actor : mActors)
	{
		if (actor->GetState() == Actor::EDead)
		{
			deadActors.emplace_back(actor);	
		}
	}

	// Deleta os dead Actors
	for (auto actor : deadActors)
	{
		delete actor;
	}

	
	// controla o paddle esquerdo
	if (mPaddleDir != 0)
	{
		mPaddlePos.y += mPaddleDir * mPaddleVel * deltatime;

		if (mPaddlePos.y < (paddleH/2.0f + thickness))
		{
			mPaddlePos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddlePos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPaddlePos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}

	// Movimentação da bola
	mBallPos.x += mBallVel.x * deltatime;
	mBallPos.y += mBallVel.y * deltatime;

	// colisão com a parede superior
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// colisão com a parede inferior
	if (mBallPos.y >= 768 - thickness && mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}
	// colisão com a parede direita
	if (mBallPos.x >= 1024 - thickness && mBallVel.x > 0.0f)
	{
		mBallVel.x *= -1;
	}

	if (mBallPos.x - thickness < 0)
	{
		mBallPos.y = 768/2;
		mBallPos.x = 1024/2;
	}
	
	// diferença entra a posição da bola e do paddle
	float diff = mBallPos.y - mPaddlePos.y;
	//SDL_Log("a diferença é de: %f", diff);

	// colisão com o paddle esquerdo
	if (diff <= paddleH/2.0f && diff >= -paddleH/2.0f &&
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1;
	}
}


void Game::GenerateOutput()
{
	// game objects SDL_Rect structs
	SDL_Rect wall { 0, 0, 1024, thickness};
	SDL_Rect bottomWall { 0, 768 - thickness, 1024, thickness};
	SDL_Rect rightWall { 1024 - thickness, 0, thickness, 768};
	SDL_Rect ball {
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_Rect paddle {
		static_cast<int>(mPaddlePos.x - thickness/2),
		static_cast<int>(mPaddlePos.y - thickness/2 * 10),
		thickness,
		static_cast<int>(paddleH)
	};

	// cor de fundo
	SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 0); 

	// limpa o back buffer
	SDL_RenderClear(mRenderer);

	//cor dos objetos
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 0);

	// desenha as paredes
	SDL_RenderFillRect(mRenderer, &wall);
	SDL_RenderFillRect(mRenderer, &bottomWall);
	SDL_RenderFillRect(mRenderer, &rightWall);

	// desenha a bola
	SDL_RenderFillRect(mRenderer, &ball);

	// desenha o paddle
	SDL_RenderFillRect(mRenderer, &paddle);

	// troca para o front buffer 
	SDL_RenderPresent(mRenderer);
}


void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();

		UpdateGame();

		GenerateOutput();
	}
}