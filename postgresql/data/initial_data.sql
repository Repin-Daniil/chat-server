WITH generated_salt AS (SELECT gen_salt('bf') AS salt)

INSERT INTO chat.users (login, password, salt)
SELECT 'Ivan', crypt('ivan_password', salt), salt
FROM generated_salt;

INSERT INTO chat.users (login, password, salt)
SELECT 'Alice', crypt('strong_password', salt), salt
FROM generated_salt;