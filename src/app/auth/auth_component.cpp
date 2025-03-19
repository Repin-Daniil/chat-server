#include "auth_component.hpp"

#include <userver/components/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>
#include "userver/storages/postgres/component.hpp"

namespace chat::app::auth {
AuthComponent::AuthComponent(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context) {
    if (config["stand-alone"].As<bool>()) {
        auth_manager_ = std::make_unique<AuthManager>(
            context.FindComponent<userver::components::Postgres>("chat-database").GetCluster()
        );
    } else {
    }
}

AuthManager& AuthComponent::GetAuthManager() const { return *auth_manager_; }

userver::yaml_config::Schema AuthComponent::GetStaticConfigSchema() {
    return userver::yaml_config::MergeSchemas<userver::components::ComponentBase>(R"(
type: object
description: auth component
additionalProperties: false
properties:
    stand-alone:
        type: boolean
        description: stand-alone or replica set
)");
}
}  // namespace chat::app::auth