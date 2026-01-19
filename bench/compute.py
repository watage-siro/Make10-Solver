import sys
import statistics

args = list(map(float, sys.argv[1:]))

n = len(args) // 4
calls = args[0::4]
ops = args[1::4]
evals = args[2::4]
times = args[3::4]

print(",".join(map(str, calls + ops + evals + times + [
    statistics.mean(calls),
    statistics.mean(ops),
    statistics.mean(evals),
    statistics.mean(times),
    min(times),
    max(times),
])))
