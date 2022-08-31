// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_LUAPLAYER_H
#define FS_LUAPLAYER_H

#include "otpch.h"

#include "luascript.h"

class LuaPlayer
{
public:
	static void registerFunctions(LuaScriptInterface* interface);

private:
	friend class LuaScriptInterface;
};

#endif // FS_LUAPLAYER_H
