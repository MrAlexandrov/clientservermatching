#include "balance.hpp"

namespace NMatching {

userver::formats::json::Value
Serialize(const TBalance& balance,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["usd"] = balance.usd_balance;
    item["rub"] = balance.rub_balance;
    return item.ExtractValue();
}

} // namespace NMatching