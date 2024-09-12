#include "order.hpp"
#include <string>

namespace NMatching {

userver::formats::json::Value
Serialize(const TOrder& data,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["id"] = data.id;
    item["user_id"] = data.user_id;
    item["order_type"] = data.type;
    item["price"] = data.price;
    item["amount"] = data.amount;
    item["status"] = data.status;
    item["created_ts"] = data.created_ts;
    return item.ExtractValue();
}

} // namespace NMatching
