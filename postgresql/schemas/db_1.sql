DROP SCHEMA IF EXISTS chat CASCADE;

CREATE SCHEMA IF NOT EXISTS chat;

CREATE EXTENSION IF NOT EXISTS pgcrypto SCHEMA chat;

CREATE TABLE IF NOT EXISTS chat.users (
                                             id         SERIAL PRIMARY KEY,
                                             login      TEXT        NOT NULL
                                                 CONSTRAINT chk_login_length CHECK (LENGTH(login) BETWEEN 1 AND 100),
                                             password   TEXT        NOT NULL
                                                 CONSTRAINT chk_password_length CHECK (LENGTH(password) BETWEEN 8 AND 100),
                                             salt       TEXT        NOT NULL,
                                             registered TIMESTAMPTZ NOT NULL DEFAULT NOW()
);

ALTER TABLE chat.users ADD CONSTRAINT unique_login UNIQUE (login);

CREATE UNIQUE INDEX IF NOT EXISTS user_login_idx ON chat.users (login);
