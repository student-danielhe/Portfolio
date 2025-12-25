#pragma once
#include <vector>
#include <string>
#include "Engine/Core/NamedStrings.hpp"

typedef NamedStrings EventArgs;
typedef bool (*EventCallbackFunction)(EventArgs& args);
struct EventSystemConfig {

};
struct EventSubscription {
	EventCallbackFunction m_function;
};
//---------------------------------------
typedef std::vector<EventSubscription> SubscriptionList;

//----------------------------------------------------
class EventSystem
{
public: 
	EventSystem(EventSystemConfig const& config);
	~EventSystem();
	void StartUp();
	void ShutDown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	void FireEvent(std::string const& eventName);
	void FireEvent(std::string const& eventName, EventArgs& args);

	std::vector<std::string> GetAllSubscribedEvent();

	std::map<std::string, SubscriptionList> m_subscriptionListByEventName;
protected:
	EventSystemConfig m_config;
	

	//--------------------------------------------------------------------


};

void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void FireEvent(std::string const& eventName);
void FireEvent(std::string const& eventName, EventArgs& args);
extern EventSystem* g_theEventSystem;

