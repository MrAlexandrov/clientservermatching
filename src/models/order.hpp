#pragma once

#include <chrono>
#include <string>
#include <type_traits>
#include <string_view>
#include <unordered_map>
#include <userver/decimal64/decimal64.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace NMatching {

class IOrder {
public:

};

struct OrderData {    
    std::string id;
    std::string user_id;
    std::string type;
    double price;
    double amount;
    std::string status;
    std::chrono::time_point<std::chrono::system_clock> created_ts;
};

userver::formats::json::Value
Serialize(const OrderData& data,
          userver::formats::serialize::To<userver::formats::json::Value>);

class TOrder final : public IOrder {
public:
    TOrder() = default;
    TOrder(const OrderData&);
    TOrder(OrderData&&);

private:
    OrderData data_;
};

} // namespace NMatching