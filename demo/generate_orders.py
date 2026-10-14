from pathlib import Path
import random

script_dir = Path(__file__).resolve().parent
output_file = script_dir/"orders.txt"

n = 10000
base_buy = 95
base_sell = 105
base_orders = 50
base_peak = 10

# 50% Limit, 20% Market, 10% Iceberg, 20% Stop Losses
with open(output_file, "w") as file:
    for i in range(n):
        rand_state = random.randint(1, 100)
        rand_price = random.randint(-7, 7)
        rand_quant = random.randint(-25, 25)
        rand_peak = random.randint(-5, 5)
        rand_stop = random.randint(1,4)

        if rand_state < 25:
            file.write(f"buy limit {i} {base_buy + rand_price} {base_orders + rand_quant}\n")
        elif 25 <= rand_state < 50:
            file.write(f"sell limit {i} {base_sell + rand_price} {base_orders + rand_quant}\n")
        elif 50 <= rand_state < 60:
            file.write(f"buy market {i} {(base_orders + rand_quant)//4}\n")
        elif 60 <= rand_state < 70:
            file.write(f"sell market {i} {(base_orders + rand_quant)//4}\n")
        elif 70 <= rand_state < 75:
            file.write(f"buy iceberg {i} {base_buy + rand_price} {base_orders + rand_quant} {base_peak + rand_peak}\n")
        elif 75 <= rand_state < 80:
            file.write(f"sell iceberg {i} {base_sell + rand_price} {base_orders + rand_quant} {base_peak + rand_peak}\n")
        elif 80 <= rand_state < 90:
            file.write(f"buy stop_loss {i} {base_sell + rand_stop } {(base_orders + rand_quant)//4}\n")
        else:
            file.write(f"sell stop_loss {i} {base_buy - rand_stop} {(base_orders + rand_quant)//4}\n")


    
