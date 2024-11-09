#pragma once
#include "userver/components/component_base.hpp"
#include <userver/components/component_context.hpp>

#include "stand_alone/user_registry.h"

namespace bifrost::app::registry {

class RegistryComponent : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "registry";

    RegistryComponent(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& context);

    UserRegistry& GetRegistry();

    ~RegistryComponent() override = default;

private:
    UserRegistry registry_;
};

}