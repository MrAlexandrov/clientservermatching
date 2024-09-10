import pytest
from .conftest import GET_ORDERS_URL, create_headers_json

import json

# Тест на успешное получение заявок
async def test_get_orders_success(service_client, register_user, login_user, add_order):
    username = "user1"
    password = "password1"

    # Регистрация и логин пользователя
    user_id = await register_user(username, password)
    token = await login_user(username, password)

    # Добавление нескольких заявок
    await add_order(token, "10", "100", "buy")
    #
    headers = create_headers_json(token)
    response = await service_client.get(GET_ORDERS_URL, headers=headers)
    json_response = response.json()
    with open('log.log', 'w+') as f:
        f.write(json.dumps(json_response, indent=4))
    #
    await add_order(token, "5", "50", "sell")
    #
    headers = create_headers_json(token)
    response = await service_client.get(GET_ORDERS_URL, headers=headers)
    json_response = response.json()
    with open('log.log', 'w+') as f:
        f.write(json.dumps(json_response, indent=4))
    #

    # Получение списка заявок
    headers = create_headers_json(token)
    response = await service_client.get(GET_ORDERS_URL, headers=headers)
    
    # Проверка успешного статуса
    assert response.status == 200
    
    # Проверка содержимого ответа
    json_response = response.json()
    assert "items" in json_response
    assert len(json_response["items"]) == 2  # Мы добавили 2 заявки


    # with open("log.log", 'w') as f:
    #     f.print(json_response)    

    # Проверка первой заявки
    first_order = json_response["items"][0]
    assert first_order["amount"] == "10"
    assert first_order["price"] == "100"
    assert first_order["order_type"] == "buy"
    
    # Проверка второй заявки
    second_order = json_response["items"][1]
    assert second_order["amount"] == "5"
    assert second_order["price"] == "50"
    assert second_order["order_type"] == "sell"


# Тест на получение пустого списка заявок
async def test_get_orders_empty(service_client, register_user, login_user):
    username = "user2"
    password = "password2"

    # Регистрация и логин пользователя
    user_id = await register_user(username, password)
    token = await login_user(username, password)

    # Попытка получить заявки, не добавив ни одной
    headers = create_headers_json(token)
    response = await service_client.get(GET_ORDERS_URL, headers=headers)
    
    # Проверка успешного статуса
    assert response.status == 200
    
    # Проверка пустого списка
    json_response = response.json()
    assert "items" in json_response
    assert len(json_response["items"]) == 0  # Ожидаем пустой список

# Тест на неавторизованный доступ
async def test_get_orders_unauthorized(service_client):
    # Попытка получить заявки без авторизации
    response = await service_client.get(GET_ORDERS_URL)
    
    # Проверка статуса 401
    assert response.status == 401

