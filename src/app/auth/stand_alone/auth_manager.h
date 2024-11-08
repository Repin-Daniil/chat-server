#pragma once
#include "userver/rcu/rcu_map.hpp"

namespace bifrost::app::auth {
using Token = std::string;

struct AuthData {
    std::string login;
    // time expiration time unix-время наверное;
};

class AuthManager {
public:
    Token AuthenticateUser(std::string login, std::string password);

    bool VerifyToken(std::string login, Token token);

private:
    userver::rcu::RcuMap<Token, AuthData> token_map_;

    // pg cluster
    //redis cluster
};
}