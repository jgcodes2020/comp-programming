import random
import sys

sys.set_int_max_str_digits(100_001)

cases = []

YUGE = 10**100_000

for i in range(1):
    a = random.randint(-YUGE, YUGE)
    b = random.randint(-YUGE, YUGE)
    s = a + b

    cases.append((a, b, s))

with open("test.in.txt", "w") as f:
    print(f"{len(cases)}", file=f)
    for (a, b, _) in cases:
        print(f"{a} {b}", file=f)

with open("test.out.txt", "w") as f:
    for (_, _, s) in cases:
        print(f"{s}", file=f)
