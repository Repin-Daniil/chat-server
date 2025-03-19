#pragma once

#include "userver/logging/log.hpp"
#include "userver/rcu/rcu_map.hpp"

#include "receiver.hpp"

namespace bifrost::app::registry {

class UserRegistry {
public:
    std::shared_ptr<Queue> AddReceiver(const Login& new_recipient);
    bool Send(const Login& recipient, Message message);
    [[nodiscard]] std::size_t GetApproxRegistrySize() const;
    // std::vector<Login> GetReceivers() const; //TODO для ручки получить онлайн пользователей
    // void RemoveReceiver(const Login& retired_recipient); //TODO для ручки, которая будет удалять ресивера, если
    // клиент больше не хочет принимать сообщения void AddRoom(); void DeleteRoom();
    // TODO Нужно будет посмотреть планы выполнения запросов для всех, может навесить индексы

private:
    userver::rcu::RcuMap<Login, Receiver> login_to_receiver_;
    // TODO redis cluster
};
}  // namespace bifrost::app::registry
