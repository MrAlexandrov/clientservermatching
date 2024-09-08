#pragma once

#include <userver/server/http/http_request.hpp>
#include <userver/storages/postgres/cluster.hpp>

#include <optional>
#include <string_view>

#include "../../models/session.hpp"

namespace NMatching {

static constexpr std::string_view USER_TICKET_HEADER_NAME = "Authorization";

std::optional<TSession>
GetSessionInfo(userver::storages::postgres::ClusterPtr pg_cluster,
               const userver::server::http::HttpRequest &request);

} // namespace NMatching