from pathlib import Path
import random

script_dir = Path(__file__).resolve().parent
output_file = script_dir/"orders.txt"

n = 10000
base_buy = 1000
base_sell = 2*base_buy
n_orders = 10
rand_orders_range = n_orders//2
rand_price_range_buy = int(base_buy * 0.1)
rand_price_range_sell = int(base_sell * 0.1)
with open(output_file, "w") as file:
    for i in range(n):
        file.write(f"buy limit {i} {(base_buy) - random.randint(-rand_price_range_buy, rand_price_range_buy)} {n_orders + random.randint(-rand_orders_range, rand_orders_range)}\n")
        file.write(f"sell limit {n + i} {(base_sell) + random.randint(-rand_price_range_sell, rand_price_range_sell)} {n_orders + random.randint(-rand_orders_range, rand_orders_range)}\n")

    for i in range(n//2):
        file.write(f"buy market {2*n + i} {(n_orders + random.randint(-rand_orders_range, rand_orders_range))}\n")
        file.write(f"sell market {3*n + i} {(n_orders + random.randint(-rand_orders_range, rand_orders_range))}\n")

    
