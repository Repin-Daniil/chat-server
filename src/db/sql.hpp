#pragma once
#include "userver/storages/postgres/query.hpp"

namespace chat::sql {
const userver::storages::postgres::Query kInsertUser{
    R"~(WITH generated_salt AS (SELECT gen_salt('bf') AS salt)
                                                         INSERT INTO chat.users (login, password, salt)
                                                         SELECT $1, crypt($2, salt), salt
                                                         FROM generated_salt;)~",
    userver::storages::postgres::Query::Name{"insert_user"}
};

const userver::storages::postgres::Query kFindUser{
    R"~(SELECT id FROM chat.users  WHERE login = $1;)~",
    userver::storages::postgres::Query::Name{"update_game_session"}
};

const userver::storages::postgres::Query kCheckPassword{
    R"~(SELECT id FROM chat.users
                                                        WHERE login = $1 AND password = crypt($2, salt);)~",
    userver::storages::postgres::Query::Name{"update_game_session"}
};

}  // namespace chat::sql