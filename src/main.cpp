#include <userver/clients/dns/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/server_monitor.hpp>
#include <userver/utils/daemon_run.hpp>
#include "userver/components/component_list.hpp"
#include <userver/clients/http/component.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/storages/postgres/component.hpp>

#include "app/application_component.hpp"
#include "app/registry/registry_component.hpp"
#include "app/auth/auth_component.hpp"

#include "handlers/chat/socket_manager.hpp"
#include "handlers/auth/auth_handler.hpp"
#include <userver/server/handlers/ping.hpp>

int main(int argc, const char* const argv[]) {
    const auto component_list = userver::components::MinimalServerComponentList()
        .Append<userver::server::handlers::Ping>()
        .Append<userver::server::handlers::ServerMonitor>()
        .Append<bifrost::handlers::chat::SocketManager>()
        .Append<bifrost::app::Application>()
        .Append<bifrost::handlers::auth::AuthHandler>()
        .Append<bifrost::app::registry::RegistryComponent>()
        .Append<bifrost::app::auth::AuthComponent>()
        .Append<userver::components::Postgres>("bifrost-database")
        // Testuite components:
        .Append<userver::server::handlers::TestsControl>()
        .Append<userver::components::TestsuiteSupport>()
        .Append<userver::clients::dns::Component>()
        .Append<userver::components::HttpClient>();

    return userver::utils::DaemonMain(argc, argv, component_list);
}