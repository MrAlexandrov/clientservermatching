#pragma once

#include <string>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/decimal64/decimal64.hpp>

namespace NMatching {
    
using Number = userver::decimal64::Decimal<4, userver::decimal64::HalfEvenRoundPolicy>;

struct TBalance {
    std::string user_id;
    Number usd_balance;
    Number rub_balance;
};

userver::formats::json::Value
Serialize(const TBalance& balance,
          userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace NMatching