#include "auth_component.h"

namespace bifrost::app::auth {

AuthComponent::AuthComponent(const userver::components::ComponentConfig& config,
                              const userver::components::ComponentContext& context) : LoggableComponentBase(config,
        context) {
    //TODO Считать из конфига stand-alone и сконсрутировать нужного AuthManager (редис или рсу мапа)
 //TODO Вкинуть сюда кластер постгреса
}

AuthManager& AuthComponent::GetAuthManager() {
    return auth_manager_;
}
} // bifrost