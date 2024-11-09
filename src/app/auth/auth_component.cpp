#include "auth_component.h"

#include "userver/storages/postgres/component.hpp"

namespace bifrost::app::auth {
AuthComponent::AuthComponent(const userver::components::ComponentConfig& config,
                             const userver::components::ComponentContext& context) : LoggableComponentBase(config,
        context), auth_manager_(context.FindComponent<userver::components::Postgres>("postgres-db-1").GetCluster()) {
    //TODO Считать из конфига stand-alone и сконсрутировать нужного AuthManager (редис или рсу мапа)
}

AuthManager& AuthComponent::GetAuthManager() {
    return auth_manager_;
}
} // bifrost