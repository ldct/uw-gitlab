#!/usr/bin/env python3

FILENAME = "../data/Innerloop CCW E(50,5) 14.txt"

from collections import defaultdict

data = defaultdict(list)

def grabeq(s):
    [a, b] = s.split('=')
    return int(b, 10)

with open(FILENAME) as f:
    lines = f.readlines()[1:-14]
    for line in lines:
        [last_tripped, trip_time, _] = line.split('\t')
        last_tripped = grabeq(last_tripped)
        trip_time = grabeq(trip_time)
        data[last_tripped] += [trip_time]

z = [sum(x) for x in zip( data[8], data[9], data[10])]
for e in z:
    print(e)
