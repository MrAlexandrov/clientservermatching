#include "delete-order.hpp"

#include "userver/components/component_config.hpp"
#include "userver/components/component_fwd.hpp"
#include "userver/formats/json/inline.hpp"
#include "userver/formats/json/value_builder.hpp"
#include "userver/http/status_code.hpp"
#include "userver/server/handlers/http_handler_base.hpp"
#include "userver/server/http/http_request.hpp"
#include "userver/server/request/request_context.hpp"
#include "userver/storages/postgres/cluster_types.hpp"
#include "userver/storages/postgres/component.hpp"
#include "userver/storages/postgres/io/row_types.hpp"
#include "userver/storages/postgres/postgres_fwd.hpp"
#include <userver/components/component.hpp>


#include "../lib/auth.hpp"
#include "../../models/order.hpp"

namespace NMatching {

namespace {

class DeleteOrder final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-delete-order";

    DeleteOrder(const userver::components::ComponentConfig& config,
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

        // auto session = session_opt.value();
        auto user_id = session_opt.value().user_id;

        auto& order_id = request.GetPathArg("id");
        if (order_id.empty()) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "missing required parameters"
                )
            );
        }

        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "DELETE FROM exchange.orders "
            "WHERE id = $1 AND user_id = $2 ",
            order_id, user_id
        );

        if (result.RowsAffected() == 0) {
            auto &response = request.GetHttpResponse();
            response.SetStatusNotFound();
            return userver::formats::json::ToString(
                userver::formats::json::MakeObject("error", "no such order"));
        }

        userver::formats::json::ValueBuilder response;
        
        response["id"] = order_id;

        return userver::formats::json::ToPrettyString(response.ExtractValue());
    }
private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace

void AppendDeleteOrder(userver::components::ComponentList &component_list) {
    component_list.Append<DeleteOrder>();
}

} // namespace NMatching