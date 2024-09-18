#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

constexpr char host[] = "http://localhost:8080";

enum class Command {
    Register,
    Login,
    Exit,
    ViewBalance,
    CreateOrder,
    DeleteOrder,
    ViewOrders,
    ViewDeals,
    Logout,
    Invalid
};

Command display_menu(const std::vector<std::pair<Command, std::string>>& options) {
    std::cout << "\nMenu:\n";
    int index = 1;
    for (const auto& option : options) {
        std::cout << index++ << ") " << option.second << '\n';
    }
    std::cout << "Select an option: ";

    int choice;
    std::cin >> choice;

    if (choice >= 1 && choice <= static_cast<int>(options.size())) {
        return options[choice - 1].first;
    } else {
        return Command::Invalid;
    }
}

int main() {
    std::optional<std::string> token;
    bool running = true;

    std::vector<std::pair<Command, std::string>> unauthenticated_menu = {
        {Command::Register, "Register"},
        {Command::Login, "Login"},
        {Command::Exit, "Exit"}
    };

    std::vector<std::pair<Command, std::string>> authenticated_menu = {
        {Command::ViewBalance, "View balance"},
        {Command::CreateOrder, "Create order"},
        {Command::DeleteOrder, "Delete order"},
        {Command::ViewOrders, "View orders"},
        {Command::ViewDeals, "View deals"},
        {Command::Logout, "Logout"}
    };

    while (running) {
        if (!token) {
            Command cmd = display_menu(unauthenticated_menu);

            switch (cmd) {
                case Command::Register: {
                    std::string username, password;
                    std::cout << "Enter username: ";
                    std::cin >> username;
                    std::cout << "Enter password: ";
                    std::cin >> password;

                    cpr::Response response = cpr::Post(
                        cpr::Url{std::string(host) + "/register"},
                        cpr::Multipart{
                            {"username", username},
                            {"password", password}
                        }
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        std::cout << "Registered successfully. Your ID: " << json_response["id"] << std::endl;
                    } else {
                        std::cout << "Registration failed: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }
                    break;
                }
                case Command::Login: {
                    std::string username, password;
                    std::cout << "Enter username: ";
                    std::cin >> username;
                    std::cout << "Enter password: ";
                    std::cin >> password;

                    cpr::Response response = cpr::Post(
                        cpr::Url{std::string(host) + "/login"},
                        cpr::Multipart{
                            {"username", username},
                            {"password", password}
                        }
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        if (json_response.contains("token") && !json_response["token"].is_null()) {
                            token = json_response["token"].get<std::string>();
                            std::cout << "Logged in successfully. Token stored." << std::endl;
                        } else {
                            std::cout << "Login failed: 'token' not found in response." << std::endl;
                            std::cout << "Server response: " << json_response.dump(4) << std::endl;
                        }
                    } else {
                        std::cout << "Login failed: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }
                    break;
                }
                case Command::Exit: {
                    std::cout << "Exiting...\n";
                    running = false;
                    break;
                }
                default: {
                    std::cout << "Unknown option. Please try again.\n";
                    break;
                }
            }
        } else {
            Command cmd = display_menu(authenticated_menu);

            switch (cmd) {
                case Command::ViewBalance: {
                    cpr::Response response = cpr::Get(
                        cpr::Url{std::string(host) + "/balance"},
                        cpr::Header{{"Authorization", *token}}
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        std::cout << "Your balance:\n";
                        std::cout << "USD: " << json_response["usd"] << "\n";
                        std::cout << "RUB: " << json_response["rub"] << "\n";
                    } else {
                        std::cout << "Failed to get balance: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }
                    break;
                }
                case Command::CreateOrder: {
                    std::string amount, price, order_type;
                    std::cout << "Enter amount (in USD): ";
                    std::cin >> amount;
                    std::cout << "Enter price (in RUB): ";
                    std::cin >> price;
                    std::cout << "Enter order type (buy/sell): ";
                    std::cin >> order_type;

                    json body;
                    body["amount"] = amount;
                    body["price"] = price;
                    body["order_type"] = order_type;

                    cpr::Response response = cpr::Post(
                        cpr::Url{std::string(host) + "/orders"},
                        cpr::Header{
                            {"Authorization", *token},
                            {"Content-Type", "application/json"}
                        },
                        cpr::Body{body.dump()}
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        std::cout << "Order created successfully. Order details:\n";
                        std::cout << json_response.dump(4) << std::endl;
                    } else {
                        std::cout << "Failed to create order: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }

                    break;
                }
                case Command::DeleteOrder: {
                    std::string order_id;
                    std::cout << "Enter order ID to delete: ";
                    std::cin >> order_id;

                    cpr::Response response = cpr::Delete(
                        cpr::Url{std::string(host) + "/orders/" + order_id},
                        cpr::Header{{"Authorization", *token}}
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        std::cout << "Order deleted successfully. Order ID: " << json_response["id"] << std::endl;
                    } else {
                        std::cout << "Failed to delete order: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }

                    break;
                }
                case Command::ViewOrders: {
                    cpr::Response response = cpr::Get(
                        cpr::Url{std::string(host) + "/orders"},
                        cpr::Header{{"Authorization", *token}}
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        std::cout << "Your active orders:\n";
                        std::cout << json_response.dump(4) << std::endl;
                    } else {
                        std::cout << "Failed to get orders: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }

                    break;
                }
                case Command::ViewDeals: {
                    cpr::Response response = cpr::Get(
                        cpr::Url{std::string(host) + "/deals"},
                        cpr::Header{{"Authorization", *token}}
                    );

                    if (response.status_code == 200) {
                        auto json_response = json::parse(response.text);
                        std::cout << "Your deals:\n";
                        std::cout << json_response.dump(4) << std::endl;
                    } else if (response.status_code == 404) {
                        std::cout << "Feature not implemented yet.\n";
                    } else {
                        std::cout << "Failed to get deals: " << response.status_code << " " << response.error.message << std::endl;
                        std::cout << "Server response: " << response.text << std::endl;
                    }

                    break;
                }
                case Command::Logout: {
                    token.reset();
                    std::cout << "Logged out successfully.\n";
                    break;
                }
                default: {
                    std::cout << "Unknown option. Please try again.\n";
                    break;
                }
            }
        }
    }

    return 0;
}
