
/*
 * Filename: md2_main.c
 *
 * Implementation Details:
 *   - Implements a command-line interface for the MD2 hash algorithm.
 *   - Takes a string input from command line arguments and computes its MD2 hash digest.
 *   - Uses the MD2 algorithm implementation from md2.c following RFC 1115 specifications.
 *   - Produces a 128-bit (16-byte) hash digest displayed in hexadecimal format.
 *
 * Functions with Parameters:
 *   int main(int argc, char *argv[])
 *     - argc: Number of command line arguments.
 *     - argv[]: Array of command line argument strings.
 *     - Returns: 0 on success, 1 on error.
 *
 * Usage:
 *   - Compile: gcc -std=c89 -Wno-deprecated-non-prototype -o md2 md2_main.c or sh build_md2.sh
 *   - Execute: ./md2 "input_string"
 *   - Example: ./md2 "hello world"
 *   - Output: MD2 Digest: d9cce882ee690a5c1ce70beff3a78c77
 *
 * Notes:
 *   - Requires exactly one command line argument (the input string to hash).
 *   - The MD2 algorithm is considered cryptographically broken and should not be used for security purposes.
 *   - This implementation is for educational and historical reference purposes.
 *   - The digest is always 128 bits (32 hexadecimal characters) regardless of input size.
 *
 * Time Complexity:
 *   - O(n): Where n is the length of the input string, as each character is processed once.
 *
 * Space Complexity:
 *   - O(1): Uses fixed-size buffers and context structures regardless of input size.
 */

#include <stdio.h>
#include "md2.c"

/*
 * Function: main
 * --------------
 * Entry point for the MD2 hash calculator program.
 *
 * Parameters:
 *   argc: Number of command line arguments (should be 2: program name + input string).
 *   argv: Array of command line arguments where argv[1] is the string to hash.
 *
 * Returns:
 *   int: 0 on successful execution, 1 on error (incorrect usage).
 *
 * Notes:
 *   - Validates command line arguments before processing.
 *   - Initializes MD2 context, processes input string, and finalizes computation.
 *   - Outputs the resulting 128-bit digest in lowercase hexadecimal format.
 */
int main(int argc, char *argv[])
{
    // Validate command line arguments - exactly one input string required
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <input_string>\n", argv[0]);
        fprintf(stderr, "Example: %s \"hello world\"\n", argv[0]);
        return 1;
    }

    const char *input = argv[1];  // Input string to be hashed

    // MD2 context structure to maintain algorithm state
    struct MDCTX md;
    unsigned char digest[16];     // Final 128-bit (16-byte) hash digest
    int i;                        // Loop counter variable
    
    /**** MD2 Hash Computation Process ****/
    
    // Step 1: Initialize the MD2 context buffer
    // Sets all internal buffers (D, C) to zero and resets counters
    MDINIT(&md);
    
    // Step 2: Update the MD2 context with each character of the input string
    // Process input byte by byte, updating internal state after each character
    for (i = 0; input[i] != '\0'; i++) {
        MDUPDATE(&md, input[i]);
    }
    
    // Step 3: Finalize the MD2 computation
    // Applies padding and processes the checksum to complete the hash
    MDFINAL(&md);
    
    // Step 4: Extract the final digest from the MD2 context
    // The hash result is stored in md.D[0...15] after finalization
    for (i = 0; i < 16; i++)
        digest[i] = md.D[i];
    
    /**** Output the computed hash digest ****/
    
    // Print the resulting 128-bit digest in hexadecimal format
    printf("MD2 Digest: ");
    for (i = 0; i < 16; i++) {
        printf("%02x", digest[i]);  // Format each byte as 2-digit lowercase hex
    }
    printf("\n");
    
    return 0;  // Successful execution
}