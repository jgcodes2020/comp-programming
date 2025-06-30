import random

cases = []

for i in range(50):
    a = random.randint(0, (1 << 512))
    b = random.randint(0, (1 << 512))
    s = a + b

    cases.append((a, b, s))

print("INPUTS:")
for (a, b, _) in cases:
    print(f"{a} {b}")
print("")
print("OUTPUTS:")
for (_, _, s) in cases:
    print(f"{s}")
