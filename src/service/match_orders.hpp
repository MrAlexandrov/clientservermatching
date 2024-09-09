#pragma once

#include "../models/order.hpp"
#include <userver/storages/postgres/cluster.hpp>
#include "userver/storages/postgres/postgres_fwd.hpp"

namespace NMatching {

class Match final {
public:
    Match(const userver::storages::postgres::ClusterPtr& other) : pg_cluster_(other) {}

    Match(userver::storages::postgres::ClusterPtr&& other) = delete;
    Match& operator=(userver::storages::postgres::ClusterPtr&& other) = delete;


    void MatchOrders(const OrderData&);

private:
    void ExecuteDeal(const OrderData&, const OrderData&, const Number&);
    void UpdateOrderAmount(const OrderData&, const Number&);
    void UpdateBalances(const OrderData&, const OrderData&, const Number&);

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace NMatching