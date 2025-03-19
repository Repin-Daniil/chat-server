#pragma once

#include <userver/server/handlers/server_monitor.hpp>
#include <userver/utils/statistics/metrics_storage.hpp>

namespace bifrost::utils {
struct Stats {
    std::atomic<std::uint64_t> opened_sockets{0};
    std::atomic<std::uint64_t> closed_sockets{0};
    std::atomic<std::uint64_t> bytes_read{0};
};

void DumpMetric(userver::utils::statistics::Writer& writer, const Stats& stats);
void ResetMetric(Stats& stats);

}  // namespace bifrost::utils