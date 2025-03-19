#include "socket_metrics.hpp"

namespace chat::utils {
void DumpMetric(userver::utils::statistics::Writer& writer, const Stats& stats) {
    writer["sockets"]["opened"] = stats.opened_sockets;
    writer["sockets"]["closed"] = stats.closed_sockets;
    writer["bytes"]["read"] = stats.bytes_read;
}

void ResetMetric(Stats& stats) {
    stats.opened_sockets = 0;
    stats.closed_sockets = 0;
    stats.bytes_read = 0;
}
}  // namespace chat::utils