// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "script.h"

#include "configmanager.h"

#include <filesystem>
#include <thread>

extern LuaEnvironment g_luaEnvironment;
extern ConfigManager g_config;

Scripts::Scripts(bool newLuaState) : scriptInterface("Scripts Interface")
{
	scriptInterface.initState(newLuaState);
}

Scripts::~Scripts() { scriptInterface.closeState(); }

bool Scripts::loadScripts(bool reload)
{
	namespace fs = std::filesystem;

	fs::path dir = "data/scripts";
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		std::cout << "[Warning - Scripts::loadScripts] Can not load scripts folder." << std::endl;
		return false;
	}

	fs::recursive_directory_iterator endit;
	std::vector<fs::path> v;
	std::string disable = ("#");
	for (fs::recursive_directory_iterator it(dir); it != endit; ++it) {
		if (fs::is_regular_file(*it) && it->path().extension() == ".lua") {
			size_t found = it->path().filename().string().find(disable);
			if (found != std::string::npos) {
				if (g_config.getBoolean(ConfigManager::SCRIPTS_CONSOLE_LOGS)) {
					std::cout << "> " << it->path().filename().string() << " [disabled]" << std::endl;
				}
				continue;
			}

			if (scriptInterface.loadFile(it->path().string()) == -1) {
				std::cout << "> " << it->path().string() << " [error]" << std::endl;
				std::cout << "^ " << scriptInterface.getLastLuaError() << std::endl;
				continue;
			}

			if (g_config.getBoolean(ConfigManager::SCRIPTS_CONSOLE_LOGS)) {
				if (!reload) {
					std::cout << "> " << it->path().string() << " [loaded]" << std::endl;
				} else {
					std::cout << "> " << it->path().string() << " [reloaded]" << std::endl;
				}
			}
		}
	}

	return true;
}

bool Scripts::loadMonsters()
{
	namespace fs = std::filesystem;

	fs::path dir = "data/monster/lua";
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		std::cout << "[Warning - Scripts::loadMonsters] Can not load monster/lua folder." << std::endl;
		return false;
	}

	fs::recursive_directory_iterator endit;
	std::vector<fs::path> v;
	std::string disable = ("#");
	for (fs::recursive_directory_iterator it(dir); it != endit; ++it) {
		if (fs::is_regular_file(*it) && it->path().extension() == ".lua") {
			size_t found = it->path().filename().string().find(disable);
			if (found != std::string::npos) {
				continue;
			}

			if (scriptInterface.loadFile(it->path().string()) == -1) {
				std::cout << "> " << it->path().string() << " [error]" << std::endl;
				std::cout << "^ " << scriptInterface.getLastLuaError() << std::endl;
				continue;
			}
		}
	}

	return true;
}

bool Scripts::loadLibs()
{
	namespace fs = std::filesystem;

	fs::path dir = "data/lib";
	if (!fs::exists(dir) || !fs::is_directory(dir)) {
		std::cout << "[Warning - Scripts::loadlibs] Can not load lib folder." << std::endl;
		return false;
	}

	fs::recursive_directory_iterator endit;
	std::vector<fs::path> v;
	std::string disable = ("#");
	for (fs::recursive_directory_iterator it(dir); it != endit; ++it) {
		if (fs::is_regular_file(*it) && it->path().extension() == ".lua") {
			size_t found = it->path().filename().string().find(disable);
			if (found != std::string::npos) {
				continue;
			}

			if (scriptInterface.loadFile(it->path().string()) == -1) {
				std::cout << "> " << it->path().string() << " [error]" << std::endl;
				std::cout << "^ " << scriptInterface.getLastLuaError() << std::endl;
				continue;
			}
		}
	}

	return true;
}

bool Scripts::loadItems(std::string folderName)
{
	std::filesystem::path dir = "data/items/" + folderName;
	std::filesystem::recursive_directory_iterator endit;

	for (std::filesystem::recursive_directory_iterator it(dir); it != endit; ++it) {
		if (scriptInterface.loadFile(it->path().string()) == -1) {
			std::cout << "> " << it->path().string() << " [error]" << std::endl;
			std::cout << "^ " << scriptInterface.getLastLuaError() << std::endl;
			continue;
		}
	}

	return true;
}

bool Scripts::loadItemsMultiThreaded(std::string folderName)
{
	std::filesystem::path dir = "data/items/" + folderName;
	std::filesystem::recursive_directory_iterator endit;
	std::vector<std::string> temp;

	for (std::filesystem::recursive_directory_iterator it(dir); it != endit; ++it) {
		temp.push_back(it->path().string());
	}

	auto cores = std::thread::hardware_concurrency();
	std::vector<std::thread> t(cores);
	size_t size = temp.size();
	uint16_t perCore = size / cores;

	while (perCore * cores < temp.size()) {
		++perCore;
	}

	for (int i = 0; i < cores; ++i) {
		t[i] = std::thread([i, perCore, cores, size, &temp] {
			int start = i == 0 ? 1 : perCore * i + 1;
			int end = i == 0 ? perCore * 1 : start + perCore - 1;
			if (i == cores-1) {
				end = size;
			}
			std::cout << "core[" << i << "] start = " << start << " end = " << end << std::endl;

			Scripts* items = new Scripts(true);

			for (int x = start; x < end; x++) {
				//std::cout << temp[x] << std::endl;
				if (items->scriptInterface.loadFile(temp[x]) == -1) {
					std::cout << "> " << temp[x] << " [error]" << std::endl;
					std::cout << "^ " << items->scriptInterface.getLastLuaError() << std::endl;
					continue;
				}
			}

			//items->~Scripts();
			delete items;
			
		});
	}

	for (auto& th : t) {
		th.join();
	}

	return true;
}
