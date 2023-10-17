#include "GameLogic.h"

#include "Logger.h"
#include "ResourceZipFile.h"
#include "TextureResource.h"

#include <filesystem>

GameLogic* g_game_logic = nullptr;

GameLogic::GameLogic()
	: current_state(starting_up)
	, resource_cache(nullptr)
{
	g_game_logic = this;
}

bool GameLogic::initialize()
{
	std::filesystem::path resource_path{ "assets.zip" };
	std::filesystem::path full_resource_path = 
		std::filesystem::canonical(resource_path);

	ResourceFile* zip_file = ALLOC ResourceZipFile(full_resource_path.string());
	resource_cache = ALLOC ResourceCache(50, zip_file);

	if (!resource_cache->init())
	{
		LOG_FATAL("Failed to initialize the resource cache. Is there enough memory?");
		return false;
	}

	resource_cache->register_loader(std::make_shared<TextureLoader>());

	window = std::unique_ptr<Window>(ALLOC Window());
	window->initialize();

	Resource default_texture("textures/default_texture.image");
	auto handle = resource_cache->get_handle(&default_texture);
	std::shared_ptr<TextureExtraData> texture_extra =
		static_pointer_cast<TextureExtraData>(handle->get_extra());
	Texture::default_texture = texture_extra->texture;

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


