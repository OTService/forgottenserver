// Copyright 2022 The Forgotten Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_OTPCH_H
#define FS_OTPCH_H

// Definitions should be global.
#include "definitions.h"

// System headers required in headers should be included here.
#include <algorithm>
#include <array>
#include <atomic>
#include <bitset>
#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/http/message.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/variant.hpp>
#include <cassert>
#include <condition_variable>
#include <cryptopp/rsa.h>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <fmt/color.h>
#include <forward_list>
#include <functional>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <mysql/mysql.h>
#include <pugixml.hpp>
#include <random>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>

#if __has_include("luajit/lua.hpp")
#include <luajit/lua.hpp>
#else
#include <lua.hpp>
#endif

#endif // FS_OTPCH_H
