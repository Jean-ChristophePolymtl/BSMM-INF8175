#!/usr/bin/env python3

import re
import sys
from pathlib import Path

import numpy as np


if len(sys.argv) != 2:
    print("Usage: python3 generate_demo_input.py <demo_test_vector.h>")
    sys.exit(1)


header_path = Path(sys.argv[1])
text = header_path.read_text()


def extract_float_array(name):
    pattern = rf"{name}\s*\[\s*128\s*\]\s*=\s*\{{(.*?)\}};"

    match = re.search(pattern, text, re.DOTALL)

    if match is None:
        raise RuntimeError(f"Array not found: {name}")

    values_text = match.group(1)

    values = [
        float(value.rstrip("f"))
        for value in values_text.split(",")
        if value.strip()
    ]

    if len(values) != 128:
        raise RuntimeError(
            f"{name}: expected 128 values, found {len(values)}."
        )

    return np.asarray(values, dtype=np.float64)


# Extract the input and reference output.
input_values = extract_float_array("test_input_flat")
reference_values = extract_float_array("ref_output_flat")

# Convert the input from floating point to signed Q22.
q22_values = np.rint(input_values * (1 << 22))

q22_values = np.clip(
    q22_values,
    -(1 << 31),
    (1 << 31) - 1
).astype("<i4")

# Store 128 signed int32 values.
q22_values.tofile("input.bin")

# Store the reference output as 128 float32 values.
reference_values.astype("<f4").tofile("ref_output.bin")

print("Generated input.bin")
print(f"  Values: {q22_values.size}")
print(f"  Size:   {q22_values.nbytes} bytes")
print()

print("Generated ref_output.bin")
print(f"  Values: {reference_values.size}")
print(f"  Size:   {reference_values.astype('<f4').nbytes} bytes")