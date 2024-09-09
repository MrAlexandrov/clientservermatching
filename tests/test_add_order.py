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
    assert json_response["status"] != "cancelled"
