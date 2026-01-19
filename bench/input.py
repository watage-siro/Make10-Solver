import random, os

random.seed(1234)
os.makedirs("bench/inputs", exist_ok=True)

CASE_NUM = 20  # ここを変えると生成数が変わる
N = 6          # 数字の個数

for idx in range(1, CASE_NUM + 1):
    nums = [random.randint(1, 9) for _ in range(N)]
    with open(f"bench/inputs/input{idx}.txt", "w") as f:
        f.write(" ".join(map(str, nums)))
