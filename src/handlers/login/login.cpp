#include "login.hpp"
#include <userver/components/component_config.hpp>
#include <userver/components/component_fwd.hpp>
#include <userver/components/component_list.hpp>
#include <userver/components/component_context.hpp>
#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/postgres_fwd.hpp>
#include <userver/formats/json/inline.hpp>
#include <userver/formats/json/value_builder.hpp>
#include <userver/http/status_code.hpp>
#include <userver/server/http/http_request.hpp>
#include <userver/server/request/request_context.hpp>
#include <userver/storages/postgres/cluster_types.hpp>
#include <userver/storages/postgres/io/row_types.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/crypto/hash.hpp>

#include "../../models/user.hpp"

namespace NMatching {

namespace {

class LoginUser final : public userver::server::handlers::HttpHandlerBase {
public:
    static constexpr std::string_view kName = "handler-login-user";
    LoginUser(const userver::components::ComponentConfig& config,
              const userver::components::ComponentContext& component_context) 
        : HttpHandlerBase(config, component_context),
          pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}
    
    std::string HandleRequestThrow(
        const userver::server::http::HttpRequest& request,
        userver::server::request::RequestContext&
    ) const override {

        auto username_opt = request.GetFormDataArg("username");
        auto password_opt = request.GetFormDataArg("password");

        if (username_opt.value.empty() || password_opt.value.empty()) {
            auto& response = request.GetHttpResponse();
            response.SetStatus(userver::http::kBadRequest);
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error", 
                    "missing required parameters")
            );
        }

        auto username = username_opt.value;
        auto password = password_opt.value;

        auto hashed_password = userver::crypto::hash::Sha256(password);

        auto userResult = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kSlave,
            "SELECT * FROM exchange.users "
            "WHERE username = $1 ",
            username
        );

        if (userResult.IsEmpty()) {
            auto& response = request.GetHttpResponse();
            response.SetStatusNotFound();
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "user with this username in not registrated"
                )
            );
        }
        
        auto user = TUser(userResult.AsSingleRow<TUser>(userver::storages::postgres::kRowTag));
        
        if (hashed_password != user.hashed_password) {
            auto& response = request.GetHttpResponse();
            response.SetStatusNotFound();
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error",
                    "wrong password")
            );
        }

        auto result = pg_cluster_->Execute(
            userver::storages::postgres::ClusterHostType::kMaster,
            "INSERT INTO exchange.auth_sessions (user_id) VALUES($1) "
            "ON CONFLICT (user_id) DO UPDATE "
            "SET id = uuid_generate_v4() "
            "RETURNING id ",
            user.id
        );

        if (result.IsEmpty()) {
            auto& response = request.GetHttpResponse();
            response.SetStatusServiceUnavailable();
            return userver::formats::json::ToPrettyString(
                userver::formats::json::MakeObject("error", 
                    "failed to create session")
            );
        }

        userver::formats::json::ValueBuilder response;
        response["token"] = result.AsSingleRow<std::string>();

        return userver::formats::json::ToPrettyString(response.ExtractValue());
    }
private:
    userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace

void AppendLoginUser(userver::components::ComponentList& component_list) {
    component_list.Append<LoginUser>();
}

} // namespace NMatching