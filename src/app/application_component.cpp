#include "application_component.h"

#include "registry/registry_component.h"
#include "auth/auth_component.h"

namespace bifrost::app {
Application::Application(const userver::components::ComponentConfig& config,
                         const userver::components::ComponentContext& context) : LoggableComponentBase(config,
        context),
    chat_({context.FindComponent<auth::AuthComponent>().GetAuthManager(), context.FindComponent<registry::RegistryComponent>().GetRegistry()}) {
}

Chat& Application::GetApp() {
    return chat_;
}
} // bifrost