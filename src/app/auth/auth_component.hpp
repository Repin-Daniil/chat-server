#pragma once

#include "userver/components/component_base.hpp"
#include <userver/components/component_context.hpp>

#include "stand_alone/auth_manager.hpp"

namespace bifrost::app::auth {
class AuthComponent final : public userver::components::LoggableComponentBase {
public:
    static constexpr std::string_view kName = "auth-manager";

    AuthComponent(const userver::components::ComponentConfig& config,
                  const userver::components::ComponentContext& context);

    [[nodiscard]] AuthManager& GetAuthManager() const;

    static userver::yaml_config::Schema GetStaticConfigSchema();

private:
    std::unique_ptr<AuthManager> auth_manager_;
};
}