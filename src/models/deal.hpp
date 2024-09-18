#pragma once

#include <string>
#include <userver/decimal64/decimal64.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/formats/json/value_builder.hpp>

namespace NMatching {

using Number = userver::decimal64::Decimal<4, userver::decimal64::HalfEvenRoundPolicy>;

struct TDeal {
    std::string id;
    std::string buy_order_id;
    std::string sell_order_id;
    Number amount;
    Number price;
    userver::storages::postgres::TimePoint timestamp;
};

userver::formats::json::Value
Serialize(const TDeal& data,
          userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace NMatching
