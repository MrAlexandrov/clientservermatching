import pytest
from .conftest import GET_ORDERS_URL, create_headers_json

import json

async def test_get_orders_success(service_client, register_user, login_user, add_order):
    username = "user1"
    password = "password1"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    await add_order(token, "10", "100", "buy")
    await add_order(token, "5", "50", "sell")

    headers = create_headers_json(token)
    response = await service_client.get(GET_ORDERS_URL, headers=headers)

    assert response.status == 200

    json_response = response.json()
    assert "items" in json_response
    assert len(json_response["items"]) == 2


    first_order = json_response["items"][0]
    assert first_order["amount"] == "10"
    assert first_order["price"] == "100"
    assert first_order["order_type"] == "buy"

    second_order = json_response["items"][1]
    assert second_order["amount"] == "5"
    assert second_order["price"] == "50"
    assert second_order["order_type"] == "sell"


async def test_get_orders_empty(service_client, register_user, login_user):
    username = "user2"
    password = "password2"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    headers = create_headers_json(token)
    response = await service_client.get(GET_ORDERS_URL, headers=headers)

    assert response.status == 200

    json_response = response.json()
    assert "items" in json_response
    assert len(json_response["items"]) == 0


async def test_get_orders_unauthorized(service_client):
    response = await service_client.get(GET_ORDERS_URL)

    assert response.status == 401

