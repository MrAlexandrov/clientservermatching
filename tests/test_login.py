import pytest
from .conftest import LOGIN_URL, create_multipart_data, create_headers_form_data


async def test_login_missing_username(service_client):
    username = None
    password = "login_missing_username"
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 400


async def test_login_missing_password(service_client):
    username = "login_missing_password"
    password = None 
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 400


async def test_login_missing_parametres(service_client):
    username = None
    password = None
    data = await create_multipart_data()
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 400


async def test_login_no_user(service_client):
    username = "login_no_user"
    password = "login_no_user"
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 404


async def test_login_wrong_password(service_client, register_user):
    username = "login_wrong_password"
    password = "login_wrong_password"
    await register_user(username, password)

    data = await create_multipart_data(username=username, password="wrong password")
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 404


async def test_login_success(service_client, register_user):
    username = "login_success"
    password = "login_success"
    await register_user(username=username, password=password)

    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(LOGIN_URL, data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert json_response["id"] is not None