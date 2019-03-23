#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <sstream>
#include <deque>
#include <algorithm>
