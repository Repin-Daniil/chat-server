#include <handlers/auth/auth_registration_handler.h>

#include <userver/utest/using_namespace_userver.hpp>

#include <userver/clients/dns/component.hpp>
#include <userver/components/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/components/statistics_storage.hpp>
#include <userver/components/tcp_acceptor_base.hpp>
#include <userver/concurrent/queue.hpp>
#include <userver/server/handlers/server_monitor.hpp>
#include <userver/utils/daemon_run.hpp>
#include "userver/components/component_list.hpp"

#include <userver/utils/statistics/metric_tag.hpp>
#include <userver/utils/statistics/metrics_storage.hpp>


#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include "handlers/chat/socket_manager.h"
#include "app/registry/registry_component.h"
#include "app/application_component.h"
#include "app/auth/auth_component.h"
#include <userver/server/handlers/ping.hpp>

int main(int argc, const char* const argv[]) {
    const auto component_list = components::MinimalServerComponentList()
        .Append<userver::server::handlers::Ping>()
        .Append<server::handlers::ServerMonitor>()
        .Append<bifrost::handlers::chat::SocketManager>()
        .Append<bifrost::app::Application>()
        .Append<bifrost::handlers::auth::AuthRegistrationHandler>()
        .Append<bifrost::app::registry::RegistryComponent>()
        .Append<bifrost::app::auth::AuthComponent>()
        .Append<userver::components::Postgres>("postgres-db-1")
        // Testuite components:
        .Append<server::handlers::TestsControl>()
        .Append<components::TestsuiteSupport>()
        .Append<clients::dns::Component>()
        .Append<components::HttpClient>();
    return utils::DaemonMain(argc, argv, component_list);
}