#pragma once

#include <string>
#include <vector>
#include "order.hpp"
#include <userver/formats/json/value_builder.hpp>
#include <userver/formats/serialize/common_containers.hpp>

namespace NMatching {

struct TUser {
    std::string id;
    std::string username;
    std::string hashed_password;
};

userver::formats::json::Value
Serialize(const TUser& user,
          userver::formats::serialize::To<userver::formats::json::Value>);

} // namespace NMatching