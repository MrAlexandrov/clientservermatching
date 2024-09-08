#include "register.hpp"

#include <userver/server/handlers/http_handler_base.hpp>
#include <userver/storages/postgres/component.hpp>
#include <userver/storages/postgres/cluster.hpp>
#include <userver/components/component.hpp>
#include <userver/crypto/crypto.hpp>

namespace NMatching {

namespace {

class RegisterUser final : public userver::server::handlers::HttpHandlerBase {
public:
  static constexpr std::string_view kName = "handler-register-user";

  RegisterUser(const userver::components::ComponentConfig &config,
               const userver::components::ComponentContext &component_context)
      : HttpHandlerBase(config, component_context),
        pg_cluster_(
            component_context
                .FindComponent<userver::components::Postgres>("postgres-db-1")
                .GetCluster()) {}

  std::string HandleRequestThrow(
      const userver::server::http::HttpRequest &request,
      userver::server::request::RequestContext &) const override {

    auto username_opt = request.GetFormDataArg("username");
    auto password_opt = request.GetFormDataArg("password");

    if (username_opt.value.empty() || password_opt.value.empty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kBadRequest);
      return userver::formats::json::ToPrettyString(
          userver::formats::json::MakeObject("error",
                                             "missing required parameters")
      );
    }

    auto username = username_opt.value;
    auto password = password_opt.value;

    auto hashed_password = userver::crypto::hash::Sha256(password);

    auto exist_username = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kSlave,
        "SELECT username "
        "FROM exchange.users "
        "WHERE username = $1",
        username);

    if (!exist_username.IsEmpty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(userver::server::http::HttpStatus::kConflict);
      return userver::formats::json::ToPrettyString(
          userver::formats::json::MakeObject("error", 
                                             "user with this username already exists")
      );
    }

    auto result = pg_cluster_->Execute(
        userver::storages::postgres::ClusterHostType::kMaster,
        "WITH ins AS ( "
        "INSERT INTO exchange.users(username, hashed_password) "
        "VALUES ($1, $2) "
        "ON CONFLICT DO NOTHING "
        "RETURNING users.id "
        ") "
        "SELECT id FROM ins "
        "UNION ALL "
        "SELECT id FROM exchange.users WHERE username = $1 ",
        username, hashed_password);

    if (result.IsEmpty()) {
      auto &response = request.GetHttpResponse();
      response.SetStatus(
          userver::server::http::HttpStatus::kInternalServerError);
      return userver::formats::json::ToPrettyString(
          userver::formats::json::MakeObject("error", "error creating user")
      );
    }

    userver::formats::json::ValueBuilder response;
    response["id"] = result.AsSingleRow<std::string>();

    return userver::formats::json::ToPrettyString(response.ExtractValue());
  }

private:
  userver::storages::postgres::ClusterPtr pg_cluster_;
};

} // namespace

void AppendRegisterUser(userver::components::ComponentList &component_list) {
  component_list.Append<RegisterUser>();
}

} // namespace NMatching
