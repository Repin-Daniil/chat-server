#pragma once
#include "userver/rcu/rcu_map.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"

#include <iomanip>
#include <random>

namespace bifrost::app::auth {
using Token = std::string;

struct AuthData {
    std::string login;
    // time expiration time unix-время наверное;
};

class TokenGenerator {
public:
    std::string GenerateNewToken() {
        std::stringstream ss;

        while (ss.str().size() != 32) {
            ss.clear();
            ss << std::setfill('0') << std::setw(16) << std::hex << generator1_();
            ss << std::setfill('0') << std::setw(16) << std::hex << generator2_();
        }

        return ss.str();
    }

private:
    std::random_device random_device_;
    std::mt19937_64 generator1_{
        [this] {
            std::uniform_int_distribution<std::mt19937_64::result_type> dist;
            return dist(random_device_);
        }()
    };

    std::mt19937_64 generator2_{
        [this] {
            std::uniform_int_distribution<std::mt19937_64::result_type> dist;
            return dist(random_device_);
        }()
    };
};

class AuthManager {
public:
    AuthManager(userver::storages::postgres::ClusterPtr pg_cluster);

    Token AuthenticateUser(const std::string& login, const std::string& password);

    bool VerifyToken(const std::string& login, const Token& token);

private:
    userver::rcu::RcuMap<Token, std::string> token_map_;
    userver::storages::postgres::ClusterPtr pg_cluster_;
    TokenGenerator generator_;

    //redis cluster

};
}