import statistics, sys

calls = list(map(float, sys.argv[1::4]))
ops = list(map(float, sys.argv[2::4]))
evals = list(map(float, sys.argv[3::4]))
times = list(map(float, sys.argv[4::4]))

print(",".join(map(str, calls + ops + evals + times + [
    statistics.mean(calls),
    statistics.mean(ops),
    statistics.mean(evals),
    statistics.mean(times),
    min(calls),
    max(calls),
    min(times),
    max(times),
])))
