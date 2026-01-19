import statistics, sys

times = list(map(float, sys.argv[1::4]))
calls = list(map(int, sys.argv[1::4]))
ops = list(map(int, sys.argv[2::4]))
evals = list(map(int, sys.argv[3::4]))


print(",".join(map(str, times + calls + ops + evals + [
    statistics.mean(times),
    statistics.mean(calls),
    statistics.mean(ops),
    statistics.mean(evals),
    min(times),
    max(times),
    min(calls),
    max(calls),
])))
