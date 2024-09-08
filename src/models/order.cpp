#include "order.hpp"
#include <string>

namespace NMatching {

TOrder::TOrder(const OrderData& other) : data_(other) {}

TOrder::TOrder(OrderData&& other) : data_(std::move(other)) {}

userver::formats::json::Value
Serialize(const OrderData& data,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["id"] = data.id;
    item["user_id"] = data.user_id;
    item["order_type"] = data.type;
    item["price"] = std::to_string(data.price);
    item["amount"] = std::to_string(data.amount);
    item["status"] = data.status;
    item["created_ts"] = data.created_ts;
    return item.ExtractValue();
}

} // namespace NMatching
