array_123 = [
    105, 115, 114, 118, 101, 97, 119, 104,
    111, 98, 112, 110, 117, 116, 102, 103
]

target = "giants"

# On veut trouver des lettres c telles que:
# array_123[ord(c) & 0xF] == target_char
valid_chars = []

for t in target:
    index = array_123.index(ord(t))
    candidates = []
    for c in range(ord('a'), ord('z') + 1):
        if (c & 0xF) == index:
            candidates.append(chr(c))
    valid_chars.append(candidates)

# Affichage des possibilités et d'une combinaison possible
from itertools import product

print("Lettres valides par position :")
for i, chars in enumerate(valid_chars):
    print(f"Position {i+1}: {chars}")

print("\nExemple de combinaison valide (tout en lettres):")
for comb in product(*valid_chars):
    print(''.join(comb))
    break  # On ne montre qu'une combinaison possible
