#pragma once

#include <cstdint>

static constexpr std::size_t OrdersCacheReserverSize = 1000000U;     // Predicted amount of operations
static constexpr std::size_t UserOrdersCacheReserverSize = 1000U;    // Predicted amount of users
static constexpr std::size_t OrdersPerUserCacheReserverSize = 1000U; // Predicted amount of orders of each user
static constexpr std::size_t SecuritiesCacheReserverSize = 1000000U; // Predicted amount of orders per security
