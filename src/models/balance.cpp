#include "balance.hpp"

namespace NMatching {

userver::formats::json::Value
Serialize(const TBalance& user,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["usd"] = user.usd_balance;
    item["rub"] = user.rub_balance;
    return item.ExtractValue();
}

} // namespace NMatching