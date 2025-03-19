#pragma once
#include "userver/rcu/rcu_map.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include "utils/token/token.hpp"

namespace chat::app::auth {

using Token = std::string;

class AuthManager {
public:
    explicit AuthManager(userver::storages::postgres::ClusterPtr pg_cluster);

    std::pair<Token, bool> AuthenticateUser(std::string_view login, std::string_view password);

    bool VerifyToken(std::string_view login, const Token& token);

private:
    userver::rcu::RcuMap<Token, std::string> token_map_{};
    // TODO RcuMap<Login, time> expiration_map, чтобы следить чтобу одному опльзователю соотвт один токен, заодно
    // инвалидацию проводить так
    userver::storages::postgres::ClusterPtr pg_cluster_;
    utils::TokenGenerator generator_;

    Token IssueToken(std::string_view login);
};
}  // namespace chat::app::auth