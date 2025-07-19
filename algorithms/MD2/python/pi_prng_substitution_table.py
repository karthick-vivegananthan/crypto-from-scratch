"""
Filename: pi_prng_substitution_table.py

Implementation Details:
  This file implements the Durstenfeld shuffle algorithm (an optimized Fisher-Yates shuffle)
  for shuffling a substitution table of byte values. It uses a pseudo-random number
  generator (PRNG) based on the digits of Pi for deterministic shuffling.

Functions:
  - pi_prng(n, index)
      Parameters:
        n: Upper bound for the random number (exclusive).
        index: Current index in the Pi digits list (modified in-place).
      Description:
        Generates a pseudo-random number in the range [0, n) using digits of Pi.

  - swap(arr, i, j)
      Parameters:
        arr: The list containing elements to swap.
        i: Index of the first element to swap.
        j: Index of the second element to swap.
      Description:
        Swaps the values at indices i and j in the list.

  - durstenfeld_shuffle(arr)
      Parameters:
        arr: List to be shuffled in-place.
      Description:
        Performs an in-place shuffle of the list using the Durstenfeld algorithm and the Pi-based PRNG.

Usage:
  - Fill a list with byte values.
  - Print the original list.
  - Call durstenfeld_shuffle(arr) to shuffle the list in-place.
  - Print the shuffled list.

Notes:
  - The shuffle is unbiased and produces a uniform random permutation of the input list.
  - The algorithm modifies the input list directly.
  - The PRNG is deterministic and based on the digits of Pi.

Time Complexity:
  - O(n): The algorithm iterates through the list once, performing a constant-time swap for each element.

Space Complexity:
  - O(1): The algorithm uses only a constant amount of extra memory for temporary variables during swapping.
"""

# Array of the first 722 decimal digits of Pi, used as a deterministic pseudo-random source
# 722 because it is exactly used for generating pi_prng values for n ranging from 2 to 255
PI = [
    3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5, 8, 9, 7, 9, 3, 2, 3, 8, 4, 6, 2, 6, 4, 3, 3, 8, 3, 2, 7, 9, 5,
    0, 2, 8, 8, 4, 1, 9, 7, 1, 6, 9, 3, 9, 9, 3, 7, 5, 1, 0, 5, 8, 2, 0, 9, 7, 4, 9, 4, 4, 5, 9, 2,
    3, 0, 7, 8, 1, 6, 4, 0, 6, 2, 8, 6, 2, 0, 8, 9, 9, 8, 6, 2, 8, 0, 3, 4, 8, 2, 5, 3, 4, 2, 1, 1,
    7, 0, 6, 7, 9, 8, 2, 1, 4, 8, 0, 8, 6, 5, 1, 3, 2, 8, 2, 3, 0, 6, 6, 4, 7, 0, 9, 3, 8, 4, 4, 6,
    0, 9, 5, 5, 0, 5, 8, 2, 2, 3, 1, 7, 2, 5, 3, 5, 9, 4, 0, 8, 1, 2, 8, 4, 8, 1, 1, 1, 7, 4, 5, 0,
    2, 8, 4, 1, 0, 2, 7, 0, 1, 9, 3, 8, 5, 2, 1, 1, 0, 5, 5, 5, 9, 6, 4, 4, 6, 2, 2, 9, 4, 8, 9, 5,
    4, 9, 3, 0, 3, 8, 1, 9, 6, 4, 4, 2, 8, 8, 1, 0, 9, 7, 5, 6, 6, 5, 9, 3, 3, 4, 4, 6, 1, 2, 8, 4,
    7, 5, 6, 4, 8, 2, 3, 3, 7, 8, 6, 7, 8, 3, 1, 6, 5, 2, 7, 1, 2, 0, 1, 9, 0, 9, 1, 4, 5, 6, 4, 8,
    5, 6, 6, 9, 2, 3, 4, 6, 0, 3, 4, 8, 6, 1, 0, 4, 5, 4, 3, 2, 6, 6, 4, 8, 2, 1, 3, 3, 9, 3, 6, 0,
    7, 2, 6, 0, 2, 4, 9, 1, 4, 1, 2, 7, 3, 7, 2, 4, 5, 8, 7, 0, 0, 6, 6, 0, 6, 3, 1, 5, 5, 8, 8, 1,
    7, 4, 8, 8, 1, 5, 2, 0, 9, 2, 0, 9, 6, 2, 8, 2, 9, 2, 5, 4, 0, 9, 1, 7, 1, 5, 3, 6, 4, 3, 6, 7,
    8, 9, 2, 5, 9, 0, 3, 6, 0, 0, 1, 1, 3, 3, 0, 5, 3, 0, 5, 4, 8, 8, 2, 0, 4, 6, 6, 5, 2, 1, 3, 8,
    4, 1, 4, 6, 9, 5, 1, 9, 4, 1, 5, 1, 1, 6, 0, 9, 4, 3, 3, 0, 5, 7, 2, 7, 0, 3, 6, 5, 7, 5, 9, 5,
    9, 1, 9, 5, 3, 0, 9, 2, 1, 8, 6, 1, 1, 7, 3, 8, 1, 9, 3, 2, 6, 1, 1, 7, 9, 3, 1, 0, 5, 1, 1, 8,
    5, 4, 8, 0, 7, 4, 4, 6, 2, 3, 7, 9, 9, 6, 2, 7, 4, 9, 5, 6, 7, 3, 5, 1, 8, 8, 5, 7, 5, 2, 7, 2,
    4, 8, 9, 1, 2, 2, 7, 9, 3, 8, 1, 8, 3, 0, 1, 1, 9, 4, 9, 1, 2, 9, 8, 3, 3, 6, 7, 3, 3, 6, 2, 4,
    4, 0, 6, 5, 6, 6, 4, 3, 0, 8, 6, 0, 2, 1, 3, 9, 4, 9, 4, 6, 3, 9, 5, 2, 2, 4, 7, 3, 7, 1, 9, 0,
    7, 0, 2, 1, 7, 9, 8, 6, 0, 9, 4, 3, 7, 0, 2, 7, 7, 0, 5, 3, 9, 2, 1, 7, 1, 7, 6, 2, 9, 3, 1, 7,
    6, 7, 5, 2, 3, 8, 4, 6, 7, 4, 8, 1, 8, 4, 6, 7, 6, 6, 9, 4, 0, 5, 1, 3, 2, 0, 0, 0, 5, 6, 8, 1,
    2, 7, 1, 4, 5, 2, 6, 3, 5, 6, 0, 8, 2, 7, 7, 8, 5, 7, 7, 1, 3, 4, 2, 7, 5, 7, 7, 8, 9, 6, 0, 9,
    1, 7, 3, 6, 3, 7, 1, 7, 8, 7, 2, 1, 4, 6, 8, 4, 4, 0, 9, 0, 1, 2, 2, 4, 9, 5, 3, 4, 3, 0, 1, 4,
    6, 5, 4, 9, 5, 8, 5, 3, 7, 1, 0, 5, 0, 7, 9, 2, 2, 7, 9, 6, 8, 9, 2, 5, 8, 9, 2, 3, 5, 4, 2, 0,
    1, 9, 9, 5, 6, 1, 1, 2, 1, 2, 9, 0, 2, 1, 9, 6, 0, 8
]


def pi_prng(n, index):
    """
    Function: pi_prng
    -----------------
    Generates a pseudo-random number in the range (0, n) using digits of PI as a deterministic source.

    Parameters:
      n: The upper bound (exclusive) for the random number to be generated.
      index: List containing current index in the PI array (modified in-place).

    Returns:
      int: A pseudo-random integer in the range (0, n).

    Notes:
      - Uses the digits of PI as a deterministic random source.
      - Recursively retries if the generated value is not within the acceptable range to avoid bias.
      - Not cryptographically secure, but useful for deterministic shuffling or testing.
    """
    # Use 1, 2, or 3 digits of PI depending on the range needed
    # y is the range of values represented by the digits read from PI
    if n <= 10:
        # Use a single digit (0-9)
        x = PI[index[0]]
        index[0] += 1
        y = 10
    elif n <= 100:
        # Use two digits to form a number (00-99)
        x = (PI[index[0]] * 10) + PI[index[0] + 1]
        index[0] += 2
        y = 100
    else:
        # Use three digits to form a number (000-999)
        x = (PI[index[0]] * 100) + (PI[index[0] + 1] * 10) + PI[index[0] + 2]
        index[0] += 3
        y = 1000

    # Accept only values in the largest multiple of n less than y to avoid modulo bias
    if x < (n * (y // n)):
        # Return the value modulo n
        return x % n
    else:
        # If value is out of range, try again recursively
        return pi_prng(n, index)


def swap(arr, i, j):
    """
    Function: swap
    --------------
    Swaps the values at indices i and j in the list.

    Parameters:
      arr: The list containing elements to swap.
      i: Index of the first element to swap.
      j: Index of the second element to swap.

    Returns:
      None

    Notes:
      - Modifies the list in-place.
      - Used as a utility function for shuffling algorithms.
    """
    arr[i], arr[j] = arr[j], arr[i]


def durstenfeld_shuffle(arr):
    """
    Function: durstenfeld_shuffle
    -----------------------------
    Performs an in-place Durstenfeld shuffle (optimized Fisher-Yates shuffle) on the given list.

    Parameters:
      arr: List to be shuffled.

    Returns:
      None

    Notes:
      - Modifies the input list directly.
      - Produces a uniform random permutation using the pi_prng function.
      - Time Complexity: O(n).
      - Space Complexity: O(1).
    """
    n = len(arr)
    index = [0]  # Use list to make it mutable for pi_prng

    # Only difference is durstenfeld shuffle starts from the end of the array while here we start from the beginning
    for i in range(1, n):
        # Pick a random index from 0 to i
        j = pi_prng(i + 1, index)
        # Swap the current element with the randomly chosen element
        swap(arr, i, j)

def main():
    """
    Function: main
    --------------
    Entry point of the program. Demonstrates the creation and shuffling of a substitution table.

    Returns:
      None

    Notes:
      - Initializes a substitution table with values 0 to 255.
      - Prints the table before and after shuffling.
      - Uses durstenfeld_shuffle for randomization.
    """
    n = 256  # Size of the substitution table

    # Fill list with values from 0 to n-1
    substitution_table = list(range(n))

    # Print the original list before shuffling
    print("Original array before shuffling:")
    for i, val in enumerate(substitution_table):
        print(f"{val} ", end="")
        if (i + 1) % 16 == 0:
            print()  # Newline every 16 elements
    print("\n")

    # Shuffle the list in-place using Durstenfeld shuffle
    durstenfeld_shuffle(substitution_table)

    # Print the shuffled list after shuffling
    print("Shuffled array after Durstenfeld shuffle:")
    for i, val in enumerate(substitution_table):
        print(f"{val} ", end="")
        if (i + 1) % 16 == 0:
            print()  # Newline every 16 elements
    print("\n")

    return substitution_table

if __name__ == "__main__":    
    substitution_table = main()

"""
Substitution Table
-------------------
The result of this program is a static substitution table used in the MD2 algorithm.
It is a permutation of the numbers 0 to 255, shuffled using PI digits as random number generator in the Durstenfeld shuffle.
The table is used for byte substitution in the MD2 hashing process.

Expected output (deterministic based on PI digits):
static unsigned char PI_SUBST[256] = {
41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};
"""