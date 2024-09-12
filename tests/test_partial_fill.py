import pytest

async def test_partial_fill(service_client, register_user, login_user, add_order, get_order, get_balance):
    buyer_id = await register_user("buyer", "password")
    buyer_token = await login_user("buyer", "password")

    seller_id = await register_user("seller", "password")
    seller_token = await login_user("seller", "password")

    seller_order_id = await add_order(seller_token, "50", "61", "sell")
    buyer_order_id = await add_order(buyer_token, "10", "62", "buy")

    seller_order = await get_order(seller_token, seller_order_id)
    assert seller_order["amount"] == "40"
    assert seller_order["status"] == "partially_filled"

    buyer_order = await get_order(buyer_token, buyer_order_id)
    assert buyer_order["amount"] == "0"
    assert buyer_order["status"] == "filled"

    buyer_balance = await get_balance(buyer_token)
    assert buyer_balance["usd"] == 10
    assert buyer_balance["rub"] == -620

    seller_balance = await get_balance(seller_token)
    assert seller_balance["usd"] == -10
    assert seller_balance["rub"] == 620



async def test_match_multiple_orders_reverse(service_client, register_user, login_user, add_order, get_order, get_balance):
    user1_id = await register_user("user1", "password")
    user1_token = await login_user("user1", "password")
    
    user2_id = await register_user("user2", "password")
    user2_token = await login_user("user2", "password")
    
    user3_id = await register_user("user3", "password")
    user3_token = await login_user("user3", "password")

    user1_order_id = await add_order(user1_token, "15", "62", "buy")
    user2_order_id = await add_order(user2_token, "25", "63", "buy")
    user3_order_id = await add_order(user3_token, "20", "64", "buy")

    seller_id = await register_user("seller", "password")
    seller_token = await login_user("seller", "password")

    seller_order_id = await add_order(seller_token, "50", "61", "sell")

    user3_order = await get_order(user3_token, user3_order_id)
    assert user3_order["amount"] == "0"
    assert user3_order["status"] == "filled"

    user2_order = await get_order(user2_token, user2_order_id)
    assert user2_order["amount"] == "0"
    assert user2_order["status"] == "filled"

    user1_order = await get_order(user1_token, user1_order_id)
    assert user1_order["amount"] == "10"
    assert user1_order["status"] == "partially_filled"

    seller_order = await get_order(seller_token, seller_order_id)
    assert seller_order["amount"] == "0"
    assert seller_order["status"] == "filled"

    user1_balance = await get_balance(user1_token)
    assert user1_balance["usd"] == 5
    assert user1_balance["rub"] == -310

    user2_balance = await get_balance(user2_token)
    assert user2_balance["usd"] == 25
    assert user2_balance["rub"] == -1575

    user3_balance = await get_balance(user3_token)
    assert user3_balance["usd"] == 20
    assert user3_balance["rub"] == -1280

    seller_balance = await get_balance(seller_token)
    assert seller_balance["usd"] == -50
    assert seller_balance["rub"] == 3165



async def test_match_orders_example(service_client, register_user, login_user, add_order, get_order, get_balance):
    user1_id = await register_user("user1", "password")
    user1_token = await login_user("user1", "password")
    
    user2_id = await register_user("user2", "password")
    user2_token = await login_user("user2", "password")
    
    user3_id = await register_user("user3", "password")
    user3_token = await login_user("user3", "password")

    user1_order_id = await add_order(user1_token, "10", "62", "buy")
    
    user2_order_id = await add_order(user2_token, "20", "63", "buy")
    
    user3_order_id = await add_order(user3_token, "50", "61", "sell")

    user2_order = await get_order(user2_token, user2_order_id)
    assert user2_order["amount"] == "0"
    assert user2_order["status"] == "filled"
    
    user1_order = await get_order(user1_token, user1_order_id)
    assert user1_order["amount"] == "0"
    assert user1_order["status"] == "filled"
    
    user3_order = await get_order(user3_token, user3_order_id)
    assert user3_order["amount"] == "20"
    assert user3_order["status"] == "partially_filled"

    user1_balance = await get_balance(user1_token)
    assert user1_balance["usd"] == 10
    assert user1_balance["rub"] == -620
    
    user2_balance = await get_balance(user2_token)
    assert user2_balance["usd"] == 20
    assert user2_balance["rub"] == -1260
    
    user3_balance = await get_balance(user3_token)
    assert user3_balance["usd"] == -30
    assert user3_balance["rub"] == 1880