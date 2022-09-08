// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "globalevent.h"

#include "configmanager.h"
#include "pugicast.h"
#include "scheduler.h"
#include "tools.h"

extern ConfigManager g_config;

GlobalEvents::GlobalEvents() : scriptInterface("GlobalEvent Interface") { scriptInterface.initState(); }

GlobalEvents::~GlobalEvents() { clear(false); }

void GlobalEvents::clearMap(GlobalEventMap& map, bool fromLua)
{
	for (auto it = map.begin(); it != map.end();) {
		if (fromLua == it->second.fromLua) {
			it = map.erase(it);
		} else {
			++it;
		}
	}
}

void GlobalEvents::clear(bool fromLua)
{
	g_scheduler.stopEvent(thinkEventId);
	thinkEventId = 0;
	g_scheduler.stopEvent(timerEventId);
	timerEventId = 0;

	clearMap(serverMap, fromLua);

	reInitState(fromLua);
}

Event_ptr GlobalEvents::getEvent(const std::string& nodeName)
{
	if (!caseInsensitiveEqual(nodeName, "globalevent")) {
		return nullptr;
	}
	return Event_ptr(new GlobalEvent(&scriptInterface));
}

bool GlobalEvents::registerEvent(Event_ptr event, const pugi::xml_node&)
{
	GlobalEvent_ptr globalEvent{static_cast<GlobalEvent*>(event.release())}; // event is guaranteed to be a GlobalEvent
	auto result = serverMap.emplace(globalEvent->getName(), std::move(*globalEvent));
	if (result.second) {
		if (globalEvent->getEventType() == GLOBALEVENT_TIMER) {
			if (timerEventId == 0) {
				timerEventId = g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, [this]() { timer(); }));
			}
		} else if (globalEvent->getEventType() == GLOBALEVENT_NONE) {
			if (thinkEventId == 0) {
				thinkEventId = g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, [this]() { think(); }));
			}
		}
		return true;
	}

	std::cout << "[Warning - GlobalEvents::configureEvent] Duplicate registered globalevent with name: "
	          << globalEvent->getName() << std::endl;
	return false;
}

bool GlobalEvents::registerLuaEvent(GlobalEvent_shared_ptr globalEvent)
{
	auto result = serverMap.emplace(globalEvent->getName(), std::move(*globalEvent));
	if (result.second) {
		if (globalEvent->getEventType() == GLOBALEVENT_TIMER) {
			if (timerEventId == 0) {
				timerEventId = g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, [this]() { timer(); }));
			}
		} else if (globalEvent->getEventType() == GLOBALEVENT_NONE) {
			if (thinkEventId == 0) {
				thinkEventId = g_scheduler.addEvent(createSchedulerTask(SCHEDULER_MINTICKS, [this]() { think(); }));
			}
		}
		return true;
	}

	std::cout << "[Warning - GlobalEvents::configureEvent] Duplicate registered globalevent with name: "
	          << globalEvent->getName() << std::endl;
	return false;
}

GlobalEvent_shared_ptr GlobalEvents::getGlobalEvent(const std::string& name)
{
	auto it = serverMap.find(name);
	if (it != serverMap.end()) {
		return GlobalEvent_shared_ptr(&it->second);
	}
	return nullptr;
}

void GlobalEvents::startup() const { execute(GLOBALEVENT_STARTUP); }

void GlobalEvents::timer()
{
	time_t now = time(nullptr);

	int64_t nextScheduledTime = std::numeric_limits<int64_t>::max();

	auto it = serverMap.begin();
	while (it != serverMap.end() && it->second.getEventType() == GLOBALEVENT_TIMER) {
		GlobalEvent& globalEvent = it->second;

		int64_t nextExecutionTime = globalEvent.getNextExecution() - now;
		if (nextExecutionTime > 0) {
			if (nextExecutionTime < nextScheduledTime) {
				nextScheduledTime = nextExecutionTime;
			}

			++it;
			continue;
		}

		if (!globalEvent.executeEvent()) {
			it = serverMap.erase(it);
			continue;
		}

		nextExecutionTime = 86400;
		if (nextExecutionTime < nextScheduledTime) {
			nextScheduledTime = nextExecutionTime;
		}

		globalEvent.setNextExecution(globalEvent.getNextExecution() + nextExecutionTime);

		++it;
	}

	if (nextScheduledTime != std::numeric_limits<int64_t>::max()) {
		timerEventId = g_scheduler.addEvent(
		    createSchedulerTask(std::max<int64_t>(1000, nextScheduledTime * 1000), [this]() { timer(); }));
	}
}

void GlobalEvents::think()
{
	int64_t now = OTSYS_TIME();

	int64_t nextScheduledTime = std::numeric_limits<int64_t>::max();
	for (auto& it : serverMap) {
		if (it.second.getEventType() == GLOBALEVENT_NONE) {
			GlobalEvent& globalEvent = it.second;

			int64_t nextExecutionTime = globalEvent.getNextExecution() - now;
			if (nextExecutionTime > 0) {
				if (nextExecutionTime < nextScheduledTime) {
					nextScheduledTime = nextExecutionTime;
				}
				continue;
			}

			if (!globalEvent.executeEvent()) {
				std::cout << "[Error - GlobalEvents::think] Failed to execute event: " << globalEvent.getName()
				          << std::endl;
			}

			nextExecutionTime = globalEvent.getInterval();
			if (nextExecutionTime < nextScheduledTime) {
				nextScheduledTime = nextExecutionTime;
			}

			globalEvent.setNextExecution(globalEvent.getNextExecution() + nextExecutionTime);
		}
	}

	if (nextScheduledTime != std::numeric_limits<int64_t>::max()) {
		thinkEventId = g_scheduler.addEvent(createSchedulerTask(nextScheduledTime, [this]() { think(); }));
	}
}

void GlobalEvents::execute(GlobalEvent_t type) const
{
	for (const auto& it : serverMap) {
		const GlobalEvent& globalEvent = it.second;
		if (globalEvent.getEventType() == type) {
			globalEvent.executeEvent();
		}
	}
}

GlobalEventMap GlobalEvents::getEventMap(GlobalEvent_t type)
{
	GlobalEventMap tmp;
	for (const auto& it : serverMap) {
		const GlobalEvent& globalEvent = it.second;
		if (globalEvent.getEventType() == type) {
			tmp.emplace(globalEvent.getName(), globalEvent);
		}
	}
	return tmp;
}

GlobalEvent::GlobalEvent(LuaScriptInterface* interface) : Event(interface) {}

bool GlobalEvent::configureEvent(const pugi::xml_node& node)
{
	pugi::xml_attribute nameAttribute = node.attribute("name");
	if (!nameAttribute) {
		std::cout << "[Error - GlobalEvent::configureEvent] Missing name for a globalevent" << std::endl;
		return false;
	}

	name = nameAttribute.as_string();
	eventType = GLOBALEVENT_NONE;

	pugi::xml_attribute attr;
	if ((attr = node.attribute("time"))) {
		std::vector<int32_t> params = vectorAtoi(explodeString(attr.as_string(), ":"));

		int32_t hour = params.front();
		if (hour < 0 || hour > 23) {
			std::cout << "[Error - GlobalEvent::configureEvent] Invalid hour \"" << attr.as_string()
			          << "\" for globalevent with name: " << name << std::endl;
			return false;
		}

		interval |= hour << 16;

		int32_t min = 0;
		int32_t sec = 0;
		if (params.size() > 1) {
			min = params[1];
			if (min < 0 || min > 59) {
				std::cout << "[Error - GlobalEvent::configureEvent] Invalid minute \"" << attr.as_string()
				          << "\" for globalevent with name: " << name << std::endl;
				return false;
			}

			if (params.size() > 2) {
				sec = params[2];
				if (sec < 0 || sec > 59) {
					std::cout << "[Error - GlobalEvent::configureEvent] Invalid second \"" << attr.as_string()
					          << "\" for globalevent with name: " << name << std::endl;
					return false;
				}
			}
		}

		time_t current_time = time(nullptr);
		tm* timeinfo = localtime(&current_time);
		timeinfo->tm_hour = hour;
		timeinfo->tm_min = min;
		timeinfo->tm_sec = sec;

		time_t difference = static_cast<time_t>(difftime(mktime(timeinfo), current_time));
		if (difference < 0) {
			difference += 86400;
		}

		nextExecution = current_time + difference;
		eventType = GLOBALEVENT_TIMER;
	} else if ((attr = node.attribute("type"))) {
		const char* value = attr.value();
		if (caseInsensitiveEqual(value, "startup")) {
			eventType = GLOBALEVENT_STARTUP;
		} else if (caseInsensitiveEqual(value, "shutdown")) {
			eventType = GLOBALEVENT_SHUTDOWN;
		} else if (caseInsensitiveEqual(value, "record")) {
			eventType = GLOBALEVENT_RECORD;
		} else {
			std::cout << "[Error - GlobalEvent::configureEvent] No valid type \"" << attr.as_string()
			          << "\" for globalevent with name " << name << std::endl;
			return false;
		}
	} else if ((attr = node.attribute("interval"))) {
		interval = std::max<int32_t>(SCHEDULER_MINTICKS, pugi::cast<int32_t>(attr.value()));
		nextExecution = OTSYS_TIME() + interval;
	} else {
		std::cout << "[Error - GlobalEvent::configureEvent] No interval for globalevent with name " << name
		          << std::endl;
		return false;
	}
	return true;
}

std::string GlobalEvent::getScriptEventName() const
{
	switch (eventType) {
		case GLOBALEVENT_STARTUP:
			return "onStartup";
		case GLOBALEVENT_SHUTDOWN:
			return "onShutdown";
		case GLOBALEVENT_RECORD:
			return "onRecord";
		case GLOBALEVENT_TIMER:
			return "onTime";
		default:
			return "onThink";
	}
}

bool GlobalEvent::executeRecord(uint32_t current, uint32_t old)
{
	// onRecord(current, old)
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - GlobalEvent::executeRecord] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);

	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	lua_pushnumber(L, current);
	lua_pushnumber(L, old);
	return scriptInterface->callFunction(2);
}

bool GlobalEvent::executeEvent() const
{
	if (!scriptInterface->reserveScriptEnv()) {
		std::cout << "[Error - GlobalEvent::executeEvent] Call stack overflow" << std::endl;
		return false;
	}

	ScriptEnvironment* env = scriptInterface->getScriptEnv();
	env->setScriptId(scriptId, scriptInterface);
	lua_State* L = scriptInterface->getLuaState();
	scriptInterface->pushFunction(scriptId);

	int32_t params = 0;
	if (eventType == GLOBALEVENT_NONE || eventType == GLOBALEVENT_TIMER) {
		lua_pushnumber(L, interval);
		params = 1;
	}

	return scriptInterface->callFunction(params);
}
