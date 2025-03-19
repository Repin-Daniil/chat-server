#include "application_component.hpp"

#include "auth/auth_component.hpp"
#include "registry/registry_component.hpp"

namespace chat::app {
Application::Application(
    const userver::components::ComponentConfig& config,
    const userver::components::ComponentContext& context
)
    : LoggableComponentBase(config, context),
      chat_(
          {context.FindComponent<auth::AuthComponent>().GetAuthManager(),
           context.FindComponent<registry::RegistryComponent>().GetRegistry()}
      ) {}

Chat& Application::GetApp() { return chat_; }
}  // namespace chat::app