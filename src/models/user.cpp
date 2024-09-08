#include "user.hpp"
#include <tuple>
#include <vector>
#include "order.hpp"

namespace NMatching {

TUser::TUser(const UserData& other) : data_(other) {}

TUser::TUser(UserData&& other) : data_(std::move(other)) {}

std::string TUser::GetUsername() const { return data_.username; }

std::string TUser::GetHashedPassword() const { return data_.hashed_password; }

std::string TUser::GetId() const { return data_.id; }

// std::string TUser::GetUsername() const { return username; }

// std::string TUser::GetHashedPassword() const { return hashed_password; }

// std::string TUser::GetId() const { return id; }

// TOrder TUser::GetOrder(const int32_t& order_id) const {
//     ///
// }

// std::vector<TOrder> TUser::GetOrders() const {
//     ///
// }

double TUser::GetBalance() const {
    ///
}

void TUser::CreateOrder() {
    ///
}

void TUser::DeleteOrder(const int32_t& order_id) {
    ///
}

userver::formats::json::Value
Serialize(const TUser& user,
          userver::formats::serialize::To<userver::formats::json::Value>) {
    userver::formats::json::ValueBuilder item;
    item["id"] = user.GetId();
    item["username"] = user.GetUsername();
    item["hashed_password"] = user.GetHashedPassword();
    return item.ExtractValue();
}

// std::tuple<std::string, std::string, std::string> TUser::Introspect() const {
//     return std::tie(data_.id, data_.username, data_.hashed_password);
// }

// userver::formats::json::Value
// Serialize(const TBookmark &bookmark,
//           userver::formats::serialize::To<userver::formats::json::Value>) {
//   userver::formats::json::ValueBuilder item;
//   item["id"] = bookmark.id;
//   item["url"] = bookmark.url;
//   item["title"] = bookmark.title;
//   if (bookmark.tag.has_value()) {
//     item["tag"] = bookmark.tag;
//   }
//   item["created_ts"] = bookmark.created_ts;
//   return item.ExtractValue();
// }

} // namespace NMatching