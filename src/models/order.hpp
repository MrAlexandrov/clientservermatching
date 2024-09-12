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

struct TOrder {    
    std::string id;
    std::string user_id;
    std::string type;
    Number price;
    Number amount;
    std::string status;
    userver::storages::postgres::TimePoint created_ts;
};

userver::formats::json::Value
Serialize(const TOrder& data,
          userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace NMatching