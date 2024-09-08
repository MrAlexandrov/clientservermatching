import pytest
from .conftest import REGISTER_URL, create_multipart_data, create_headers_form_data  


async def test_register_missing_username(service_client):
    username = None
    password = "register_missing_username"
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 400


async def test_register_missing_password(service_client):
    username = "register_missing_password"
    password = None
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 400


async def test_register_missing_parametres(service_client):
    username = None
    password = None
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 400


async def test_register_same_username(service_client):
    username = "register_same_username"
    password1 = "first_password"
    password2 = "second_password"
    data = await create_multipart_data(username=username, password=password1)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response

    data = await create_multipart_data(username=username, password=password2)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 409



async def test_register_success(service_client):
    username = "register_success"
    password = "register_success"
    data = await create_multipart_data(username=username, password=password)
    headers = create_headers_form_data(data)
    response = await service_client.post(REGISTER_URL, data=data, headers=headers)
    assert response.status == 200
    json_response = response.json()
    assert "id" in json_response
    assert json_response["id"] is not None