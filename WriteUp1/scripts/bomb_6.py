import itertools

# Index 1 à 6 => valeurs des nœuds
node_values = {
    1: 253,
    2: 725,
    3: 301,
    4: 997,
    5: 212,
    6: 432
}

# Vérifie que les valeurs sont en ordre strictement décroissant
def is_descending(values):
    return all(x > y for x, y in zip(values, values[1:]))

# Tester toutes les permutations possibles
for perm in itertools.permutations(range(1, 7)):
    values = [node_values[i] for i in perm]
    if is_descending(values):
        print("✅ Valid input for phase 6:")
        print("Input:", ' '.join(map(str, perm)))
        print("Values:", values)
        break
