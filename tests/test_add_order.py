import pytest
from .conftest import ADD_ORDER_URL, create_order_data, create_headers_json

async def test_add_order_unauthorized(service_client):
    response = await service_client.post(ADD_ORDER_URL)
    assert response.status == 401


async def test_add_order_success(service_client, register_user, login_user):
    username = "username"
    password = "password"

    amount = "10"
    price = "10"
    type = "buy"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    headers = create_headers_json(token)
    order_data = create_order_data(amount, price, type)

    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data)
    assert response.status == 200

    json_response = response.json()

    assert "id" in json_response
    assert "user_id" in json_response
    assert "order_type" in json_response
    assert "price" in json_response
    assert "amount" in json_response
    assert "status" in json_response
    assert "created_ts" in json_response

    assert json_response["user_id"] == user_id
    assert json_response["order_type"] == type
    assert json_response["price"] == price
    assert json_response["amount"] == amount
    assert json_response["status"] in ["active", "partially_filled", "filled"]


async def test_add_order_invalid_data(service_client, register_user, login_user):
    username = "username"
    password = "password"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    headers = create_headers_json(token)
    
    order_data = create_order_data("-10", "10", "buy")
    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data)
    assert response.status == 400

    order_data = create_order_data("0", "10", "buy")
    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data)
    assert response.status == 400

    order_data = create_order_data("10", "10", "invalid_type")
    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data)
    assert response.status == 400


# async def test_add_partial_order_filled(service_client, register_user, login_user):
#     username = "username"
#     password = "password"

#     user_id = await register_user(username, password)
#     token = await login_user(username, password)

#     headers = create_headers_json(token)
    
#     # Создание исходного ордера на покупку
#     initial_order_data = create_order_data("20", "10", "buy")
#     response = await service_client.post(ADD_ORDER_URL, headers=headers, json=initial_order_data)
#     assert response.status == 200

#     json_response = response.json()
#     assert json_response["status"] == "active"

#     # Добавление частично выполняемого ордера на продажу
#     partial_order_data = create_order_data("10", "10", "sell")
#     response = await service_client.post(ADD_ORDER_URL, headers=headers, json=partial_order_data)
#     assert response.status == 200

#     # Проверьте, что статус ордера обновлен на "partially_filled"
#     json_response = response.json()
#     assert json_response["status"] == "partially_filled"


async def test_add_multiple_orders_same_user(service_client, register_user, login_user):
    username = "username"
    password = "password"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    headers = create_headers_json(token)

    order_data_1 = create_order_data("10", "10", "buy")
    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data_1)
    assert response.status == 200

    order_data_2 = create_order_data("5", "8", "sell")
    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data_2)
    assert response.status == 200


async def test_add_order_missing_fields(service_client, register_user, login_user):
    username = "username"
    password = "password"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    headers = create_headers_json(token)

    order_data = {
        "amount": "10",
        "order_type": "buy"
    }
    response = await service_client.post(ADD_ORDER_URL, headers=headers, json=order_data)
    assert response.status == 400
