/*
 * Filename: durstenfeld_shuffle.c
 *
 * Implementation Details:
 *   This file implements the Durstenfeld shuffle algorithm, which is an optimized version of the Fisher-Yates shuffle.
 *   The algorithm randomly permutes the elements of an integer array in-place using a single pass and constant extra space.
 *
 * Functions:
 *   - void swap(int *a, int *b, int *tmp)
 *       Parameters:
 *         *a: Pointer to the first integer to swap.
 *         *b: Pointer to the second integer to swap.
 *         *tmp: Pointer to a temporary integer for swapping.
 *       Description:
 *         Swaps the values of the two integers pointed to by a and b.
 *
 *   - void durstenfeld_shuffle(int arr[], int n)
 *       Parameters:
 *         arr[]: Pointer to the integer array to be shuffled.
 *         n:   Number of elements in the array.
 *       Description:
 *         Performs an in-place shuffle of the array using the Durstenfeld algorithm.
 *
 * Usage:
 *   - Fill an integer array with values.
 *   - Print the original array.
 *   - Seed the random number generator using srand(time(NULL)) for different results on each run.
 *   - Call durstenfeld_shuffle(arr, n) to shuffle the array in-place.
 *   - Print the shuffled array.
 *
 * Notes:
 *   - The shuffle is unbiased and produces a uniform random permutation of the input array.
 *   - The algorithm modifies the input array directly.
 *   - The random number generator should be seeded for different results on each execution.
 *
 * Time Complexity:
 *   - O(n): The algorithm iterates through the array once, performing a constant-time swap for each element.
 *
 * Space Complexity:
 *   - O(1): The algorithm uses only a constant amount of extra memory for temporary variables during swapping.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * Function: swap
 * --------------
 * Swaps the values of two integers pointed to by a and b.
 *
 * Parameters:
 *   a: Pointer to the first integer.
 *   b: Pointer to the second integer.
 *   tmp: Pointer to a temporary integer for swapping.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Modifies the values at the provided memory addresses.
 *   - Used as a utility function for shuffling algorithms.
 */
void swap(int *a, int *b, int *tmp)
{
    tmp = *a;
    *a = *b;
    *b = tmp;
}

/*
 * Function: durstenfeld_shuffle
 * -----------------------------
 * Performs an in-place Durstenfeld shuffle (optimized Fisher-Yates shuffle) on the given array.
 *
 * Parameters:
 *   arr[]: Pointer to the array to be shuffled.
 *   n: Number of elements in the array.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Modifies the input array directly.
 *   - Produces a uniform random permutation.
 *   - Time Complexity: O(n).
 *   - Space Complexity: O(1).
 */
void durstenfeld_shuffle(int arr[], int n)
{
    int i, j, tmp;
    // Iterate from the end of the array to the beginning
    for (i = n - 1; i > 0; i--)
    {
        // Pick a random index from 0 to i
        j = rand() % (i + 1);
        // Swap the current element with the randomly chosen element
        swap(&arr[i], &arr[j], &tmp);
    }
}

/*
 * Function: main
 * --------------
 * Demonstrates the Durstenfeld shuffle by filling an array, printing it, shuffling it, and printing the result.
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
    int i, n = 100;
    int arr[n];

    // Fill array with values from 1 to n
    for (i = 0; i < n; i++)
    {
        arr[i] = i + 1;
    }

    // Print the original array before shuffling
    printf("Original array before shuffling: \n");
    for (i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
        if ((i + 1) % 16 == 0)
        {
            // Print '\b' to remove trailing space and \n for newline
            printf("\b\n");
        }
    }
    printf("\n");

    // Seed the random number generator for different results each run
    //srand(time(NULL));
    srand(1);

    // Shuffle the array in-place
    durstenfeld_shuffle(arr, n);

    // Print the shuffled array after shuffling
    printf("Shuffled array after Durstenfeld shuffle: \n");
    for (i = 0; i < n; i++)
    {
        printf("%d ", arr[i]);
        if ((i + 1) % 16 == 0)
        {
            // Print '\b' to remove trailing space and \n for newline
            printf("\b\n");
        }
    }
    printf("\n");

    return 0;
}