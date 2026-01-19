import random, os

random.seed(114514)
os.makedirs("bench/inputs", exist_ok=True)

for idx in range(1, 11):
    nums = [random.randint(1, 9) for _ in range(6)]
    with open(f"bench/inputs/input{idx}.txt", "w") as f:
        f.write(" ".join(map(str, nums)))
