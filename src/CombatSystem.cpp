#include <iostream>
#include <cstdlib>
#include <map>

#include "CombatSystem.h"
#include "EventManager.h"
#include "Events.h"
#include "EventTypes.h"
#include "DamageTypes.h"
#include "Slots.h"

CombatSystem::CombatSystem(EventManager* eventManager, std::map<int, GameObject*> *entities):
m_eventManager(eventManager), m_entities(entities)
{
  m_eventManager->registerSystem(ATTACK, this);
  m_eventManager->registerSystem(ONHIT, this);
  m_eventManager->registerSystem(DAMAGE, this);
  m_eventManager->registerSystem(DEAD, this);
}

CombatSystem::~CombatSystem()
{
  m_eventManager = nullptr;
  m_entities = nullptr;
}

void CombatSystem::doAttack(AttackEvent event)
{
  int roll = std::rand()%20 + 1;

	

  if (roll >= 10 + m_entities->at(event.m_defender_uid)->fighter->defence){
    OnHitEvent onHit = OnHitEvent(event.m_attacker_uid, event.m_defender_uid);
    m_eventManager->pushEvent(onHit);
  } else {
    OnMissEvent onMiss = OnMissEvent(event.m_attacker_uid, event.m_defender_uid);
    m_eventManager->pushEvent(onMiss);
  }
}

void CombatSystem::calculateDamage(OnHitEvent event)
{
  int dmg;

  dmg = std::rand()%6 + 1 + m_entities->at(event.m_attacker_uid)->fighter->power;

	DamageTypes type = getDamageType(m_entities->at(event.m_attacker_uid));
	if (isResistantToDamageType(m_entities->at(event.m_defender_uid), type)){
		dmg = dmg / 2;
	}
	if (isWeakToDamageType(m_entities->at(event.m_defender_uid), type)){
		dmg = dmg * 2;
	}

  DamageEvent damageEvent = DamageEvent(event.m_defender_uid, dmg);
  m_eventManager->pushEvent(damageEvent);

}

void CombatSystem::calculateDamage(OnCriticalHitEvent event)
{
	int dmg;

	dmg = std::rand()%6 + 1 + m_entities->at(event.m_attacker_uid)->fighter->power;

	DamageEvent damageEvent = DamageEvent(event.m_defender_uid, dmg*2);
	m_eventManager->pushEvent(damageEvent);
}

void CombatSystem::applyDamage(DamageEvent event)
{
  if (m_entities->at(event.m_uid)->fighter != nullptr){
    m_entities->at(event.m_uid)->fighter->health -= event.m_damage;

    if (m_entities->at(event.m_uid)->fighter->health <= 0){
			m_entities->at(event.m_uid)->fighter->isAlive = false;
      DeadEvent deadEvent = DeadEvent(event.m_uid);
      m_eventManager->pushEvent(deadEvent);
    } else if (m_entities->at(event.m_uid)->fighter->health > m_entities->at(event.m_uid)->fighter->maxHealth){
			m_entities->at(event.m_uid)->fighter->health = m_entities->at(event.m_uid)->fighter->maxHealth;
		}
  }
}

DamageTypes CombatSystem::getDamageType(GameObject* attacker)
{
	if (attacker->body != nullptr){
		if (attacker->body->slots.at(RIGHTHAND) != nullptr){
			return attacker->body->slots.at(RIGHTHAND)->weapon->damageType;
		}
	}
	return NODAMAGETYPE;
}

bool CombatSystem::isResistantToDamageType(GameObject* defender, DamageTypes type)
{
	if (type == NODAMAGETYPE) { return false; }

	if (defender->armour != nullptr){
		if (defender->armour->resistance == type){
			return true;
		} else {
			return false;
		}
	} else {
		if (defender->body->slots.at(BODY)->armour->resistance == type){
			return true;
		} else {
			return false;
		}
	}	
}

bool CombatSystem::isWeakToDamageType(GameObject* defender, DamageTypes type)
{
	if (type == NODAMAGETYPE) { return false; }

	if (defender->armour != nullptr){
		if (defender->armour->weakness == type){
			return true;
		} else {
			return false;
		}
	} else {
		if (defender->body->slots.at(BODY)->armour->weakness == type){
			return true;
		} else {
			return false;
		}
	}	
}

int CombatSystem::getAttackerHitModifiers(GameObject* attacker)
{
	int attackerHitBonus = 0;

	return attackerHitBonus;
}


int CombatSystem::getDefenderHitModifiers(GameObject* defender)
{
	int defenderBonus = 0;

	return defenderBonus;
}

int CombatSystem::getAttackerDamageModifiers(GameObject* attacker)
{
	int attackerDamageBonus = 0;

	return attackerDamageBonus;
}

int CombatSystem::getDefenderDamageModifiers(GameObject* defender)
{
	int defenderBonus = 0;

	return defenderBonus;
}

void CombatSystem::onDead(DeadEvent event)
{
  SDL_Color c = {0x55, 0x0f, 0x0a};
  m_entities->at(event.m_uid)->renderable->colour = c;
  m_entities->at(event.m_uid)->renderable->chr = '%';
}

// notify overrides below

void CombatSystem::notify(AttackEvent event)
{
  doAttack(event);
}

void CombatSystem::notify(OnHitEvent event)
{
  calculateDamage(event);
}

void CombatSystem::notify(OnCriticalHitEvent event)
{
  calculateDamage(event);
}

void CombatSystem::notify(DamageEvent event)
{
  applyDamage(event);
}

void CombatSystem::notify(DeadEvent event)
{
  onDead(event);
}
