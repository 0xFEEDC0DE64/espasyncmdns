#pragma once

// system includes
#include <string>
#include <chrono>
#include <optional>
#include <expected>

// esp-idf includes
#include <mdns.h>

// local includes
#include "asyncmdnsresults.h"

class AsyncMdnsSearch
{
public:
    AsyncMdnsSearch() = default;
    AsyncMdnsSearch(const AsyncMdnsSearch &) = delete;
    AsyncMdnsSearch(AsyncMdnsSearch &&other);
    ~AsyncMdnsSearch();

    AsyncMdnsSearch &operator=(const AsyncMdnsSearch &) = delete;
    AsyncMdnsSearch &operator=(AsyncMdnsSearch &&other);

    bool searchStarted() const { return m_mdnsScan; }

    std::expected<void, std::string> startSearch(const char *name, const char *service, const char *proto, uint16_t type, std::chrono::milliseconds timeout, size_t max_results);
    std::optional<AsyncMdnsResults> getResults();
    void deleteSearch();

private:
    mdns_search_once_t *m_mdnsScan{};
};
