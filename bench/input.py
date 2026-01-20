import random
from pathlib import Path

random.seed(114514)

input_dir = Path("bench/inputs")
input_dir.mkdir(parents=True, exist_ok=True)

def make_file(n, nums=None):
    if nums is None:
        nums = [random.randint(1, 9) for _ in range(n)]
    else:
        if len(nums) != n:
            raise ValueError(f"n ({n}) and len(nums) ({len(nums)}) mismatch")

    filename = f"{n}({','.join(map(str, nums))}).txt"

    with open(input_dir / filename, "w") as f:
        f.write(str(n) + "\n")
        f.write(" ".join(map(str, nums)) + "\n")


if __name__ == "__main__":
    make_file(4, [1, 2, 3, 4])
    make_file(5, [1, 2, 3, 4, 5])
    make_file(6, [1, 2, 3, 4, 5, 6])
    make_file(7, [1, 2, 3, 4, 5, 6, 7])
    make_file(8, [1, 2, 3, 4, 5, 6, 7, 8])
    
