import pytest
from .conftest import GET_DEALS_URL, create_headers_json

async def test_get_deals_success(service_client, register_user, login_user, add_order):
    username = "user1"
    password = "password1"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    order_id1 = await add_order(token, "10", "100", "buy")
    order_id2 = await add_order(token, "5", "50", "sell")

    username2 = "user2"
    password2 = "password2"

    user_id2 = await register_user(username2, password2)
    token2 = await login_user(username2, password2)

    await add_order(token2, "10", "100", "sell")
    await add_order(token2, "5", "50", "buy")

    headers = create_headers_json(token)
    response = await service_client.get(GET_DEALS_URL, headers=headers)

    assert response.status == 200

    json_response = response.json()
    assert "items" in json_response
    assert len(json_response["items"]) >= 2

    user_order_ids = [order_id1, order_id2]

    user_deals_found = False
    for deal in json_response["items"]:
        if deal["buy_order_id"] in user_order_ids or deal["sell_order_id"] in user_order_ids:
            user_deals_found = True
            break

    assert user_deals_found, "No deals found involving user's orders"



async def test_get_deals_empty(service_client, register_user, login_user):
    username = "user3"
    password = "password3"

    user_id = await register_user(username, password)
    token = await login_user(username, password)

    headers = create_headers_json(token)
    response = await service_client.get(GET_DEALS_URL, headers=headers)

    assert response.status == 200

    json_response = response.json()
    assert "items" in json_response
    assert len(json_response["items"]) == 0

async def test_get_deals_unauthorized(service_client):
    response = await service_client.get(GET_DEALS_URL)

    assert response.status == 401
