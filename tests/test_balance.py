import pytest
from .conftest import GET_BALANCE_URL, create_headers_json


async def test_get_balance_unauthorized(service_client):
    response = await service_client.get(GET_BALANCE_URL)
    assert response.status == 401


async def test_get_balance_success(service_client, register_user, login_user):
    username = "username"
    password = "password"
    await register_user(username, password)
    user_id = await login_user(username, password)

    headers = create_headers_json(user_id)
    response = await service_client.get(GET_BALANCE_URL, headers=headers)

    assert response.status == 200

    json_response = response.json()

    assert "usd" in json_response
    assert json_response["usd"] == "0"

    assert "rub" in json_response
    assert json_response["rub"] == "0"