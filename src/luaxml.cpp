// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "luascript.h"
#include "tools.h"

using namespace Lua;

static int luaCreateXmlDocument(lua_State* L)
{
	// XMLDocument(filename)
	std::string filename = getString(L, 2);
	if (filename.empty()) {
		lua_pushnil(L);
		return 1;
	}

	auto doc = std::make_unique<pugi::xml_document>();
	if (auto result = doc->load_file(filename.c_str())) {
		pushUserdata<pugi::xml_document>(L, doc.release());
		setMetatable(L, -1, "XMLDocument");
	} else {
		printXMLError("Error - LuaScriptInterface::luaCreateXmlDocument", filename, result);
		lua_pushnil(L);
	}
	return 1;
}

static int luaDeleteXmlDocument(lua_State* L)
{
	// doc:delete() or doc:__gc()
	pugi::xml_document** document = getRawUserdata<pugi::xml_document>(L, 1);
	if (document && *document) {
		delete *document;
		*document = nullptr;
	}
	return 1;
}

static int luaXmlDocumentChild(lua_State* L)
{
	// doc:child(name)
	pugi::xml_document* document = getUserdata<pugi::xml_document>(L, 1);
	if (!document) {
		lua_pushnil(L);
		return 1;
	}

	std::string name = getString(L, 2);
	if (name.empty()) {
		lua_pushnil(L);
		return 1;
	}

	auto node = std::make_unique<pugi::xml_node>(document->child(name.c_str()));
	pushUserdata<pugi::xml_node>(L, node.release());
	setMetatable(L, -1, "XMLNode");
	return 1;
}

static int luaDeleteXmlNode(lua_State* L)
{
	// node:delete() or node:__gc()
	pugi::xml_node** node = getRawUserdata<pugi::xml_node>(L, 1);
	if (node && *node) {
		delete *node;
		*node = nullptr;
	}
	return 1;
}

static int luaXmlNodeAttribute(lua_State* L)
{
	// node:attribute(name)
	pugi::xml_node* node = getUserdata<pugi::xml_node>(L, 1);
	if (!node) {
		lua_pushnil(L);
		return 1;
	}

	std::string name = getString(L, 2);
	if (name.empty()) {
		lua_pushnil(L);
		return 1;
	}

	pugi::xml_attribute attribute = node->attribute(name.c_str());
	if (attribute) {
		pushString(L, attribute.value());
	} else {
		lua_pushnil(L);
	}
	return 1;
}

static int luaXmlNodeName(lua_State* L)
{
	// node:name()
	pugi::xml_node* node = getUserdata<pugi::xml_node>(L, 1);
	if (!node) {
		lua_pushnil(L);
		return 1;
	}

	pushString(L, node->name());
	return 1;
}

static int luaXmlNodeFirstChild(lua_State* L)
{
	// node:firstChild()
	pugi::xml_node* node = getUserdata<pugi::xml_node>(L, 1);
	if (!node) {
		lua_pushnil(L);
		return 1;
	}

	auto firstChild = node->first_child();
	if (!firstChild) {
		lua_pushnil(L);
		return 1;
	}

	auto newNode = std::make_unique<pugi::xml_node>(std::move(firstChild));
	pushUserdata<pugi::xml_node>(L, newNode.release());
	setMetatable(L, -1, "XMLNode");
	return 1;
}

static int luaXmlNodeNextSibling(lua_State* L)
{
	// node:nextSibling()
	pugi::xml_node* node = getUserdata<pugi::xml_node>(L, 1);
	if (!node) {
		lua_pushnil(L);
		return 1;
	}

	auto nextSibling = node->next_sibling();
	if (!nextSibling) {
		lua_pushnil(L);
		return 1;
	}

	auto newNode = std::make_unique<pugi::xml_node>(std::move(nextSibling));
	pushUserdata<pugi::xml_node>(L, newNode.release());
	setMetatable(L, -1, "XMLNode");
	return 1;
}

namespace LuaXml {
static void registerFunctions(LuaScriptInterface* interface)
{
	interface->registerClass("XMLDocument", "", luaCreateXmlDocument);
	interface->registerMetaMethod("XMLDocument", "__gc", luaDeleteXmlDocument);
	interface->registerMethod("XMLDocument", "delete", luaDeleteXmlDocument);

	interface->registerMethod("XMLDocument", "child", luaXmlDocumentChild);

	interface->registerClass("XMLNode", "");
	interface->registerMetaMethod("XMLNode", "__gc", luaDeleteXmlNode);
	interface->registerMethod("XMLNode", "delete", luaDeleteXmlNode);

	interface->registerMethod("XMLNode", "attribute", luaXmlNodeAttribute);
	interface->registerMethod("XMLNode", "name", luaXmlNodeName);
	interface->registerMethod("XMLNode", "firstChild", luaXmlNodeFirstChild);
	interface->registerMethod("XMLNode", "nextSibling", luaXmlNodeNextSibling);
}
} // namespace LuaXml
