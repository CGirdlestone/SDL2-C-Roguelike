#include <iostream>
#include <SDL2/SDL.h>
#include <cstdlib>
#include <time.h>
#include <string>

#include "DungeonGenerator.h"
#include "Console.h"
#include "KeyPressSurfaces.h"
#include "Components.h"
#include "GameObject.h"
#include "Message.h"
#include "MessageLog.h"
#include "Pathfind.h"
#include "Game.h"
#include "Renderer.h"
#include "EventTypes.h"
#include "Events.h"
#include "EventManager.h"
#include "MoveSystem.h"
#include "CombatSystem.h"
#include "InventorySystem.h"
#include "Scene.h"
#include "StartScene.h"
#include "GameScene.h"
#include "InventoryScene.h"
#include "CharacterScene.h"

Game::Game()
{
  //ctor
  m_console = nullptr;
  m_dungeon = nullptr;
  m_messageLog = nullptr;
  m_camera = nullptr;
  m_renderer = nullptr;
  m_eventManager = nullptr;
  m_moveSystem = nullptr;
  m_combatSystem = nullptr;
  m_inventorySystem = nullptr;
  m_startScene = nullptr;
  m_gameScene = nullptr;
	m_inventoryScene = nullptr;
	m_characterScene = nullptr;
  m_sceneManager = nullptr;

}

Game::~Game()
{
  //dtor
  delete m_console;
  m_console = nullptr;

  delete m_dungeon;
  m_dungeon = nullptr;

  delete m_messageLog;
  m_messageLog = nullptr;

  delete m_camera;
  m_camera = nullptr;

  delete m_renderer;
  m_renderer = nullptr;

  delete m_eventManager;
  m_eventManager = nullptr;

  delete m_moveSystem;
  m_moveSystem = nullptr;

  delete m_combatSystem;
  m_combatSystem = nullptr;

  delete m_inventorySystem;
  m_inventorySystem = nullptr;

  delete m_startScene;
  m_startScene = nullptr;

  delete m_gameScene;
  m_gameScene = nullptr;

	delete m_inventoryScene;
	m_inventoryScene = nullptr;

	delete m_characterScene;
	m_characterScene = nullptr;

  delete m_sceneManager;
  m_sceneManager = nullptr;

  for (int i = 0; i < static_cast<int>(m_actors.size()); i++){
    delete m_actors[i];
  }
}

bool Game::init(int mapWidth, int mapHeight, int width, int height, int tileSize, char* title)
{
  m_dungeon = new DungeonGenerator(mapWidth, mapHeight);
  m_camera = new Camera(width, height, mapWidth, mapHeight);
  m_console = new Console(width, height, title, (char*)"./resources/Cheepicus_8x8x2.png", tileSize);

  m_eventManager = new EventManager();
  m_messageLog = new MessageLog(width, 8, m_eventManager, &m_actors);
  m_combatSystem = new CombatSystem(m_eventManager, &m_actors);
  m_inventorySystem = new InventorySystem(m_eventManager, &m_actors);
  m_moveSystem = new MoveSystem(m_eventManager, &m_actors, m_dungeon);
  m_renderer = new Renderer(m_console);

  m_sceneManager = new GameStateManager(m_eventManager, &m_actors);
  m_startScene = new StartScene(m_eventManager, m_renderer);
  m_gameScene = new GameScene(m_eventManager, m_renderer, &m_actors, m_camera, m_dungeon, m_messageLog);
	m_inventoryScene = new InventoryScene(m_eventManager, m_renderer, &m_actors);
	m_characterScene = new CharacterScene(m_eventManager, m_renderer, &m_actors);
  m_sceneManager->m_startScene = m_startScene;
  m_sceneManager->m_gameScene = m_gameScene;
	m_sceneManager->m_inventoryScene = m_inventoryScene;
	m_sceneManager->m_characterScene = m_characterScene;

  m_width = width;
  m_height = height;
  m_mapWidth = mapWidth;
  m_mapHeight = mapHeight;
  m_tileSize = tileSize;

  if (m_dungeon == nullptr || m_console == nullptr){
      return false;
  } else {
      return true;
  }
}

void Game::run()
{
  SDL_Event e;

  Uint32 currentTime;
  Uint32 lastTime = 0;
  Uint32 dt = 0;


  m_dungeon->createMap(60, 6, 2, 5);
  m_dungeon->createPlayer(&m_actors);
  m_dungeon->createEntities(&m_actors);
  m_dungeon->createItems(&m_actors);
  m_dungeon->shadowCast(m_actors.at(0)->position->x, m_actors.at(0)->position->y, 10);
  m_camera->updatePosition(m_actors.at(0)->position->x, m_actors.at(0)->position->y);

  m_eventManager->pushEvent(PushScene(STARTMENU));

  while(m_sceneManager->playing){
    currentTime = SDL_GetTicks();

    m_sceneManager->render();
    m_sceneManager->processInput(&e);

    dt = (currentTime - lastTime);
    lastTime = currentTime;

    m_sceneManager->update(dt);
    m_sceneManager->onTick();
  }

  m_console->closeSDL();
}
