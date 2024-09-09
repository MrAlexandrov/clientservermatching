#include "match_orders.hpp"

#include <cassert>
#include <userver/storages/postgres/transaction.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/options.hpp>

namespace NMatching {

void Match::MatchOrders(const OrderData &new_order) {
    std::string opposite_type;
    if (new_order.type == "buy") {
        opposite_type = "sell";
        auto matching_orders = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlave,
            "SELECT * FROM exchange.orders "
            "WHERE order_type = $1 AND price <= $2 AND "
            "(status = 'active' OR status = 'partially_filled') AND user_id != $3 "
            "ORDER BY price ASC, created_ts ASC ",
            opposite_type, Number(new_order.price), new_order.id
        ).AsSetOf<OrderData>(userver::storages::postgres::kRowTag); 

        for (const auto& order : matching_orders) {
            auto zero = Number{};
            auto new_order_amount = Number(new_order.amount);
            auto order_amount = Number(order.amount);
            if (new_order_amount > zero && order_amount > zero) {
                auto deal_amount = std::min(new_order_amount, order_amount);
                ExecuteDeal(new_order, order, deal_amount);

                UpdateOrderAmount(new_order, deal_amount);
                UpdateOrderAmount(order, deal_amount);

                UpdateBalances(new_order, order, deal_amount);
            }
        }
    } else {
        opposite_type = "sell";
        auto matching_orders = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlave,
            "SELECT * FROM exchange.orders "
            "WHERE order_type = $1 AND price >= $2 AND "
            "(status = 'active' OR status = 'partially_filled') AND user_id != $3 "
            "ORDER BY price ASC, created_ts ASC ",
            opposite_type, Number(new_order.price), new_order.id
        );
    }
}

void Match::ExecuteDeal(const OrderData& buy_order, const OrderData& sell_order, const Number& amount) {
    // auto transaction = pg_cluster_->Begin(userver::storages::postgres::Transaction::RW);
    //,userver::storages::postgres::TransactionOptions::kReadWrite 
    // userver::storages::postgres::ClusterHostType::kMaster
    // transaction.Commit();

    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "INSERT INTO exchange.deals (buy_order_id, sell_order_id, amount, price) "
        "VALUES ($1, $2, $3, $4) ",
        buy_order.id, sell_order.id, amount, buy_order.price
    );
}

void Match::UpdateOrderAmount(const OrderData& order, const Number& amount) {
    OrderData changed_order{order};
    changed_order.amount -= amount;
    
    changed_order.status = (changed_order.amount > Number{}) ? "partially_filled" : "filled";

    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE exchange.orders SET amount = $1, status = $2 WHERE id = $3 ",
        changed_order.amount, changed_order.status, changed_order.id
    );
}

void Match::UpdateBalances(const OrderData& buy_order, const OrderData& sell_order, const Number& amount) {
    Number deal_value = amount * buy_order.price;

    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE exchange.balances SET usd_balance = usd_balance + $1, "
        "rub_balance = rub_balance - $2 " 
        "WHERE user_id = $3 ",
        amount, deal_value, buy_order.user_id
    );

    pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "UPDATE exchange.balances SET usd_balance = usd_balance - $1, "
        "rub_balance = rub_balance + $2 "
        "WHERE user_id = $3 ",
        amount, deal_value, sell_order.user_id
    );
}

} // namespace NMatching