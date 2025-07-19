"""
Filename: md2.py

Implementation Details:
  This file implements the MD2 (Message Digest 2) cryptographic hash algorithm as specified in RFC 1319.
  MD2 produces a 128-bit (16-byte) hash digest from input data of arbitrary length. The algorithm uses
  a substitution table based on π (pi) and processes data in 16-byte blocks with padding and checksum.

Classes:
  - MD2Context
      Attributes:
        state: 16-byte internal state buffer (D).
        checksum: 16-byte checksum buffer (C).
        count: Number of bytes processed, modulo 16.
        buffer: 16-byte input buffer for incomplete blocks.
      Description:
        Context class to maintain algorithm state during computation.

Functions:
  - md2_init() -> MD2Context
      Returns:
        MD2Context: Initialized MD2 context structure.
      Description:
        Initializes and returns a new MD2 context structure with all buffers set to zero.

  - md2_update(context: MD2Context, input_data: bytes) -> None
      Parameters:
        context: MD2Context object maintaining hash state.
        input_data: Input data to be processed as bytes.
      Description:
        Processes input data in chunks, updating the internal state and applying transformations.

  - md2_final(context: MD2Context) -> bytes
      Parameters:
        context: MD2Context object with current hash state.
      Returns:
        bytes: Final 16-byte hash digest.
      Description:
        Finalizes the hash computation by applying padding, processing checksum, and generating final digest.

  - _md2_transform(state: bytearray, checksum: bytearray, block: bytes) -> None
      Parameters:
        state: 16-byte state buffer to be updated.
        checksum: 16-byte checksum buffer to be updated.
        block: 16-byte input block to process.
      Description:
        Performs the core MD2 transformation on a single 16-byte block using substitution and XOR operations.

  - md2_hash(data: bytes) -> bytes
      Parameters:
        data: Input data to be hashed as bytes.
      Returns:
        bytes: 16-byte MD2 hash digest.
      Description:
        Convenience function that computes MD2 hash in a single call.

  - main() -> None
      Description:
        Demonstrates MD2 hash computation by processing command-line input and displaying result.

Usage:
  - For single-shot hashing: use md2_hash(data)
  - For streaming: create context with md2_init(), update with md2_update(), finalize with md2_final()
  - Command line: python md2.py <input_string>

Notes:
  - MD2 is considered cryptographically weak and should not be used for security-critical applications.
  - The algorithm uses a pre-computed substitution table based on π for randomization.
  - Input is padded to ensure length is multiple of 16 bytes before final processing.
  - A checksum is computed and appended before final transformation.

Time Complexity:
  - O(n): Processing time is linear with input size, where n is the number of input bytes.

Space Complexity:
  - O(1): Uses constant space for internal buffers (48 bytes for state, checksum, and temporary data).
"""

import sys

class MD2Context:
    """
    Class: MD2Context
    -----------------
    Context class to maintain MD2 algorithm state during computation.
    Stores internal buffers and counters needed for hash calculation.
    
    Attributes:
        state: 16-byte internal state buffer (D).
        checksum: 16-byte checksum buffer (C).
        count: Number of bytes processed, modulo 16.
        buffer: 16-byte input buffer for incomplete blocks.
    
    Notes:
        - All buffers are initialized to zero
        - State and checksum are modified during transformation
        - Buffer accumulates input until 16-byte blocks are available
        - Count tracks position for padding calculation
    """
    
    def __init__(self):
        self.state = bytearray(16)      # 16-byte internal state buffer (D)
        self.checksum = bytearray(16)   # 16-byte checksum buffer (C)
        self.count = 0                  # Number of bytes processed, modulo 16
        self.buffer = bytearray(16)     # 16-byte input buffer for incomplete blocks


# Pre-computed substitution table S based on the digits of π (pi).
# This 256-byte table provides pseudo-random permutation for MD2 transformations.
# Each value from 0-255 appears exactly once, making it a permutation table.
PI_SUBST = [
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
]

# Padding bytes used to ensure input length is multiple of 16.
# PADDING[i] contains i bytes, each with value i.
# For example: PADDING[3] = b"\x03\x03\x03" (3 bytes of value 3)
PADDING = [
    b"",
    b"\x01",
    b"\x02\x02",
    b"\x03\x03\x03",
    b"\x04\x04\x04\x04",
    b"\x05\x05\x05\x05\x05",
    b"\x06\x06\x06\x06\x06\x06",
    b"\x07\x07\x07\x07\x07\x07\x07",
    b"\x08\x08\x08\x08\x08\x08\x08\x08",
    b"\x09\x09\x09\x09\x09\x09\x09\x09\x09",
    b"\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A\x0A",
    b"\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B\x0B",
    b"\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C\x0C",
    b"\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D\x0D",
    b"\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E\x0E",
    b"\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F",
    b"\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10\x10"
]


def _md2_transform(state: bytearray, checksum: bytearray, block: bytes) -> None:
    """
    Function: _md2_transform
    ------------------------
    Performs the core MD2 transformation on a single 16-byte block.
    This function implements the heart of the MD2 algorithm, performing
    substitution and XOR operations through 18 rounds of encryption.
    
    Parameters:
        state: 16-byte state buffer to be updated.
        checksum: 16-byte checksum buffer to be updated.
        block: 16-byte input block to process.
    
    Returns:
        None
    
    Notes:
        - Creates 48-byte encryption block: [state][block][state XOR block]
        - Performs 18 rounds of substitution using PI_SUBST table
        - Updates both state and checksum based on input block
        - Modifies state and checksum arrays in place
    """
    # 48-byte encryption block
    x = bytearray(48)
    
    # Form encryption block from state, block, state ^ block.
    # x[0..15] = state, x[16..31] = block, x[32..47] = state XOR block
    x[0:16] = state[0:16]           # Copy state to first 16 bytes
    x[16:32] = block[0:16]          # Copy block to next 16 bytes
    for i in range(16):             # XOR for last 16 bytes
        x[i + 32] = state[i] ^ block[i]
    
    # Encrypt block (18 rounds).
    # Each round applies substitution table to all 48 bytes
    t = 0
    for i in range(18):
        for j in range(48):
            t = x[j] = x[j] ^ PI_SUBST[t]   # Apply substitution and update running value
        t = (t + i) & 0xFF                   # Add round number and keep in byte range
    
    # Save new state from first 16 bytes of encrypted block
    state[0:16] = x[0:16]
    
    # Update checksum using substitution table and input block.
    # Each checksum byte is XORed with substitution of (block byte XOR previous checksum byte)
    t = checksum[15]                         # Start with last checksum byte
    for i in range(16):
        t = checksum[i] = checksum[i] ^ PI_SUBST[block[i] ^ t]


def md2_init() -> MD2Context:
    """
    Function: md2_init
    ------------------
    Initializes and returns a new MD2 context structure for hash computation.
    Sets all internal buffers (state, checksum, buffer) to zero and resets counters.
    
    Returns:
        MD2Context: Initialized MD2 context with all buffers cleared.
    
    Notes:
        - Must be called before any md2_update() calls
        - Prepares context for processing input data
        - All internal state is cleared to ensure clean start
    """
    return MD2Context()


def md2_update(context: MD2Context, input_data: bytes) -> None:
    """
    Function: md2_update
    --------------------
    Processes input data and updates the MD2 context.
    Handles data buffering and calls _md2_transform when complete 16-byte blocks are available.
    
    Parameters:
        context: MD2Context object maintaining hash state.
        input_data: Input data to be processed as bytes.
    
    Returns:
        None
    
    Notes:
        - Can be called multiple times to process data in chunks
        - Automatically buffers incomplete blocks for next call
        - Processes complete 16-byte blocks immediately via _md2_transform
        - Updates byte counter modulo 16 for padding calculation
    """
    input_length = len(input_data)
    
    # Update number of bytes mod 16 for padding calculation
    index = context.count
    context.count = (index + input_length) & 0xF   # Keep only low 4 bits (mod 16)
    remaining_length = 16 - index                   # Bytes needed to fill current buffer
    
    # Transform as many complete 16-byte blocks as possible
    if input_length >= remaining_length:
        # Fill the buffer and process it
        context.buffer[index:16] = input_data[0:remaining_length]
        _md2_transform(context.state, context.checksum, bytes(context.buffer))
        
        # Process additional complete 16-byte blocks directly from input
        i = remaining_length
        while i + 15 < input_length:
            _md2_transform(context.state, context.checksum, input_data[i:i+16])
            i += 16
        index = 0   # Buffer is now empty
    else:
        i = 0       # No complete blocks to process
    
    # Buffer remaining input bytes for next call or final processing
    remaining_bytes = input_length - i
    if remaining_bytes > 0:
        context.buffer[index:index + remaining_bytes] = input_data[i:input_length]


def md2_final(context: MD2Context) -> bytes:
    """
    Function: md2_final
    -------------------
    Finalizes the MD2 hash computation and produces the final digest.
    Applies padding, processes the checksum, and generates the 16-byte hash result.
    
    Parameters:
        context: MD2Context object with current hash state.
    
    Returns:
        bytes: Final 16-byte hash digest.
    
    Notes:
        - Adds padding to make total length multiple of 16 bytes
        - Appends checksum as final 16-byte block
        - Returns final state as digest
        - Zeros context to prevent information leakage
        - After calling this function, context is no longer usable
    """
    # Pad out to multiple of 16 bytes.
    # Padding byte value equals number of padding bytes needed.
    index = context.count               # Current position in buffer (0-15)
    padding_length = 16 - index        # Bytes needed to reach multiple of 16
    md2_update(context, PADDING[padding_length])
    
    # Extend with checksum - process checksum as final 16-byte block
    md2_update(context, bytes(context.checksum))
    
    # Store final state as the computed digest
    digest = bytes(context.state)
    
    # Zeroize sensitive information to prevent memory attacks
    context.state = bytearray(16)
    context.checksum = bytearray(16)
    context.buffer = bytearray(16)
    context.count = 0
    
    return digest


def md2_hash(data: bytes) -> bytes:
    """
    Function: md2_hash
    ------------------
    Convenience function that computes MD2 hash in a single call.
    Handles initialization, update, and finalization automatically.
    
    Parameters:
        data: Input data to be hashed as bytes.
    
    Returns:
        bytes: 16-byte MD2 hash digest.
    
    Notes:
        - Suitable for one-shot hashing operations
        - Equivalent to: init -> update -> final sequence
        - More convenient than manual context management
    """
    context = md2_init()
    md2_update(context, data)
    return md2_final(context)


def main() -> None:
    """
    Function: main
    --------------
    Demonstrates MD2 hash computation by processing command-line input.
    Validates arguments, computes MD2 hash, and displays result in hexadecimal format.
    
    Returns:
        None
    
    Notes:
        - Requires exactly one input string as command-line argument
        - Outputs 32-character hexadecimal representation of 128-bit hash
        - Demonstrates proper MD2 usage: init -> update -> final
        - Handles UTF-8 encoding of input string
    """
    # Validate command line arguments - exactly one input string required
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <input_string>", file=sys.stderr)
        print(f"Example: {sys.argv[0]} \"hello world\"", file=sys.stderr)
        sys.exit(1)
    
    input_string = sys.argv[1]              # Input string to be hashed
    input_data = input_string.encode('utf-8')  # Convert string to bytes using UTF-8 encoding
    
    #### MD2 Hash Computation Process ####
    
    # Method 1: Using convenience function (recommended for simple cases)
    digest = md2_hash(input_data)
    
    # Method 2: Manual context management (for streaming or advanced usage)
    # context = md2_init()                    # Step 1: Initialize MD2 context
    # md2_update(context, input_data)         # Step 2: Process input data
    # digest = md2_final(context)             # Step 3: Finalize and get digest
    
    #### Output the computed hash digest ####
    
    # Print the resulting 128-bit digest in hexadecimal format (32 hex characters)
    print("MD2 Digest: ", end="")
    for byte in digest:
        print(f"{byte:02x}", end="")         # Format each byte as 2-digit lowercase hex
    print()  # Add newline


if __name__ == "__main__":
    main()
