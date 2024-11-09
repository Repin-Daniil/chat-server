#include "auth_manager.h"

#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/cluster_types.hpp"

namespace {
const userver::storages::postgres::Query kInsertUser{
    "WITH generated_salt AS (SELECT gen_salt('bf') AS salt) INSERT INTO users (login, password, salt) SELECT $1, crypt($2, salt) salt FROM generated_salt;",
    userver::storages::postgres::Query::Name{"insert_user"}
};
const userver::storages::postgres::Query kFindUser{
    "SELECT id FROM users  WHERE login = $1;",
    userver::storages::postgres::Query::Name{"update_game_session"}};

const userver::storages::postgres::Query kCheckPassword{
    "SELECT id FROM users  WHERE login = $1 AND password = crypt($2, salt);",
    userver::storages::postgres::Query::Name{"update_game_session"}};
} // namespace

namespace bifrost::app::auth {
AuthManager::AuthManager(userver::storages::postgres::ClusterPtr pg_cluster) : pg_cluster_(pg_cluster) {
}

Token AuthManager::AuthenticateUser(const std::string& login, const std::string& password) {
    // Проверить действительно ли этому пользователлю соответствует такой пароль

     auto result_insert = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, kFindUser, login, password);

    if (result_insert.FieldCount() == 0) {
        pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, kInsertUser, login);
    } else {
        auto check_result = pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kMaster, kCheckPassword, login, password);

        if (check_result.FieldCount() == 0) {
            return {};
        }
    }

    auto token = generator_.GenerateNewToken();

    //Нужна Бимапа, чтобы одному пользователю соответствовал один токен

    bool is_token_unique = false;

    do {
        auto [auth_data, flag] = token_map_.TryEmplace(token, login);
        is_token_unique = flag;
    } while (!is_token_unique);

    return token;
}

bool AuthManager::VerifyToken(const std::string& login, const Token& token) {
    auto ptr = token_map_.Get(token);

    return ptr && (*ptr == login);
}
}