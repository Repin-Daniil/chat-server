#pragma once
#include <userver/components/component_context.hpp>
#include "userver/components/component_base.hpp"

#include "stand_alone/user_registry.hpp"

namespace chat::app::registry {

class RegistryComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "registry";

    RegistryComponent(
        const userver::components::ComponentConfig& config,
        const userver::components::ComponentContext& context
    );

    static userver::yaml_config::Schema GetStaticConfigSchema();

    [[nodiscard]] UserRegistry& GetRegistry() const;

private:
    std::unique_ptr<UserRegistry> registry_;
};

}  // namespace chat::app::registry