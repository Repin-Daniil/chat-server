#include "auth_manager.h"


namespace bifrost::app::auth {
AuthManager::AuthManager(userver::storages::postgres::ClusterPtr pg_cluster) : pg_cluster_(pg_cluster) {
}

//FIXME Вынести в отдельный классс

Token AuthManager::AuthenticateUser(const std::string& login, const std::string& password) {
    // Проверить действительно ли этому пользователлю соответствует такой пароль

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