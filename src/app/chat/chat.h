#pragma once

#include "app/auth/stand_alone/auth_manager.h"
#include "app/registry/stand_alone/user_registry.h"

namespace bifrost::app {
class Chat {
public:
    Chat(auth::AuthManager& auth, registry::UserRegistry &registry);
    auth::Token AuthenticateUser(const Login& login, const std::string& password);
    bool Verify(const Login& login, const auth::Token & token);

    bool Send(const Login& recipient, Message message);

    std::shared_ptr<Queue> Register(const Login& new_recipient);

    // GetOnlineUsers
    // Stop receiveing
private:
    auth::AuthManager& auth_;
    registry::UserRegistry& registry_;
    // archive
};
}