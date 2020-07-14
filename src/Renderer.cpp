
#include <iostream>
#include <string>
#include "Renderer.h"

Renderer::Renderer(Console* console)
{
  m_console = console;
  m_defaultColour = {0x18, 0x79, 0x87};
  m_inViewColour = {0xef, 0xd8, 0xa1};
}

Renderer::~Renderer()
{
  m_console = nullptr;
}

void Renderer::drawLog(MessageLog* messageLog, int height)
{
  std::vector<Message> messages = messageLog->getMessages();

  if (messages.size() > 0){
    for(int j = 0; j < static_cast<int>(messages.size()); j++){
      Message msg = messages.at(j);
      for(int i = 0; i < static_cast<int>(msg.m_msg.length()); i++){
        m_console->render(&msg.m_msg[i], i + 1, j + height+1, msg.m_colour);
      }
    }
  }
}

void Renderer::drawMap(Camera* camera, DungeonGenerator* dungeon, std::vector<GameObject*> *actors)
{
  int x;
  int y = 0;
  int offsetI;
  //SDL_Color colour = {0xef, 0xd8, 0xa1};
  bool occupied;

  for (int i = 0; i < dungeon->Getm_width() * dungeon->Getm_height(); i++){
    x = i % dungeon->Getm_width();

    if (!(x >= camera->getX() && x < camera->getX() + camera->getWidth() && y >= camera->getY() && y < camera->getY() + camera->getHeight())){
      if (x == dungeon->Getm_width() - 1){
        y++;
      }
      continue;
    }

    occupied = false;
    if (dungeon->m_fovMap[i] == 1){

      for(int j = 0; j < static_cast<int>(actors->size()); ++j){
        if (actors->at(j)->position == nullptr){ continue; }

        if (actors->at(j)->position->x + actors->at(j)->position->y*dungeon->Getm_width() == i){
          occupied = true;
        }
      }
      if(!occupied){
        offsetI = camera->calculateOffset(x, y);
        m_console->render(&dungeon->m_level[i], offsetI % camera->getWidth(), offsetI / camera->getWidth(), m_inViewColour);
      }
    } else if (dungeon->m_fovMap[i] == 0){
      if (dungeon->m_exploredMap[i] == 1){
        for(int j = 0; j < static_cast<int>(actors->size()); ++j){
          if (actors->at(j)->position == nullptr){ continue; }

          if (actors->at(j)->position->x + actors->at(j)->position->y*dungeon->Getm_width() == i){
            occupied = true;
          }
        }
        offsetI = camera->calculateOffset(x, y);
        m_console->render(&dungeon->m_level[i], offsetI % camera->getWidth(), offsetI / camera->getWidth(), m_defaultColour);
      }
    }
    if (x == dungeon->Getm_width() - 1){
      ++y;
    }
  }
}

void Renderer::drawActors(Camera* camera, DungeonGenerator* dungeon, std::vector<GameObject*> *actors)
{
  int mapArrayIndex;
  int offsetI;
  if (!actors->empty()){
    for (int i = static_cast<int>(actors->size())-1; i >= 0 ; --i){
      if (actors->at(i)->position == nullptr){ continue; }

      mapArrayIndex = actors->at(i)->position->x + actors->at(i)->position->y*dungeon->Getm_width();
      if (dungeon->m_fovMap[mapArrayIndex] == 1){
        offsetI = camera->calculateOffset(actors->at(i)->position->x, actors->at(i)->position->y);
        m_console->render(&actors->at(i)->renderable->chr, offsetI % camera->getWidth(), offsetI / camera->getWidth(), actors->at(i)->renderable->colour);
      }
    }
  }
}

void Renderer::drawMenuOutline()
{
  SDL_Color colour = {0xef, 0xb7, 0x75};
  int xBuffer = m_console->getXBuffer();
  int yBuffer = m_console->getYBuffer();
  int height = m_console->Getm_height();
  int width = m_console->Getm_width();
  int verticalBar = 186;
  int topLeft = 201;
  int topRight = 187;
  int bottomLeft = 200;
  int bottomRight = 188;
  int horizontalBar = 205;
  int leftJoin = 185;
  int topJoin = 202;

  for (int i = 0; i < width + xBuffer; ++i){
    for (int j = 0; j < height + yBuffer; ++j){
      if (i == width && j == 0){
        m_console->render(topLeft, i, j, colour);
      } else if (i == width + xBuffer - 1 && j == 0){
        m_console->render(topRight, i, j, colour);
      } else if (i == width && j == height + yBuffer -1){
        m_console->render(topJoin, i, j, colour);
      } else if (i == width + xBuffer - 1 && j == height + yBuffer -1){
        m_console->render(bottomRight, i, j, colour);
      }else if (j == height && i == width){
        m_console->render(leftJoin, i, j, colour);
      } else if (i == width || i == width + xBuffer - 1){
        m_console->render(verticalBar, i, j, colour);
      } else if ((j == 0 || j == height + yBuffer - 1) && i > width){
        m_console->render(horizontalBar, i, j, colour);
      } else if (i == 0 && j == height){
        m_console->render(topLeft, i, j, colour);
      } else if (i == 0 && j == height + yBuffer - 1){
        m_console->render(bottomLeft, i, j, colour);
      } else if (i == 0 && j >= height){
        m_console->render(verticalBar, i, j, colour);
      } else if (j == height && i < width){
        m_console->render(horizontalBar, i, j, colour);
      } else if (j == height + yBuffer - 1 && i < width){
        m_console->render(horizontalBar, i, j, colour);
      }
    }
  }
}

void Renderer::drawUI()
{
  drawMenuOutline();
}

void Renderer::drawStartMenu(int i)
{
  int width = m_console->Getm_width() + m_console->getXBuffer();
  int height = m_console->Getm_height() + m_console->getYBuffer();
  std::string name = "ROGUELIKE";
  std::string startText = "START GAME";
  std::string exitText = "EXIT GAME";

  m_console->flush();

  for (int j = 0; j < static_cast<int>(name.length()); ++j){
    m_console->render(&name[j], width/2 + j - 5, height/2-4, m_defaultColour);
  }

  for (int j = 0; j < static_cast<int>(startText.length()); ++j){
    if (i == 0){
      m_console->render(&startText[j], width/2 + j - 5, height/2, m_inViewColour);
    } else {
      m_console->render(&startText[j], width/2 + j - 5, height/2, m_defaultColour);
    }
  }

  for (int j = 0; j < static_cast<int>(exitText.length()); ++j){
    if (i == 1){
      m_console->render(&exitText[j], width/2 + j - 5, height/2+4, m_inViewColour);
    } else {
      m_console->render(&exitText[j], width/2 + j - 5, height/2+4, m_defaultColour);
    }
  }

  m_console->update();
}

void Renderer::drawInventory(std::vector<GameObject*> *actors, int i)
{
	m_console->flush();

	std::string inventoryHeader = "Inventory";
	int width = m_console->Getm_width() / 2;
	std::string selectedItem;

	for (int k = 0; k < static_cast<int>(inventoryHeader.length()); ++k){
		m_console->render(&inventoryHeader[k], width -static_cast<int>(inventoryHeader.length()) + k, 3, m_inViewColour);
	}

	if (actors->at(0)->inventory->inventory.size() > 0){
		for (int k = 0; k < static_cast<int>(actors->at(0)->inventory->inventory.size());++k){
			GameObject* item = actors->at(0)->inventory->inventory.at(k);
			if (k == i){
				selectedItem = ">" + item->m_name + "<";
				for (int j = 0; j < static_cast<int>(selectedItem.length()); ++j){
					m_console->render(&selectedItem[j], 1 + j, 2*k + 5, m_inViewColour);
				}
			} else {
				for (int j = 0; j < static_cast<int>(item->m_name.length()); ++j){
					m_console->render(&item->m_name[j], 2 + j, 2*k + 5, m_defaultColour);
				}
			}
		}
	}

	m_console->update();
} 

void Renderer::drawCharacterScene(std::vector<GameObject*> *actors, int i)
{
	m_console->flush();

	std::string character = "Character Screen";

	for (int j = 0; j < static_cast<int>(character.length()); ++j){
		m_console->render(&character[j], j + 3, 2, m_inViewColour);
	}	

	m_console->update();
}

void Renderer::drawGameScreen(Camera* camera, DungeonGenerator* dungeon, std::vector<GameObject*> *actors, MessageLog* messageLog)
{
  m_console->flush();
  drawMap(camera, dungeon, actors);
  drawActors(camera, dungeon, actors);
  drawLog(messageLog, camera->getHeight());
  drawUI();
  m_console->update();
}
