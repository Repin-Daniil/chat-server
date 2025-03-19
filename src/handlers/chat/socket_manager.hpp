#pragma once

#include <userver/components/tcp_acceptor_base.hpp>
#include <userver/concurrent/queue.hpp>

#include <userver/utils/daemon_run.hpp>

#include "utils/metrics/socket_metrics.hpp"

#include "app/application_component.hpp"

namespace chat::handlers::chat {
struct Stats;

class SocketManager final : public userver::components::TcpAcceptorBase {
public:
    static constexpr std::string_view kName = "socket-manager";

    SocketManager(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    void ProcessSocket(userver::engine::io::Socket&& sock) override;

private:
    app::Chat& chat_;
    utils::Stats& stats_;
};
}  // namespace chat::handlers::chat