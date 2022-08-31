// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAENUMS_H
#define FS_LUAENUMS_H

#include "luascript.h"

class LuaEnums
{
public:
	static void registerEnums(LuaScriptInterface* interface);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAENUMS_H
