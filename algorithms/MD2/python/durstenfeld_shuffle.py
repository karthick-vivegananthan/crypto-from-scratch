"""
Filename: durstenfeld_shuffle.py

Implementation Details:
  This file implements the Durstenfeld shuffle algorithm, which is an optimized version of the Fisher-Yates shuffle.
  The algorithm randomly permutes the elements of a list in-place using a single pass and constant extra space.

Functions:
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
        Performs an in-place shuffle of the list using the Durstenfeld algorithm.

Usage:
  - Fill a list with values.
  - Print the original list.
  - Seed the random number generator using random.seed() for different results on each run.
  - Call durstenfeld_shuffle(arr) to shuffle the list in-place.
  - Print the shuffled list.

Notes:
  - The shuffle is unbiased and produces a uniform random permutation of the input list.
  - The algorithm modifies the input list directly.
  - The random number generator should be seeded for different results on each execution.

Time Complexity:
  - O(n): The algorithm iterates through the list once, performing a constant-time swap for each element.

Space Complexity:
  - O(1): The algorithm uses only a constant amount of extra memory for temporary variables during swapping.
"""

import random
import time


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
      - Produces a uniform random permutation.
      - Time Complexity: O(n).
      - Space Complexity: O(1).
    """
    n = len(arr)
    # Iterate from the end of the list to the beginning
    for i in range(n - 1, 0, -1):
        # Pick a random index from 0 to i
        j = random.randint(0, i)
        # Swap the current element with the randomly chosen element
        swap(arr, i, j)


def main():
    """
    Function: main
    --------------
    Demonstrates the Durstenfeld shuffle by filling a list, printing it, shuffling it, and printing the result.

    Returns:
      None

    Notes:
      - Seeds the random number generator for reproducible results.
      - Prints the list before and after shuffling.
    """
    n = 100
    
    # Fill list with values from 1 to n
    arr = list(range(1, n + 1))

    # Print the original list before shuffling
    print("Original list before shuffling:")
    for i, val in enumerate(arr):
        print(f"{val} ", end="")
        if (i + 1) % 16 == 0:
            print()  # Newline every 16 elements
    print("\n")

    # Seed the random number generator for reproducible results
    random.seed(time.time())  # Use this for different results each run

    # Shuffle the list in-place
    durstenfeld_shuffle(arr)

    # Print the shuffled list after shuffling
    print("Shuffled list after Durstenfeld shuffle:")
    for i, val in enumerate(arr):
        print(f"{val} ", end="")
        if (i + 1) % 16 == 0:
            print()  # Newline every 16 elements
    print("\n")

if __name__ == "__main__":
    main()
