

#include <map>
#include <iostream>
#include <cstdlib>
#include <string>
#include "InventorySystem.h"
#include "EventManager.h"
#include "Events.h"
#include "EventTypes.h"
#include "Components.h"
#include "UseableFunctionEnum.h"
#include "DungeonGenerator.h"

InventorySystem::InventorySystem(EventManager* eventManager, std::map<int, GameObject*> *entities, DungeonGenerator *dungeon):
m_eventManager(eventManager), m_entities(entities), m_dungeon(dungeon)
{
  m_eventManager->registerSystem(TAKE, this);
	m_eventManager->registerSystem(DROP, this);
	m_eventManager->registerSystem(EQUIP, this);
	m_eventManager->registerSystem(UNEQUIP, this);
	m_eventManager->registerSystem(USEITEM, this);
}

InventorySystem::~InventorySystem()
{
  m_eventManager = nullptr;
  m_entities = nullptr;
}

void InventorySystem::pickUpItem(TakeEvent event)
{
	std::map<int, GameObject*>::iterator it;
  for (it = m_entities->begin(); it !=m_entities->end(); ++it){
    if(event.m_uid == it->second->m_uid){ continue; }
    if(it->second->position == nullptr){ continue; }
		if(it->second->item == nullptr){ continue; }

    if (it->second->position->x == event.m_x && it->second->position->y == event.m_y){
			if (m_entities->at(event.m_uid)->inventory->inventory.size() < 10){
      	m_entities->at(event.m_uid)->inventory->inventory.push_back(it->second);

				int i = it->second->position->x + it->second->position->y * m_dungeon->Getm_width();
				m_dungeon->removeObjectFromTile(it->second, i);

      	delete it->second->position;
      	it->second->position = nullptr;
      	m_eventManager->pushEvent(OnPickUpEvent(event.m_uid, it->second->m_name));
      	break;
			} else {
				std::string msg = "You don't have the capacity to pick that up!"; 
				m_eventManager->pushEvent(MessageEvent(msg));
			}
    }
  }
}

void InventorySystem::dropItem(DropEvent event)
{
	int x = m_entities->at(event.m_actor_uid)->position->x;
	int y = m_entities->at(event.m_actor_uid)->position->y;

	for (int i = 0; i < static_cast<int>(m_entities->at(event.m_actor_uid)->inventory->inventory.size()); ++i){
		if (m_entities->at(event.m_actor_uid)->inventory->inventory.at(i)->m_uid == event.m_item_uid){
			m_entities->at(event.m_actor_uid)->inventory->inventory.erase(m_entities->at(event.m_actor_uid)->inventory->inventory.begin() + i);
			m_entities->at(event.m_item_uid)->position = new Position(x, y);

			int i = x + y * m_dungeon->Getm_width();
			m_dungeon->moveObjectToTile(m_entities->at(event.m_item_uid), i);
		}
	}
}

void InventorySystem::equipItem(EquipEvent event)
{
	if (m_entities->at(event.m_item_uid)->wearable != nullptr){
		if (m_entities->at(event.m_actor_uid)->body->slots.at(m_entities->at(event.m_item_uid)->wearable->slot) != nullptr){
			m_entities->at(event.m_actor_uid)->inventory->inventory.push_back(m_entities->at(event.m_actor_uid)->body->slots.at(m_entities->at(event.m_item_uid)->wearable->slot));
		}
	
		m_entities->at(event.m_actor_uid)->body->slots[m_entities->at(event.m_item_uid)->wearable->slot] = m_entities->at(event.m_item_uid);

		for (int i = 0; i < static_cast<int>(m_entities->at(event.m_actor_uid)->inventory->inventory.size()); ++i){
			if (m_entities->at(event.m_actor_uid)->inventory->inventory.at(i)->m_uid == event.m_item_uid){
				m_entities->at(event.m_actor_uid)->inventory->inventory.erase(m_entities->at(event.m_actor_uid)->inventory->inventory.begin() + i);
				break;
			}
		}
	}
}

void InventorySystem::unequipItem(UnequipEvent event)
{
	if (m_entities->at(event.m_actor_uid)->inventory->inventory.size() < 10){
		GameObject* item = m_entities->at(event.m_actor_uid)->body->slots[static_cast<EquipSlots>(event.m_slotNum)];

		m_entities->at(event.m_actor_uid)->body->slots[static_cast<EquipSlots>(event.m_slotNum)] = nullptr;

		m_entities->at(event.m_actor_uid)->inventory->inventory.push_back(item);
	else{
		std::string msg = "You don't have the capacity to pick that up!"; 
		m_eventManager->pushEvent(MessageEvent(msg));
	}
}

void InventorySystem::decreaseUses(GameObject* item, int user_uid, int item_uid)
{
	--item->useable->numUses;
	
	if (item->useable->numUses == 0){
		for (int i = 0; i < static_cast<int>(m_entities->at(user_uid)->inventory->inventory.size()); ++i){
			if (m_entities->at(user_uid)->inventory->inventory.at(i)->m_uid == item_uid){
				m_entities->at(user_uid)->inventory->inventory.erase(m_entities->at(user_uid)->inventory->inventory.begin() + i);
			}
		}
	} else if (item->useable->numUses < 0){
		item->useable->numUses = -1;
	}
}

void InventorySystem::useItem(UseItemEvent event)
{
	GameObject* item = m_entities->at(event.m_item_uid);
	
	if (item->useable != nullptr){
		if(item->useable->funcToDo == HEALING){
			if (m_entities->at(event.m_user_uid)->fighter->health < m_entities->at(event.m_user_uid)->fighter->maxHealth){
				m_eventManager->pushEvent(DamageEvent(event.m_user_uid, -1 * (std::rand()%item->healing->roll + 1)));
				m_eventManager->pushEvent(PopScene(1));
				decreaseUses(item, event.m_user_uid, event.m_item_uid);
				m_eventManager->pushEvent(PlayerTurnOverEvent());
			} 
		} else if (item->useable->funcToDo == DIRECTDAMAGE || item->useable->funcToDo == AOE || item->useable->funcToDo == STATUS){
			if (event.m_target_uid == -1){
				m_eventManager->pushEvent(PopScene(1));
				m_eventManager->pushEvent(PassUserInfoEvent(event.m_user_uid, event.m_item_uid));
				m_eventManager->pushEvent(PushScene(TARGETING));
				m_eventManager->pushEvent(MessageEvent("Select a target..."));
			} else {
				// this branch indicates that a target has been selected and executes the relevant function.
				if (item->useable->funcToDo == DIRECTDAMAGE){
					m_eventManager->pushEvent(DamageEvent(event.m_target_uid, m_entities->at(event.m_item_uid)->damage->roll));
				} else if (item->useable->funcToDo == AOE){
					
				} else if (item->useable->funcToDo == STATUS){

				}
				decreaseUses(item, event.m_user_uid, event.m_item_uid);
				m_eventManager->pushEvent(PlayerTurnOverEvent());
			}
		}
	}
}

void InventorySystem::notify(TakeEvent event)
{
  pickUpItem(event);
}

void InventorySystem::notify(DropEvent event)
{
	dropItem(event);
}

void InventorySystem::notify(EquipEvent event)
{
	equipItem(event);
}

void InventorySystem::notify(UnequipEvent event)
{
	unequipItem(event);
}

void InventorySystem::notify(UseItemEvent event)
{
	useItem(event);
}

