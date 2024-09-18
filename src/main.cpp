#include <userver/clients/dns/component.hpp>
#include <userver/clients/http/component.hpp>
#include <userver/components/minimal_server_component_list.hpp>
#include <userver/server/handlers/ping.hpp>
#include <userver/server/handlers/tests_control.hpp>
#include <userver/testsuite/testsuite_support.hpp>
#include <userver/utils/daemon_run.hpp>
#include <userver/storages/postgres/component.hpp>

#include "handlers/delete-order/delete-order.hpp"
#include "handlers/get-order/get-order.hpp"
#include "handlers/get-orders/get-orders.hpp"
#include "handlers/add-order/add-order.hpp"
#include "handlers/register/register.hpp"
#include "handlers/login/login.hpp"
#include "handlers/balance/balance.hpp"
#include "handlers/get-deals/get-deals.hpp"

int main(int argc, char* argv[]) {
  auto component_list = userver::components::MinimalServerComponentList()
                            .Append<userver::server::handlers::Ping>()
                            .Append<userver::components::TestsuiteSupport>()
                            .Append<userver::components::HttpClient>()
                            .Append<userver::server::handlers::TestsControl>()
                            .Append<userver::clients::dns::Component>()
                            .Append<userver::components::Postgres>("postgres-db-1");

  NMatching::AppendRegisterUser(component_list);
  NMatching::AppendLoginUser(component_list);
  NMatching::AppendBalance(component_list);
  NMatching::AppendAddOrder(component_list);
  NMatching::AppendGetOrders(component_list);
  NMatching::AppendGetOrder(component_list);
  NMatching::AppendDeleteOrder(component_list);
  NMatching::AppendGetDeals(component_list);

  return userver::utils::DaemonMain(argc, argv, component_list);
}
