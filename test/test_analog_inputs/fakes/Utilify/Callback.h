#pragma once

#ifdef ARDUINO_AVR_UNO
using Callback = void (*)();
template <typename T>
using CallbackWithParam = void (*)(const T&);
#else
#include <functional>
using Callback = std::function<void()>;
template <typename T>
using CallbackWithParam = std::function<void(const T&)>;
#endif
