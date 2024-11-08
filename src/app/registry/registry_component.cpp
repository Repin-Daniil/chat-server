#include "registry_component.h"

namespace bifrost::app::registry {

RegistryComponent::RegistryComponent(const userver::components::ComponentConfig& config,
                              const userver::components::ComponentContext& context) : LoggableComponentBase(config,
        context) {
    //TODO Прочитать конфиг и сконструировать нужный реестр, или редис или сенд алон
}

UserRegistry& RegistryComponent::GetRegistry() {
    return registry_;
}
}