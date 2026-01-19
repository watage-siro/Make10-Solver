import random
from pathlib import Path

Path("bench/inputs").mkdir(parents=True, exist_ok=True)

n = 6

for n in range(2, 9):
    for _ in range(10):
        nums = [random.randint(1, 9) for _ in range(n)]
        filename = f"{n}({','.join(map(str, nums))}).txt"
        with open(f"bench/inputs/{filename}", "w") as f:
            f.write(str(n) + "\n")
            f.write(" ".join(map(str, nums)))
