"""
Filename: fisheryates_shuffle.py

Implementation Details:
  - Implements the Fisher-Yates shuffle algorithm to randomly permute a list.
  - Uses an auxiliary 'used' list to track which elements have already been selected.
  - For each position in the shuffled list, randomly selects one of the remaining unselected elements.
  - The random number generator is seeded with a fixed value for reproducible results.

Functions with Parameters:
  fisheryates_shuffle(original)
    - original: Input list to be shuffled.
    - Returns: A new shuffled list.

Usage:
  - Fill the 'original' list with the data to be shuffled.
  - Call fisheryates_shuffle() to generate a shuffled version.
  - Example usage is provided in the main function, which shuffles a list of integers from 1 to N.

Notes:
  - The algorithm ensures each possible permutation is equally likely.
  - Suitable for small to moderate list sizes due to O(n^2) time complexity from repeated scanning.
  - Uses additional memory for the 'used' list and 'shuffled' list.

Time Complexity:
  - O(n^2): For each of the n positions, the code scans the 'used' list to count unused elements (O(n)),
    and then scans again to find the random-th unused element (O(n)). So, total O(n^2).

Space Complexity:
  - O(n): Uses additional lists of size n: 'shuffled' and 'used'.
"""

import random
import time


def fisheryates_shuffle(original, n):
    """
    Function: fisheryates_shuffle
    -----------------------------
    Implements the Fisher-Yates shuffle algorithm using an auxiliary 'used' list.

    Parameters:
      original: Input list to be shuffled.

    Returns:
      shuffled: A new list containing the shuffled elements.

    Notes:
      - Ensures each possible permutation is equally likely.
      - Time Complexity: O(n^2) due to repeated scanning of the 'used' list.
      - Space Complexity: O(n) for the 'used' list.
    """
    n = len(original)
    used = [False] * n  # List to mark whether an element has been used (True) or not (False)
    shuffled = []  # Output list for shuffled elements
    
    count = 0
    while count < n:
        # Count how many elements are still unused (O(n) per iteration)
        remaining = sum(1 for i in range(n) if not used[i])
        
        # Pick a random position among the remaining unused elements (1 to remaining)
        random_pos = random.randint(1, remaining)
        
        # Find the random_pos-th unused element in the original list (O(n) per iteration)
        index = -1
        for i in range(n):
            if not used[i]:
                random_pos -= 1
                if random_pos == 0:
                    index = i
                    break
        
        # Place the selected element into the shuffled list
        shuffled.append(original[index])
        # Mark this element as used
        used[index] = True
        count += 1
    
    return shuffled


def main():
    """
    Function: main
    --------------
    Demonstrates the usage of the fisheryates_shuffle function.

    Notes:
      - Seeds the random number generator for reproducible results.
      - Prints the list before and after shuffling.
    """
    n = 100  # Size of the list to shuffle
    
    # Fill the original list with values 1 to n
    original = list(range(1, n + 1))
    
    # Print the original list before shuffling
    print("Original list before shuffling:")
    for i, val in enumerate(original):
        print(f"{val} ", end="")
        if (i + 1) % 16 == 0:
            print()  # Newline every 16 elements
    print("\n")
    
    # Seed the random number generator with fixed value for reproducible results
    random.seed(time.time())  # Use this for different results each run
    
    # Shuffle the original list
    shuffled = fisheryates_shuffle(original)
    
    # Print the shuffled list after shuffling
    print("Shuffled list after Fisher-Yates shuffle:")
    for i, val in enumerate(shuffled):
        print(f"{val} ", end="")
        if (i + 1) % 16 == 0:
            print()  # Newline every 16 elements
    print("\n")

if __name__ == "__main__":
    main()