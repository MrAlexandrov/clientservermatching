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

using Number = userver::decimal64::Decimal<4, userver::decimal64::HalfEvenRoundPolicy>;

class IOrder {
public:

};

struct OrderData {    
    std::string id;
    std::string user_id;
    std::string type;
    Number price;
    Number amount;
    std::string status;
    userver::storages::postgres::TimePointWithoutTz created_ts;
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