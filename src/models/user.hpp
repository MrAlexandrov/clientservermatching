#pragma once

#include <string>
#include <vector>
#include "order.hpp"
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace NMatching {

class IUser {
public:
    virtual std::string         GetId() const = 0;
    virtual std::string         GetUsername() const = 0;
    virtual std::string         GetHashedPassword() const = 0;
    // virtual TOrder              GetOrder(const int32_t& order_id) const = 0;
    // virtual std::vector<TOrder> GetOrders() const = 0;
    virtual double              GetBalance() const = 0;

    virtual void                CreateOrder() = 0;
    virtual void                DeleteOrder(const int32_t& order_id) = 0;
};

struct UserData {
    std::string id;
    std::string username;
    std::string hashed_password;
};

class TUser final : public IUser {
public:
    TUser() = default;
    TUser(const UserData&);
    TUser(UserData&&);
    std::string         GetId() const override;
    std::string         GetUsername() const override;
    std::string         GetHashedPassword() const override;
    // TOrder              GetOrder(const int32_t& order_id) const override;
    // std::vector<TOrder> GetOrders() const override;
    double              GetBalance() const override;

    void                CreateOrder() override;
    void                DeleteOrder(const int32_t& order_id) override;

    auto Introspect() const {
        return std::tie(data_.id, data_.username, data_.hashed_password);
    }
public:
    UserData data_;
};


userver::formats::json::Value
Serialize(const TUser& user,
          userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace NMatching