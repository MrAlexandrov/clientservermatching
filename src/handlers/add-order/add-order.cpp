#include "add-order.hpp"

#include <userver/components/component_config.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/io/chrono.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <string>
#include <userver/components/component.hpp>

#include <userver/decimal64/decimal64.hpp>


#include "../lib/auth.hpp"
#include "../../models/order.hpp"
#include "../../service/match_orders.hpp"

namespace NMatching {

namespace {

using Decimal = userver::decimal64::Decimal<2>;

class AddOrder final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-add-order";

    AddOrder(const userver::components::ComponentConfig& config,
                const userver::components::ComponentContext& component_context)
        : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()
        ) {} 

    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&
    ) const override {
        auto session_opt = GetSessionInfo(pg_cluster_, request);
        if (!session_opt.has_value()) {
            auto &response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kUnauthorized);
            return userver::formats::json::ToString(
                userver::formats::json::MakeObject("error",
                                                    "user is not authorized")
            );
        }

        auto user_id = session_opt.value().user_id;

        auto request_body =
            userver::formats::json::FromString(request.RequestBody());

        if (!request_body.HasMember("price") || !request_body.HasMember("amount") || !request_body.HasMember("order_type")) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "missing required parameters"
                )
            );
        }

        auto price_string = request_body["price"].As<std::string>();
        auto amount_string = request_body["amount"].As<std::string>();
        auto order_type_string = request_body["order_type"].As<std::string>();

        if (amount_string.empty() || price_string.empty() || order_type_string.empty()) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "missing required parameters"
                )
            );
        }

        if (!(order_type_string == "buy" || order_type_string == "sell")) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "order type could be buy/sell"
                )
            );
        }

        Number zero{};
        Number price;
        Number amount;
        try {
            price = Number(price_string);
            amount = Number(amount_string);
        } catch (...) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "can't convert numbers"
                )
            );
        }

        if (price <= zero || amount <= zero) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "numbers should be more than zero"
                )
            );
        }
        
        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "INSERT INTO exchange.orders(user_id, order_type, price, amount, created_ts) "
            "VALUES ($1, $2, $3, $4, $5) "
            "RETURNING * ",
            user_id, 
            order_type_string, 
            Number(price_string), 
            Number(amount_string), 
            userver::utils::datetime::Now()
        );

        if (result.IsEmpty()) {
            auto &response = request.GetHttpResponse();
            response.SetStatusServiceUnavailable();
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error", 
                    "error, creating order"));
        }

        auto matcher = Match(pg_cluster_);

        auto added = result.AsSingleRow<TOrder>(userver::storages::postgres::kRowTag);

        matcher.MatchOrders(added);

        userver::formats::json::ValueBuilder response;
        response = added;
        return userver::formats::json::ToPrettyString(response.ExtractValue());
    }
private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace

void AppendAddOrder(userver::components::ComponentList &component_list) {
    component_list.Append<AddOrder>();
}

} // namespace NMatching