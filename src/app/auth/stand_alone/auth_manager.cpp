#include "auth_manager.hpp"

#include "db/sql.hpp"
#include "userver/storages/postgres/cluster.hpp"
#include "userver/storages/postgres/cluster_types.hpp"

namespace chat::app::auth {
AuthManager::AuthManager(userver::storages::postgres::ClusterPtr pg_cluster) : pg_cluster_(std::move(pg_cluster)) {}

std::pair<Token, bool> AuthManager::AuthenticateUser(std::string_view login, std::string_view password) {
    bool is_new_user = false;
    //TODO не давать регистрировать пользователя Server
    try {
        is_new_user =
            pg_cluster_->Execute(userver::storages::postgres::ClusterHostType::kSlave, sql::kFindUser, login).IsEmpty();

        if (is_new_user) {
            pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kMaster, sql::kInsertUser, login, password
            );
            LOG_INFO() << "Register new user " << login;
        } else {
            auto check_pwd_result = pg_cluster_->Execute(
                userver::storages::postgres::ClusterHostType::kSlave, sql::kCheckPassword, login, password
            );

            if (check_pwd_result.IsEmpty()) {
                LOG_DEBUG() << "User " << login << "entered wrong password";

                return {};
            }
        }
    } catch (const std::exception& ex) {
        LOG_ERROR() << "Something go wrong with DB, user " << login << " not confirmed. Exception: " << ex;
    }

    LOG_INFO() << "Authenticate user " << login;

    return {IssueToken(login), is_new_user};
}

bool AuthManager::VerifyToken(std::string_view login, const Token& token) {
    auto ptr = token_map_.Get(token.data());

    return ptr && (*ptr == login);
}

Token AuthManager::IssueToken(std::string_view login) {
    Token token;

    // TODO Проверять если уже был выпущен токен и он не expired, то его аннулируем и удаляем и делаем новый

    do {
        token = generator_.GenerateNewToken();
        auto [auth_data, is_inserted] = token_map_.TryEmplace(token, login.data());
        token = (is_inserted ? token : "");  // FIXME проверить!
    } while (token.empty());

    return token;
}
}  // namespace chat::app::auth