#include "registry_component.hpp"

#include <userver/components/component.hpp>
#include <userver/yaml_config/merge_schemas.hpp>

namespace bifrost::app::registry {

RegistryComponent::RegistryComponent(const userver::components::ComponentConfig& config,
                              const userver::components::ComponentContext& context) : LoggableComponentBase(config,
        context) {
    if (config["stand-alone"].As<bool>()) {
       registry_ = std::make_unique<UserRegistry>();
    } else {

    }
}


userver::yaml_config::Schema RegistryComponent::GetStaticConfigSchema() {
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

UserRegistry& RegistryComponent::GetRegistry() const {
    return *registry_;
}
}