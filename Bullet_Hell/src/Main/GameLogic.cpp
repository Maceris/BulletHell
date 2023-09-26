#include "GameLogic.h"

GameLogic* g_game_logic = nullptr;

GameLogic::GameLogic()
	: current_state(starting_up)
	, resource_cache(nullptr)
{
	g_game_logic = this;
}

bool GameLogic::initialize()
{
	ResourceFile* zip_file = ALLOC ResourceZipFile("assets.zip");
	resource_cache = ALLOC ResourceCache(50, zip_file);

	if (!resource_cache->init())
	{
		//TODO(ches) log this error
		return false;
	}

	window = std::unique_ptr<Window>(ALLOC Window());
	window->initialize();

	return true;
}

void GameLogic::run_game()
{

	current_state = running;

	while (current_state == running)
	{
		window->render();
	}
	
}

void GameLogic::request_close()
{
	if (current_state == running || current_state == starting_up)
	{
		current_state = quit_requested;
	}
}

void GameLogic::on_close()
{
	current_state = quitting;

	window->terminate();
}


