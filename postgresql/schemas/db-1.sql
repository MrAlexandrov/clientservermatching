CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

DROP SCHEMA IF EXISTS exchange CASCADE;

CREATE SCHEMA IF NOT EXISTS exchange;

-- Таблица для пользователей с хешированным паролем
CREATE TABLE IF NOT EXISTS exchange.users (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    username TEXT UNIQUE NOT NULL,
    hashed_password TEXT NOT NULL  -- Хешированный пароль
);

-- Таблица для сессий авторизации
CREATE TABLE IF NOT EXISTS exchange.auth_sessions (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id TEXT UNIQUE NOT NULL,
    FOREIGN KEY (user_id) REFERENCES exchange.users(id)
);

-- Таблица для заявок на покупку/продажу
CREATE TABLE IF NOT EXISTS exchange.orders (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    user_id TEXT NOT NULL,
    order_type TEXT NOT NULL CHECK (order_type IN ('buy', 'sell')),
    price NUMERIC(10, 2) NOT NULL,
    amount NUMERIC(10, 2) NOT NULL,
    status TEXT NOT NULL DEFAULT 'active' CHECK (status IN ('active', 'partially_filled', 'filled', 'cancelled')),
    created_ts TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES exchange.users(id)
);

-- Таблица для сделок
CREATE TABLE IF NOT EXISTS exchange.deals (
    id TEXT PRIMARY KEY DEFAULT uuid_generate_v4(),
    buy_order_id TEXT NOT NULL,
    sell_order_id TEXT NOT NULL,
    amount NUMERIC(10, 2) NOT NULL,
    price NUMERIC(10, 2) NOT NULL,
    timestamp TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (buy_order_id) REFERENCES exchange.orders(id),
    FOREIGN KEY (sell_order_id) REFERENCES exchange.orders(id)
);

-- Таблица для балансов пользователей
CREATE TABLE IF NOT EXISTS exchange.balances (
    user_id TEXT PRIMARY KEY,
    usd_balance NUMERIC(15, 2) NOT NULL DEFAULT 0,
    rub_balance NUMERIC(15, 2) NOT NULL DEFAULT 0,
    FOREIGN KEY (user_id) REFERENCES exchange.users(id)
);
