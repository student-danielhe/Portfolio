#include "Engine/Core/EventSystem.hpp"
#include <mutex>
std::mutex g_eventSystemMutex;
EventSystem* g_theEventSystem;
EventSystem::EventSystem(EventSystemConfig const& config):
m_config(config)
{

}
EventSystem::~EventSystem() {

}
void EventSystem::StartUp() {
	
}
void EventSystem::ShutDown() {

}
void EventSystem::BeginFrame() {

}
void EventSystem::EndFrame() {

}


void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr) {
    g_eventSystemMutex.lock();
	auto value = m_subscriptionListByEventName.find(eventName);
	EventSubscription newSub;
	newSub.m_function = functionPtr;
	if (value != m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		subs.push_back(newSub);
	}
	else {
		SubscriptionList newList;
		newList.push_back(newSub);
		m_subscriptionListByEventName.insert({ eventName, newList });
	}
	g_eventSystemMutex.unlock();

}
void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr) {
    g_eventSystemMutex.lock();
	auto value = m_subscriptionListByEventName.find(eventName);

	if (value != m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		for (int i = 0; i < subs.size(); i++) {
			if (subs[i].m_function == functionPtr) {
				subs.erase(subs.begin()+i);
			}
		}
	}
	g_eventSystemMutex.unlock();
}
void EventSystem::FireEvent(std::string const& eventName) {
	auto value = m_subscriptionListByEventName.find(eventName);
	EventArgs empty;
	if (value != m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		for (EventSubscription subscription : subs) {
			EventCallbackFunction functionPtr= subscription.m_function;
			functionPtr(empty);
		}
	
	}
}
void EventSystem::FireEvent(std::string const& eventName, EventArgs& args) {
	auto value = m_subscriptionListByEventName.find(eventName);

	if (value != m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		for (EventSubscription subscription : subs) {
			EventCallbackFunction functionPtr = subscription.m_function;
			functionPtr(args);
		}

	}
}

//------------------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr) {
    g_eventSystemMutex.lock();
	auto value = g_theEventSystem->m_subscriptionListByEventName.find(eventName);
	EventSubscription newSub;
	newSub.m_function = functionPtr;
	if (value != g_theEventSystem->m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		subs.push_back(newSub);
	}
	else {
		SubscriptionList newList;
		newList.push_back(newSub);
		g_theEventSystem->m_subscriptionListByEventName.insert({ eventName, newList });
	}

	g_eventSystemMutex.unlock();
}
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr) {
    g_eventSystemMutex.lock();
	auto value = g_theEventSystem->m_subscriptionListByEventName.find(eventName);

	if (value != g_theEventSystem->m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		for (int i = 0; i < subs.size(); i++) {
			if (subs[i].m_function == functionPtr) {
				subs.erase(subs.begin() + i);
			}
		}
	}
	g_eventSystemMutex.unlock();
}
void FireEvent(std::string const& eventName) {
	auto value = g_theEventSystem->m_subscriptionListByEventName.find(eventName);
	EventArgs empty;
	if (value != g_theEventSystem->m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		for (EventSubscription subscription : subs) {
			EventCallbackFunction functionPtr = subscription.m_function;
			functionPtr(empty);
		}

	}
}
void FireEvent(std::string const& eventName, EventArgs& args) {
	auto value = g_theEventSystem->m_subscriptionListByEventName.find(eventName);

	if (value != g_theEventSystem->m_subscriptionListByEventName.end()) {
		SubscriptionList subs = value->second;
		for (EventSubscription subscription : subs) {
			EventCallbackFunction functionPtr = subscription.m_function;
			functionPtr(args);
		}

	}
}

std::vector<std::string> EventSystem::GetAllSubscribedEvent() {
	std::vector<std::string> result;
	if (m_subscriptionListByEventName.size() == 0) {
		result.push_back("Error: No Command Fond");
	}
	for (auto value : m_subscriptionListByEventName) {
		result.push_back(value.first);
		
	}
	return result;
}