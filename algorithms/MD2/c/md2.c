/*
 * Filename: md2.c
 *
 * Implementation Details:
 *   This file implements the MD2 (Message Digest 2) cryptographic hash algorithm as specified in RFC 1319.
 *   MD2 produces a 128-bit (16-byte) hash digest from input data of arbitrary length. The algorithm uses
 *   a substitution table based on π (pi) and processes data in 16-byte blocks with padding and checksum.
 *
 * Functions:
 *   - void MD2Init(MD2_CTX *context)
 *       Parameters:
 *         *context: Pointer to MD2 context structure to initialize.
 *       Description:
 *         Initializes the MD2 context structure, setting all internal buffers to zero.
 *
 *   - void MD2Update(MD2_CTX *context, POINTER input, unsigned int input_length)
 *       Parameters:
 *         *context: Pointer to MD2 context structure.
 *         input: Pointer to input data to be processed.
 *         input_length: Length of input data in bytes.
 *       Description:
 *         Processes input data in chunks, updating the internal state and applying transformations.
 *
 *   - void MD2Final(POINTER digest, MD2_CTX *context)
 *       Parameters:
 *         digest: Pointer to buffer where final 16-byte hash will be stored.
 *         *context: Pointer to MD2 context structure.
 *       Description:
 *         Finalizes the hash computation by applying padding, processing checksum, and generating final digest.
 *
 *   - static void MD2Transform(POINTER state, POINTER checksum, POINTER block)
 *       Parameters:
 *         state: Pointer to 16-byte state buffer.
 *         checksum: Pointer to 16-byte checksum buffer.
 *         block: Pointer to 16-byte input block to process.
 *       Description:
 *         Performs the core MD2 transformation on a single 16-byte block using substitution and XOR operations.
 *
 * Usage:
 *   - Initialize MD2 context with MD2Init().
 *   - Process input data with one or more calls to MD2Update().
 *   - Finalize computation and retrieve digest with MD2Final().
 *   - The main function demonstrates usage by computing MD2 hash of command-line input.
 *
 * Notes:
 *   - MD2 is considered cryptographically weak and should not be used for security-critical applications.
 *   - The algorithm uses a pre-computed substitution table based on π for randomization.
 *   - Input is padded to ensure length is multiple of 16 bytes before final processing.
 *   - A checksum is computed and appended before final transformation.
 *
 * Time Complexity:
 *   - O(n): Processing time is linear with input size, where n is the number of input bytes.
 *
 * Space Complexity:
 *   - O(1): Uses constant space for internal buffers (48 bytes for state, checksum, and temporary data).
 */

#include <string.h>
#include <stdio.h>

/* POINTER defines a generic pointer type for unsigned char arrays */
typedef unsigned char *POINTER;

/* MD2 context structure to maintain algorithm state during computation */
typedef struct
{
    unsigned char state[16];    /* 16-byte internal state buffer (D) */
    unsigned char checksum[16]; /* 16-byte checksum buffer (C) */
    unsigned int count;         /* number of bytes processed, modulo 16 */
    unsigned char buffer[16];   /* 16-byte input buffer for incomplete blocks */
} MD2_CTX;

/*
 * Pre-computed substitution table S based on the digits of π (pi).
 * This 256-byte table provides pseudo-random permutation for MD2 transformations.
 * Each value from 0-255 appears exactly once, making it a permutation table.
 */
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
    31, 26, 219, 153, 141, 51, 159, 17, 131, 20};

/*
 * Padding bytes used to ensure input length is multiple of 16.
 * PADDING[i] contains i bytes, each with value i.
 * For example: PADDING[3] = "\x03\x03\x03" (3 bytes of value 3)
 */
static unsigned char *PADDING[] = {
    (unsigned char *)"",
    (unsigned char *)"\x01",
    (unsigned char *)"\x02\x02",
    (unsigned char *)"\x03\x03\x03",
    (unsigned char *)"\x04\x04\x04\x04",
    (unsigned char *)"\x05\x05\x05\x05\x05",
    (unsigned char *)"\x06\x06\x06\x06\x06\x06",
    (unsigned char *)"\x07\x07\x07\x07\x07\x07\x07",
    (unsigned char *)"\x08\x08\x08\x08\x08\x08\x08\x08",
    (unsigned char *)"\x09\x09\x09\x09\x09\x09\x09\x09\x09",
    (unsigned char *)"\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A",
    (unsigned char *)"\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B",
    (unsigned char *)"\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C",
    (unsigned char *)"\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D",
    (unsigned char *)"\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E",
    (unsigned char *)"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F",
    (unsigned char *)"\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10"};

/*
 * Function: MD2Transform
 * ----------------------
 * Performs the core MD2 transformation on a single 16-byte block.
 * This function implements the heart of the MD2 algorithm, performing
 * substitution and XOR operations through 18 rounds of encryption.
 *
 * Parameters:
 *   state: Pointer to 16-byte state buffer (D) to be updated.
 *   checksum: Pointer to 16-byte checksum buffer (C) to be updated.
 *   block: Pointer to 16-byte input block to process.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Creates 48-byte encryption block: [state][block][state XOR block]
 *   - Performs 18 rounds of substitution using PI_SUBST table
 *   - Updates both state and checksum based on input block
 *   - Zeros sensitive information after processing
 */
/* MD2 basic transformation. Transforms state and updates checksum
 based on block.
 */
static void MD2Transform(POINTER state, POINTER checksum, POINTER block)
{
    unsigned int i, j, t;
    unsigned char x[48]; /* 48-byte encryption block */

    /* Form encryption block from state, block, state ^ block.
     * x[0..15] = state, x[16..31] = block, x[32..47] = state XOR block
     */
    memcpy(x, state, 16);      /* Copy state to first 16 bytes */
    memcpy(x + 16, block, 16); /* Copy block to next 16 bytes */
    for (i = 0; i < 16; i++)
        x[i + 32] = state[i] ^ block[i]; /* XOR for last 16 bytes */

    /* Encrypt block (18 rounds).
     * Each round applies substitution table to all 48 bytes
     */
    t = 0;
    for (i = 0; i < 18; i++)
    {
        for (j = 0; j < 48; j++)
            t = x[j] ^= PI_SUBST[t]; /* Apply substitution and update running value */
        t = (t + i) & 0xFF;          /* Add round number and keep in byte range */
    }

    /* Save new state from first 16 bytes of encrypted block */
    memcpy(state, x, 16);

    /* Update checksum using substitution table and input block.
     * Each checksum byte is XORed with substitution of (block byte XOR previous checksum byte)
     */
    t = checksum[15]; /* Start with last checksum byte */
    for (i = 0; i < 16; i++)
        t = checksum[i] ^= PI_SUBST[block[i] ^ t];

    /* Zeroize sensitive information to prevent memory attacks */
    memset(x, 0, sizeof(x));
}

/*
 * Function: MD2Init
 * -----------------
 * Initializes the MD2 context structure for a new hash computation.
 * Sets all internal buffers (state, checksum, buffer) to zero and resets counters.
 *
 * Parameters:
 *   context: Pointer to MD2_CTX structure to initialize.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Must be called before any MD2Update() calls
 *   - Prepares context for processing input data
 *   - All internal state is cleared to ensure clean start
 */
/* MD2 initialization. Begins an MD2 operation, writing a new context.
 */
void MD2Init(MD2_CTX *context)
{
    context->count = 0;                                      /* Reset byte counter */
    memset(context->state, 0, sizeof(context->state));       /* Clear 16-byte state buffer */
    memset(context->checksum, 0, sizeof(context->checksum)); /* Clear 16-byte checksum buffer */
}

/*
 * Function: MD2Update
 * -------------------
 * Processes input data and updates the MD2 context.
 * Handles data buffering and calls MD2Transform when complete 16-byte blocks are available.
 *
 * Parameters:
 *   context: Pointer to MD2_CTX structure maintaining hash state.
 *   input: Pointer to input data to be processed.
 *   input_length: Number of bytes in input data.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Can be called multiple times to process data in chunks
 *   - Automatically buffers incomplete blocks for next call
 *   - Processes complete 16-byte blocks immediately via MD2Transform
 *   - Updates byte counter modulo 16 for padding calculation
 */

/* MD2 block update operation. Continues an MD2 message-digest
 operation, processing another message block, and updating the
 context.
 */
void MD2Update(MD2_CTX *context, POINTER input, unsigned int input_length)
{
    unsigned int i, index, remaining_length;

    /* Update number of bytes mod 16 for padding calculation */
    index = context->count;
    context->count = (index + input_length) & 0xF; /* Keep only low 4 bits (mod 16) */
    remaining_length = 16 - index;                 /* Bytes needed to fill current buffer */

    /* Transform as many complete 16-byte blocks as possible */
    if (input_length >= remaining_length)
    {
        /* Fill the buffer and process it */
        memcpy(&context->buffer[index], input, remaining_length);
        MD2Transform(context->state, context->checksum, context->buffer);

        /* Process additional complete 16-byte blocks directly from input */
        for (i = remaining_length; i + 15 < input_length; i += 16)
            MD2Transform(context->state, context->checksum, &input[i]);
        index = 0; /* Buffer is now empty */
    }
    else
    {
        i = 0; /* No complete blocks to process */
    }

    /* Buffer remaining input bytes for next call or final processing */
    memcpy(&context->buffer[index], &input[i], input_length - i);
}

/*
 * Function: MD2Final
 * ------------------
 * Finalizes the MD2 hash computation and produces the final digest.
 * Applies padding, processes the checksum, and generates the 16-byte hash result.
 *
 * Parameters:
 *   digest: Pointer to 16-byte buffer where final hash will be stored.
 *   context: Pointer to MD2_CTX structure with current hash state.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Adds padding to make total length multiple of 16 bytes
 *   - Appends checksum as final 16-byte block
 *   - Copies final state to digest buffer
 *   - Zeros context to prevent information leakage
 *   - After calling this function, context is no longer usable
 */
/* MD2 finalization. Ends an MD2 message-digest operation, writing the
 message digest and zeroizing the context.
 */
void MD2Final(POINTER digest, MD2_CTX *context)
{
    unsigned int index, padding_length;

    /* Pad out to multiple of 16 bytes.
     * Padding byte value equals number of padding bytes needed.
     */
    index = context->count;      /* Current position in buffer (0-15) */
    padding_length = 16 - index; /* Bytes needed to reach multiple of 16 */
    MD2Update(context, PADDING[padding_length], padding_length);

    /* Extend with checksum - process checksum as final 16-byte block */
    MD2Update(context, context->checksum, 16);

    /* Store final state as the computed digest */
    memcpy(digest, context->state, 16);

    /* Zeroize sensitive information to prevent memory attacks */
    memset(context, 0, sizeof(*context));
}

/*
 * Function: main
 * --------------
 * Demonstrates MD2 hash computation by processing command-line input.
 * Validates arguments, computes MD2 hash, and displays result in hexadecimal format.
 *
 * Parameters:
 *   argc: Number of command-line arguments.
 *   argv: Array of command-line argument strings.
 *
 * Returns:
 *   int: 0 on success, 1 on error (invalid arguments).
 *
 * Notes:
 *   - Requires exactly one input string as command-line argument
 *   - Outputs 32-character hexadecimal representation of 128-bit hash
 *   - Demonstrates proper MD2 usage: Init -> Update -> Final
 */

int main(int argc, char *argv[])
{
    // Validate command line arguments - exactly one input string required
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <input_string>\n", argv[0]);
        fprintf(stderr, "Example: %s \"hello world\"\n", argv[0]);
        return 1;
    }

    const char *input = argv[1];               // Input string to be hashed
    MD2_CTX context;                           // MD2 context structure to maintain algorithm state
    unsigned char digest[16];                  // Final 128-bit (16-byte) hash digest
    unsigned int input_length = strlen(input); // Length of input string
    int i;                                     // Loop counter variable

    /**** MD2 Hash Computation Process ****/

    // Step 1: Initialize the MD2 context buffer
    // Sets all internal buffers (state, checksum, buffer) to zero and resets counters
    MD2Init(&context);

    // Step 2: Update the MD2 context with the input string
    // Process input data, handling buffering and block transformations automatically
    MD2Update(&context, (POINTER)input, input_length);

    // Step 3: Finalize the MD2 computation
    // Applies padding, processes the checksum, and generates the final digest
    MD2Final(digest, &context);

    /**** Output the computed hash digest ****/

    // Print the resulting 128-bit digest in hexadecimal format (32 hex characters)
    printf("MD2 Digest: ");
    for (i = 0; i < 16; i++)
    {
        printf("%02x", digest[i]); // Format each byte as 2-digit lowercase hex
    }
    printf("\n");

    return 0; // Successful execution
}