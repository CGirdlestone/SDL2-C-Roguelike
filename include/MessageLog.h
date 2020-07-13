#ifndef MESSAGELOG_H
#define MESSAGELOG_H

#include <vector>
#include <string>
#include "SDL2/SDL.h"

#include "Message.h"
#include "EventManager.h"
#include "System.h"
#include "GameObject.h"

class MessageLog : System
{
public:
    MessageLog(int x_buffer, int y_buffer, EventManager* eventManager, std::vector<GameObject*> *entities);
    virtual ~MessageLog();
    void addMessage(std::string msg, SDL_Color colour);
    void addMessage(std::string msg);
    std::vector<Message> getMessages();
    int getm_y_buffer(){return m_y_buffer;};
    void ageMessages(Uint32 ticks);
    virtual void notify(AttackEvent event);
    virtual void notify(OnHitEvent event);
    virtual void notify(OnMissEvent event);
    virtual void notify(DamageEvent event);
    virtual void notify(DeadEvent event);
    virtual void notify(OnPickUpEvent event);
		virtual void notify(DropEvent event);
protected:

private:
    std::vector<Message> m_messageQueue;
    int m_x_buffer;
    int m_y_buffer;
    EventManager* m_eventManager;
    std::vector<GameObject*> *m_entities;
};

#endif
