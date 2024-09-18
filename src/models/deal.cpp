#include "deal.hpp"

namespace NMatching {

userver::formats::json::Value
Serialize(const TDeal& data,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["id"] = data.id;
    item["buy_order_id"] = data.buy_order_id;
    item["sell_order_id"] = data.sell_order_id;
    item["amount"] = data.amount;
    item["price"] = data.price;
    item["timestamp"] = data.timestamp;
    return item.ExtractValue();
}

} // namespace NMatching
