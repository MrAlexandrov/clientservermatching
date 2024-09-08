#pragma once

#include <string>
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>
#include <userver/decimal64/decimal64.hpp>

namespace NMatching {

struct TBalance {
    std::string user_id;
    userver::decimal64::Decimal<2> usd_balance;
    userver::decimal64::Decimal<2> rub_balance;
};

userver::formats::json::Value
Serialize(const TBalance& user,
          userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace NMatching