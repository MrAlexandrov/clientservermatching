#include "user.hpp"
#include <tuple>
#include <vector>
#include "order.hpp"

namespace NMatching {

userver::formats::json::Value
Serialize(const TUser& user,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["id"] = user.id;
    item["username"] = user.username;
    item["hashed_password"] = user.hashed_password;
    return item.ExtractValue();
}

} // namespace NMatching