/*
 * Filename: md4.c
 *
 * Implementation Details:
 *   This file implements the MD4 (Message Digest 4) cryptographic hash algorithm as specified in RFC 1320.
 *   MD4 produces a 128-bit (16-byte) hash digest from input data of arbitrary length. The algorithm uses
 *   a series of logical functions (F, G, H) and processes data in 64-byte blocks with padding.
 *
 * Functions:
 *   - void MD4Init(MD4_CTX *context)
 *       Parameters:
 *         *context: Pointer to MD4 context structure to initialize.
 *       Description:
 *         Initializes the MD4 context structure, setting state to magic constants and resetting counters.
 *
 *   - void MD4Update(MD4_CTX *context, unsigned char *input, unsigned int input_length)
 *       Parameters:
 *         *context: Pointer to MD4 context structure.
 *         input: Pointer to input data to be processed.
 *         input_length: Length of input data in bytes.
 *       Description:
 *         Processes input data in chunks, updating the internal state and applying transformations.
 *
 *   - void MD4Final(POINTER digest, MD4_CTX *context)
 *       Parameters:
 *         digest: Pointer to buffer where final 16-byte hash will be stored.
 *         *context: Pointer to MD4 context structure.
 *       Description:
 *         Finalizes the hash computation by applying padding, appending length, and generating final digest.
 *
 *   - static void MD4Transform(UINT4 *state, POINTER block)
 *       Parameters:
 *         state: Pointer to 4-element state array (A, B, C, D).
 *         block: Pointer to 64-byte input block to process.
 *       Description:
 *         Performs the core MD4 transformation on a single 64-byte block using three rounds of operations.
 *
 * Usage:
 *   - Initialize MD4 context with MD4Init().
 *   - Process input data with one or more calls to MD4Update().
 *   - Finalize computation and retrieve digest with MD4Final().
 *   - The main function demonstrates usage by computing MD4 hash of command-line input.
 *
 * Notes:
 *   - MD4 is considered cryptographically weak and should not be used for security-critical applications.
 *   - The algorithm uses three rounds with different logical functions (F, G, H).
 *   - Input is padded to ensure length is congruent to 56 bytes modulo 64 before final processing.
 *   - Original message length is appended as 64-bit value before final transformation.
 *
 * Time Complexity:
 *   - O(n): Processing time is linear with input size, where n is the number of input bytes.
 *
 * Space Complexity:
 *   - O(1): Uses constant space for internal buffers (88 bytes for state, count, and buffer).
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>

/* POINTER defines a generic pointer type for byte arrays */
typedef unsigned char *POINTER;
/* UINT4 defines a four-byte word (32-bit unsigned integer) */
typedef uint32_t UINT4;

typedef struct
{
    UINT4 state[4];           /* state variables (A, B, C, D) - the hash state */
    UINT4 count[2];           /* number of bits processed, modulo 2^64 (lsb first) */
    unsigned char buffer[64]; /* input buffer for incomplete blocks */
} MD4_CTX;

#define I0 0x67452301 /* Initial hash values for MD4 buffer (little-endian) */
#define I1 0xefcdab89
#define I2 0x98badcfe
#define I3 0x10325476

#define C2 0x5a827999 /* Round 2 constant (square root of 2) */
#define C3 0x6ed9eba1 /* Round 3 constant (square root of 3) */

/* Shift amounts for MD4Transform rounds.
 * Round 1: S11=3, S12=7, S13=11, S14=19
 * Round 2: S21=3, S22=5, S23=9, S24=13  
 * Round 3: S31=3, S32=9, S33=11, S34=15
 */
#define S11 3
#define S12 7
#define S13 11
#define S14 19

#define S21 3
#define S22 5
#define S23 9
#define S24 13

#define S31 3
#define S32 9
#define S33 11
#define S34 15

/* Padding array for MD4 finalization.
 * 64 bytes total: first byte is 0x80 (binary 10000000), rest are zeros.
 * The 0x80 byte represents the mandatory '1' bit followed by zeros.
 * Used to pad input to ensure length ≡ 56 (mod 64) before appending length.
 */
static unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/* MD4 auxiliary functions F, G, and H for the three rounds.
 * F: Selection function - if x then y else z
 * G: Majority function - at least two of x, y, z are true  
 * H: Parity function - XOR of x, y, z
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))

/* ROTATE_LEFT performs left circular rotation of x by n bit positions.
 * Implements: (x << n) | (x >> (32-n)) for 32-bit words.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

/* Round transformation macros for MD4 algorithm.
 * FF: Round 1 operations using function F
 * GG: Round 2 operations using function G with constant C2
 * HH: Round 3 operations using function H with constant C3
 * Each macro updates variable 'a' and applies left rotation by 's' positions.
 */
#define FF(a, b, c, d, x, s)           \
    {                                  \
        (a) += F((b), (c), (d)) + (x); \
        (a) = ROTATE_LEFT((a), (s));   \
    }
#define GG(a, b, c, d, x, s)                       \
    {                                              \
        (a) += G((b), (c), (d)) + (x) + (UINT4)C2; \
        (a) = ROTATE_LEFT((a), (s));               \
    }
#define HH(a, b, c, d, x, s)                       \
    {                                              \
        (a) += H((b), (c), (d)) + (x) + (UINT4)C3; \
        (a) = ROTATE_LEFT((a), (s));               \
    }

/*
 * Function: Encode
 * ----------------
 * Converts 32-bit words to byte array in little-endian format.
 * Used to convert internal state to final digest output.
 *
 * Parameters:
 *   output: Pointer to byte array where encoded data will be stored.
 *   input: Pointer to array of 32-bit words to encode.
 *   length: Number of bytes to encode (must be multiple of 4).
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Assumes little-endian byte ordering
 *   - Each 32-bit word produces 4 bytes in output
 *   - Length parameter specifies output bytes, not input words
 */
static void Encode(POINTER output, UINT4 *input, unsigned int length)
{
    unsigned int i, j;
    // Convert each 32-bit word to 4 bytes in little-endian format
    for (i = 0, j = 0; j < length; i++, j += 4)
    {
        output[j] = (unsigned char)(input[i] & 0xff);           // LSB (bits 0-7)
        output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);  // bits 8-15
        output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff); // bits 16-23  
        output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff); // MSB (bits 24-31)
    }
}
/*
 * Function: Decode
 * ----------------
 * Converts byte array to 32-bit words in little-endian format.
 * Used to convert input blocks into format suitable for transformation.
 *
 * Parameters:
 *   output: Pointer to array of 32-bit words where decoded data will be stored.
 *   input: Pointer to byte array to decode.
 *   length: Number of bytes to decode (must be multiple of 4).
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Assumes little-endian byte ordering
 *   - Each 4 bytes in input produces one 32-bit word in output
 *   - Length parameter specifies input bytes, not output words
 */
static void Decode(UINT4 *output, POINTER input, unsigned int length)
{
    unsigned int i, j;
    // Convert each group of 4 bytes to a 32-bit word in little-endian format
    for (i = 0, j = 0; j < length; i++, j += 4)
        output[i] = ((UINT4)input[j]) | (((UINT4)input[j + 1]) << 8) |
                    (((UINT4)input[j + 2]) << 16) | (((UINT4)input[j + 3]) << 24);
}

/*
 * Function: MD4Transform
 * ----------------------
 * Performs the core MD4 transformation on a single 64-byte block.
 * This function implements the heart of the MD4 algorithm, performing
 * three rounds of operations using logical functions F, G, and H.
 *
 * Parameters:
 *   state: Pointer to 4-element state array (A, B, C, D) to be updated.
 *   block: Pointer to 64-byte input block to process.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Decodes 64-byte block into sixteen 32-bit words
 *   - Performs 48 operations in three rounds (16 operations each)
 *   - Round 1: Uses F function with shift amounts S11-S14
 *   - Round 2: Uses G function with constant C2 and shift amounts S21-S24
 *   - Round 3: Uses H function with constant C3 and shift amounts S31-S34
 *   - Updates state by adding original values to transformed values
 *   - Zeros sensitive information after processing
 */
static void MD4Transform(UINT4 *state, POINTER block)
{
    UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
    
    // Decode 64-byte block into sixteen 32-bit words
    Decode(x, block, 64);
    
    /* Round 1: Apply function F with message words in order (0,1,2,...,15) */
    FF(a, b, c, d, x[0], S11);  /* 1 */
    FF(d, a, b, c, x[1], S12);  /* 2 */
    FF(c, d, a, b, x[2], S13);  /* 3 */
    FF(b, c, d, a, x[3], S14);  /* 4 */
    FF(a, b, c, d, x[4], S11);  /* 5 */
    FF(d, a, b, c, x[5], S12);  /* 6 */
    FF(c, d, a, b, x[6], S13);  /* 7 */
    FF(b, c, d, a, x[7], S14);  /* 8 */
    FF(a, b, c, d, x[8], S11);  /* 9 */
    FF(d, a, b, c, x[9], S12);  /* 10 */
    FF(c, d, a, b, x[10], S13); /* 11 */
    FF(b, c, d, a, x[11], S14); /* 12 */
    FF(a, b, c, d, x[12], S11); /* 13 */
    FF(d, a, b, c, x[13], S12); /* 14 */
    FF(c, d, a, b, x[14], S13); /* 15 */
    FF(b, c, d, a, x[15], S14); /* 16 */
    
    /* Round 2: Apply function G with message words in pattern (0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15) */
    GG(a, b, c, d, x[0], S21);  /* 17 */
    GG(d, a, b, c, x[4], S22);  /* 18 */
    GG(c, d, a, b, x[8], S23);  /* 19 */
    GG(b, c, d, a, x[12], S24); /* 20 */
    GG(a, b, c, d, x[1], S21);  /* 21 */
    GG(d, a, b, c, x[5], S22);  /* 22 */
    GG(c, d, a, b, x[9], S23);  /* 23 */
    GG(b, c, d, a, x[13], S24); /* 24 */
    GG(a, b, c, d, x[2], S21);  /* 25 */
    GG(d, a, b, c, x[6], S22);  /* 26 */
    GG(c, d, a, b, x[10], S23); /* 27 */
    GG(b, c, d, a, x[14], S24); /* 28 */
    GG(a, b, c, d, x[3], S21);  /* 29 */
    GG(d, a, b, c, x[7], S22);  /* 30 */
    GG(c, d, a, b, x[11], S23); /* 31 */
    GG(b, c, d, a, x[15], S24); /* 32 */
    
    /* Round 3: Apply function H with message words in pattern (0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15) */
    HH(a, b, c, d, x[0], S31);  /* 33 */
    HH(d, a, b, c, x[8], S32);  /* 34 */
    HH(c, d, a, b, x[4], S33);  /* 35 */
    HH(b, c, d, a, x[12], S34); /* 36 */
    HH(a, b, c, d, x[2], S31);  /* 37 */
    HH(d, a, b, c, x[10], S32); /* 38 */
    HH(c, d, a, b, x[6], S33);  /* 39 */
    HH(b, c, d, a, x[14], S34); /* 40 */
    HH(a, b, c, d, x[1], S31);  /* 41 */
    HH(d, a, b, c, x[9], S32);  /* 42 */
    HH(c, d, a, b, x[5], S33);  /* 43 */
    HH(b, c, d, a, x[13], S34); /* 44 */
    HH(a, b, c, d, x[3], S31);  /* 45 */
    HH(d, a, b, c, x[11], S32); /* 46 */
    HH(c, d, a, b, x[7], S33);  /* 47 */
    HH(b, c, d, a, x[15], S34); /* 48 */

    // Add the transformed values back to the state (prevents length extension attacks)
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    
    /* Zeroize sensitive information from stack. */
    memset((POINTER)x, 0, sizeof(x));
}

/*
 * Function: MD4Init
 * -----------------
 * Initializes an MD4 context structure for hash computation.
 * Sets up the initial state values and resets bit counters.
 *
 * Parameters:
 *   context: Pointer to MD4_CTX structure to initialize.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Resets bit counters to zero
 *   - Loads magic initialization constants (I0, I1, I2, I3) into state
 *   - These constants are derived from fractional parts of square roots
 *   - Must be called before any MD4Update operations
 */
void MD4Init(MD4_CTX *context)
{
    // Initialize bit counters to zero (no bits processed yet)
    context->count[0] = context->count[1] = 0;
    
    /* Load magic initialization constants into state variables.
     * These hex values represent the fractional parts of square roots.
     */
    context->state[0] = I0;  // A = 0x67452301
    context->state[1] = I1;  // B = 0xEFCDAB89
    context->state[2] = I2;  // C = 0x98BADCFE
    context->state[3] = I3;  // D = 0x10325476
}

/*
 * Function: MD4Update
 * -------------------
 * Processes input data through the MD4 algorithm.
 * Handles buffering and calls MD4Transform for complete 64-byte blocks.
 *
 * Parameters:
 *   context: Pointer to MD4_CTX structure maintaining algorithm state.
 *   input: Pointer to input data to be processed.
 *   input_length: Number of bytes in input data.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Updates bit count (handles overflow from low to high word)
 *   - Processes complete 64-byte blocks immediately
 *   - Buffers remaining bytes for future processing
 *   - Can be called multiple times to process data incrementally
 *   - Automatically handles block boundaries and buffering
 */
void MD4Update(MD4_CTX *context, unsigned char *input, unsigned int input_length)
{
    unsigned int i, index, remaining_length;
    
    /* Compute current position in buffer (number of bytes mod 64) */
    index = (unsigned int)((context->count[0] >> 3) & 0x3F);
    
    /* Update total bit count, handling potential overflow from low to high word */
    if ((context->count[0] += ((UINT4)input_length << 3)) < ((UINT4)input_length << 3))
        context->count[1]++;  // Carry overflow to high word
    context->count[1] += ((UINT4)input_length >> 29);  // Add high-order bits
    
    remaining_length = 64 - index;  // Space remaining in current buffer
    
    /* Transform complete 64-byte blocks as many times as possible. */
    if (input_length >= remaining_length)
    {
        // Fill current buffer and process it
        memcpy((POINTER)&context->buffer[index], (POINTER)input, remaining_length);
        MD4Transform(context->state, context->buffer);
        
        // Process remaining complete 64-byte blocks directly from input
        for (i = remaining_length; i + 63 < input_length; i += 64)
            MD4Transform(context->state, &input[i]);
        index = 0;  // Reset buffer position
    }
    else
    {
        i = 0;  // No complete blocks to process
    }
    
    /* Buffer any remaining input bytes for future processing */
    memcpy((POINTER)&context->buffer[index], (POINTER)&input[i], input_length - i);
}
/*
 * Function: MD4Final
 * ------------------
 * Finalizes the MD4 hash computation and produces the final digest.
 * Applies padding, appends message length, and performs final transformation.
 *
 * Parameters:
 *   digest: Pointer to 16-byte buffer where final hash will be stored.
 *   context: Pointer to MD4_CTX structure containing current state.
 *
 * Returns:
 *   void
 *
 * Notes:
 *   - Saves current bit count before padding
 *   - Pads message to 56 bytes modulo 64 (leaving 8 bytes for length)
 *   - Appends original message length as 64-bit little-endian value
 *   - Encodes final state into digest buffer
 *   - Zeros context structure to clear sensitive information
 *   - After this call, context cannot be reused without re-initialization
 */
void MD4Final(POINTER digest, MD4_CTX *context)
{
    unsigned char bits[8];
    unsigned int index, padding_length;
    
    /* Save number of bits processed (before padding) for later appending */
    Encode(bits, context->count, 8);
    
    /* Apply padding to ensure message length ≡ 56 (mod 64).
     * This leaves exactly 8 bytes for the length field.
     */
    index = (unsigned int)((context->count[0] >> 3) & 0x3f);  // Current buffer position
    padding_length = (index < 56) ? (56 - index) : (120 - index);  // Pad to next 56 mod 64
    MD4Update(context, PADDING, padding_length);
    
    /* Append original message length in bits as 64-bit little-endian value */
    MD4Update(context, bits, 8);
    
    /* Store final state as digest in little-endian byte order */
    Encode(digest, context->state, 16);
    
    /* Zeroize sensitive information to prevent memory disclosure attacks. */
    memset((POINTER)context, 0, sizeof(*context));
}

/*
 * Function: main
 * --------------
 * Demonstrates MD4 hash computation by processing command-line input.
 * Validates arguments, computes MD4 hash, and displays result in hexadecimal format.
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
 *   - Demonstrates proper MD4 usage: Init -> Update -> Final
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
    MD4_CTX context;                           // MD4 context structure to maintain algorithm state
    unsigned char digest[16];                  // Final 128-bit (16-byte) hash digest
    unsigned int input_length = strlen(input); // Length of input string
    unsigned int i;                            // Loop counter variable

    /**** MD4 Hash Computation Process ****/

    // Step 1: Initialize the MD4 context buffer
    // Sets initial state values to magic constants and resets bit counters
    MD4Init(&context);

    // Step 2: Update the MD4 context with the input string
    // Process input data, handling buffering and block transformations automatically
    MD4Update(&context, (POINTER)input, input_length);

    // Step 3: Finalize the MD4 computation
    // Applies padding, appends message length, and generates the final digest
    MD4Final(digest, &context);

    /**** Output the computed hash digest ****/

    // Print the resulting 128-bit digest in hexadecimal format (32 hex characters)
    printf("MD4 Digest: ");
    for (i = 0; i < 16; i++)
    {
        printf("%02x", digest[i]); // Format each byte as 2-digit lowercase hex
    }
    printf("\n");

    return 0; // Successful execution
}