// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUACREATURE_H
#define FS_LUACREATURE_H

#include "luascript.h"

class LuaCreature
{
public:
	static void registerFunctions(LuaScriptInterface* interface);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUACREATURE_H
