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


    void MatchOrders(TOrder&);

private:
    void ExecuteDeal(const TOrder&, const TOrder&, const Number&);
    void UpdateOrderAmount(TOrder&, const Number&);
    void UpdateBalances(const TOrder&, const TOrder&, const Number&);

    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace NMatching