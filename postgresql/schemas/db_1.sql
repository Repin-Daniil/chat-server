DROP SCHEMA IF EXISTS bifrost CASCADE;


CREATE SCHEMA IF NOT EXISTS bifrost;

CREATE EXTENSION pgcrypto;
CREATE TABLE IF NOT EXISTS users (
    id SERIAL PRIMARY KEY,
    login varchar(100),
    password text,
    salt text,
    registered timestamp DEFAULT(now())
);