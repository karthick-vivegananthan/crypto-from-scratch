/*
 * Filename: fisheryates_shuffle.c
 *
 * Implementation Details:
 *   - Implements the Fisher-Yates shuffle algorithm to randomly permute an array.
 *   - Uses an auxiliary 'used' array to track which elements have already been selected.
 *   - For each position in the shuffled array, randomly selects one of the remaining unselected elements.
 *   - The random number generator is seeded with the current time to ensure different results on each run.
 *
 * Functions with Parameters:
 *   void fisheryates_shuffle(int original[], int shuffled[], int n)
 *     - original[]: Input array to be shuffled.
 *     - shuffled[]: Output array where the shuffled result is stored.
 *     - n: Number of elements in the arrays.
 *
 * Usage:
 *   - Fill the 'original' array with the data to be shuffled.
 *   - Call fisheryates_shuffle() to generate a shuffled version in the 'shuffled' array.
 *   - Example usage is provided in the main function, which shuffles an array of integers from 1 to N.
 *
 * Notes:
 *   - The algorithm ensures each possible permutation is equally likely.
 *   - Suitable for small to moderate array sizes due to O(n^2) time complexity from repeated scanning.
 *   - Uses three arrays of size n: 'original', 'shuffled', and 'used'.
 *
 * Time Complexity:
 *   - O(n^2): For each of the n positions, the code scans the 'used' array to count unused elements (O(n)),
 *     and then scans again to find the random-th unused element (O(n)). So, total O(n^2).
 *
 * Space Complexity:
 *   - O(n): Uses three arrays of size n: 'original', 'shuffled', and 'used'.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Function: fisheryates_shuffle
 * -----------------------------
 * Implements the Fisher-Yates shuffle algorithm using an auxiliary 'used' array.
 *
 * Parameters:
 *   original[]: Input array to be shuffled.
 *   shuffled[]: Output array where the shuffled result is stored.
 *   n: Number of elements in the arrays.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Ensures each possible permutation is equally likely.
 *   - Time Complexity: O(n^2) due to repeated scanning of the 'used' array.
 *   - Space Complexity: O(n) for the 'used' array.
 */
void fisheryates_shuffle(int original[], int shuffled[], int n)
{
    int used[n]; // Array to mark whether an element has been used (1) or not (0)
    int i, count, remaining, index, random;

    // Initialize all elements in 'used' to 0 (none used yet)
    for (i = 0; i < n; i++)
    {
        used[i] = 0;
    }

    count = 0;
    while (count < n)
    {
        // Count how many elements are still unused (O(n) per iteration)
        remaining = 0;
        for (i = 0; i < n; i++)
        {
            if (!used[i])
                remaining++;
        }

        // Pick a random position among the remaining unused elements (1 to remaining)
        random = rand() % (remaining + 1);

        // Find the random-th unused element in the original array (O(n) per iteration)
        index = -1;
        for (i = 0; i < n; i++)
        {
            if (!used[i])
            {
                random--;
                if (random == 0)
                {
                    index = i;
                    break;
                }
            }
        }

        // Place the selected element into the shuffled array
        shuffled[count++] = original[index];
        // Mark this element as used
        used[index] = 1;
    }
}

/*
 * Function: main
 * --------------
 * Demonstrates the usage of the fisheryates_shuffle function.
 *
 * Returns:
 *   0 on successful execution.
 *
 * Notes:
 *   - Seeds the random number generator for different results on each run.
 *   - Prints the array before and after shuffling.
 */
int main()
{
    int i, n = 100; // Size of the array to shuffle
    int original[n], shuffled[n];

    // Fill the original array with values 1 to n
    for (i = 0; i < n; i++)
    {
        original[i] = i + 1;
    }

    // Print the original array before shuffling
    printf("Original array before shuffling: \n");
    for (i = 0; i < n; i++)
    {
        printf("%d ", original[i]);
        if ((i + 1) % 16 == 0)
        {
            // Print '\b' to remove trailing space and \n for newline
            printf("\b\n");
        }
    }
    printf("\n");

    // Seed the random number generator with current time for randomness
    //srand(time(NULL));
    srand(1);

    // Shuffle the original array into the shuffled array
    fisheryates_shuffle(original, shuffled, n);

    // Print the shuffled array after shuffling
    printf("Shuffled array after Fisher-Yates shuffle: \n");
    for (i = 0; i < n; i++)
    {
        printf("%d ", shuffled[i]);
        if ((i + 1) % 16 == 0)
        {
            // Print '\b' to remove trailing space and \n for newline
            printf("\b\n");
        }
    }
    printf("\n");

    return 0;
}