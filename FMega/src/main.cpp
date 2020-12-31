#include "fmega.h"
#include "core/game.h"
#include "core/global.h"
#include "utils/random.h"
#include "fmega/fmegaScene.h"

using namespace fmega;

class MyGame : public Game {
public:
	MyGame(const GameData& data) : Game(data)
	{
		InitGlobal();
		Random::Init();
		ChangeScene({ [this]() { return new FMegaScene(this); } });
	}

	void Update() override
	{
		Game::Update();
	}
};

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	DisplayData data;
	data.width = 1280;
	data.height = 720;
	data.title = "Haha";
	data.swapInterval = 0;
	data.fpsLimit = 0;
	data.fullscreen = 0;
	GameData gdata;
	gdata.displayData = data;
	Game* game = new MyGame(gdata);
	Timer t;
	float amount = 0;
	int fps = 0;

	while (!game->GetDisplay()->GetInput()->ShouldClose() && game->GetDisplay()->GetInput()->IsKeyUp(Key::ESCAPE))
	{
		game->Update();
		t.Update();
		amount += t.GetDelta();
		fps++;
		if (amount >= 1)
		{
			amount -= int(amount);
			LOG_INFO("FPS: ", fps);
			fps = 0;
		}
	}
	delete game;
}