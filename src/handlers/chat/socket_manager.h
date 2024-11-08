#pragma once
#include <userver/utest/using_namespace_userver.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/statistics_storage.hpp>
#include <userver/components/tcp_acceptor_base.hpp>
#include <userver/concurrent/queue.hpp>
#include <userver/server/handlers/server_monitor.hpp>
#include <userver/utils/daemon_run.hpp>

#include <userver/utils/statistics/metric_tag.hpp>
#include <userver/utils/statistics/metrics_storage.hpp>

#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <utility>

#include "app/application_component.h"

namespace bifrost::handlers::chat {
  struct Stats;

  class SocketManager final : public userver::components::TcpAcceptorBase {
  public:
    static constexpr std::string_view kName = "socket-manager";

    SocketManager(const components::ComponentConfig& config,
            const components::ComponentContext& context);

    void ProcessSocket(engine::io::Socket&& sock) override;

  private:
    app::Chat& chat_;
    Stats& stats_;
    // utils::parser;
  };
}