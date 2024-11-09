WITH generated_salt AS (
    SELECT gen_salt('bf') AS salt
)

INSERT INTO users (login, password, salt)
SELECT
    'Ivan',  -- Логин
    crypt('ivan_password', salt),  -- Хеш пароля с солью
    salt  -- Соль
FROM generated_salt;