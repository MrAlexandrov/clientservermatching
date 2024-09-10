import pathlib

import pytest

from testsuite.databases.pgsql import discover

import aiohttp

pytest_plugins = ['pytest_userver.plugins.postgresql']

REGISTER_URL = "/register"
LOGIN_URL = "/login"
GET_BALANCE_URL = "/balance"
ADD_ORDER_URL = "/orders"
GET_ORDERS_URL = "/orders"
DELETE_ORDER_URL = "/orders/{id}"
GET_ORDER_URL = "/orders/{id}"

@pytest.fixture(scope='session')
def service_source_dir():
    """Path to root directory service."""
    return pathlib.Path(__file__).parent.parent


@pytest.fixture(scope='session')
def initial_data_path(service_source_dir):
    """Path for find files with data"""
    return [
        service_source_dir / 'postgresql/data',
    ]


@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    """Create schemas databases for tests"""
    databases = discover.find_schemas(
        'matching',  # service name that goes to the DB connection
        [service_source_dir.joinpath('postgresql/schemas')],
    )
    return pgsql_local_create(list(databases.values()))


async def create_multipart_data(username=None, password=None):
    data = aiohttp.MultipartWriter('form-data')
    if username:
        payload = aiohttp.payload.StringPayload(username)
        payload.set_content_disposition('form-data', name='username')
        data.append_payload(payload)
    if password:
        payload = aiohttp.payload.StringPayload(password)
        payload.set_content_disposition('form-data', name='password')
        data.append_payload(payload)
    return data

def create_headers_form_data(data):
    return {
        'Content-Type': f'multipart/form-data; boundary={data.boundary}',
    }

def create_order_data(amount, price, type):
    order_data = {
        "amount": amount,
        "price": price,
        "order_type": type
    }
    return order_data


def create_headers_json(user_id):
    return {
        'Content-Type': 'application/json',
        'Authorization': user_id
    }


@pytest.fixture
async def register_user(service_client):
    async def register_(username, password):
        data = await create_multipart_data(username=username, password=password)
        headers = create_headers_form_data(data)
        response = await service_client.post(REGISTER_URL, data=data, headers=headers)
        assert response.status == 200
        json_response = response.json()
        assert "id" in json_response
        assert json_response["id"] is not None

        return json_response["id"]
    return register_

@pytest.fixture
async def login_user(service_client):
    async def login_(username, password):
        data = await create_multipart_data(username=username, password=password)
        headers = create_headers_form_data(data)
        response = await service_client.post(LOGIN_URL, data=data, headers=headers)
        assert response.status == 200
        json_response = response.json()
        assert "id" in json_response
        assert json_response["id"] is not None

        return json_response["id"]
    return login_


@pytest.fixture
async def add_order(service_client):
    async def add_(token, amount, price, order_type):


        order_data = create_order_data(amount, price, order_type)
        headers = create_headers_json(token)
        response = await service_client.post(ADD_ORDER_URL, json=order_data, headers=headers)
        
        # Проверка статуса ответа
        assert response.status == 200
        
        # Получение и проверка содержимого ответа
        json_response = response.json()  # нужно добавить await, так как это асинхронный запрос
        assert "id" in json_response
        assert json_response["amount"] == amount
        assert json_response["price"] == price
        assert json_response["order_type"] == order_type
        assert json_response["status"] in ["active", "partially_filled", "filled"]  # Ожидаемый статус заказа
        
        return json_response["id"]
    return add_
