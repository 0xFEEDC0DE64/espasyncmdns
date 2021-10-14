# espasyncmdns
ESP32 async mdns search

## Example usage
```C++
#include <chrono>
#include <esp_log.h>
#include <arrayview.h>
#include <espwifiutils.h>

#include <asyncmdnssearch.h>

using namespace std::chrono_literals;

namespace {
constexpr const char * const TAG = "MYAPP";
} // namespace

AsyncMdnsSearch mdnsSearch;

void startSearch()
{
    ESP_LOGI(TAG, "starting search...");

    if (mdnsSearch.searchStarted())
    {
        ESP_LOGE(TAG, "search already started?!");
        return;
    }

    if (const auto result = mdnsSearch.startSearch(nullptr, "_http", "_tcp", MDNS_TYPE_PTR, 5s, 20); !result)
    {
        ESP_LOGE(TAG, "could not start search: %.*s", result.error().size(), result.error().data());
        return;
    }

    ESP_LOGI(TAG, "MDNS search started!");
}

void checkResults()
{
    if (!mdnsSearch.searchStarted())
        return;

    if (const auto results = mdnsSearch.getResults())
    {
        ESP_LOGI(TAG, "got results! %zd", results->size());

        for (const mdns_result_t &result : *results)
        {
            ESP_LOGI(TAG, "result: "
                          "tcpip_if=%i "
                          "ip_protocol=%i "
                          "instance_name=%s "
                          "hostname=%s "
                          "port=%hhu ",
                     result.tcpip_if,
                     result.ip_protocol,
                     result.instance_name ? result.instance_name : "<null>",
                     result.hostname ? result.hostname : "<null>",
                     result.port);

            if (result.txt_count && result.txt)
                for (const mdns_txt_item_t &txtRecord : cpputils::ArrayView<mdns_txt_item_t>{result.txt, result.txt_count})
                    ESP_LOGI(TAG, "  txt %s %s",
                             txtRecord.key ? txtRecord.key : "<null>",
                             txtRecord.value ? txtRecord.value : "<null>"
                    );

            for (mdns_ip_addr_t *addr = result.addr; addr; addr = addr->next)
                ESP_LOGI(TAG, "  addr %s", wifi_stack::toString(addr->addr).c_str());
        }
    }
}
```
