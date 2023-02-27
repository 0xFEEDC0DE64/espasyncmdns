#include "asyncmdnssearch.h"

// esp-idf includes
#include <esp_log.h>

namespace {
constexpr const char * const TAG = "ASYNCMDNS";
} // namespace

AsyncMdnsSearch::AsyncMdnsSearch(AsyncMdnsSearch &&other) :
    m_mdnsScan{std::move(other.m_mdnsScan)}
{
    other.m_mdnsScan = {};
}

AsyncMdnsSearch::~AsyncMdnsSearch()
{
    if (searchStarted())
        deleteSearch();
}

AsyncMdnsSearch &AsyncMdnsSearch::operator=(AsyncMdnsSearch &&other)
{
    m_mdnsScan = std::move(other.m_mdnsScan);
    other.m_mdnsScan = {};
    return *this;
}

std::expected<void, std::string> AsyncMdnsSearch::startSearch(const char *name, const char *service, const char *proto, uint16_t type, std::chrono::milliseconds timeout, size_t max_results)
{
    ESP_LOGD(TAG, "starting search...");

    if (searchStarted())
        return std::unexpected("last scan not finished yet");

    if (!max_results)
        return std::unexpected("max_results should be greater than 0");

    m_mdnsScan = mdns_query_async_new(name, service, proto, type, timeout.count(), max_results, nullptr);
    if (!searchStarted())
        return std::unexpected("mdns_query_async_new() returned invalid");

    return {};
}

std::optional<AsyncMdnsResults> AsyncMdnsSearch::getResults()
{
    if (!searchStarted())
    {
        ESP_LOGW(TAG, "called without a running search");
        return std::nullopt;
    }

    mdns_result_t *results{};
    uint8_t num_results{};
    if (!mdns_query_async_get_results(m_mdnsScan, 0, &results, &num_results))
        return std::nullopt;

    deleteSearch();

    return AsyncMdnsResults{results, num_results};
}

void AsyncMdnsSearch::deleteSearch()
{
    if (!m_mdnsScan)
        ESP_LOGW(TAG, "no search started!");

    mdns_query_async_delete(m_mdnsScan);
    m_mdnsScan = nullptr;
}
