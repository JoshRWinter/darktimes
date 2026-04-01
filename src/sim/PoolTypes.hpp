#pragma once

#include <win/Pool.hpp>

template<typename T> using Pool = win::Pool<T, 50, false>;
