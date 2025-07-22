#!/usr/bin/env python3
"""
Filename: md4.py

Implementation Details:
  This file implements the MD4 (Message Digest 4) cryptographic hash algorithm as specified in RFC 1320.
  MD4 produces a 128-bit (16-byte) hash digest from input data of arbitrary length. The algorithm uses
  a series of logical functions (F, G, H) and processes data in 64-byte blocks with padding.

Classes:
  - MD4: Main class implementing the MD4 hash algorithm
    Methods:
      - __init__(): Initializes the MD4 context structure.
      - update(data): Processes input data through the MD4 algorithm.
      - digest(): Finalizes the hash computation and returns the digest.
      - hexdigest(): Returns the digest as a hexadecimal string.

Functions:
  - md4_hash(data): Convenience function to compute MD4 hash of input data.

Usage:
  - Create MD4 instance and use update()/digest() methods.
  - Or use the convenience function md4_hash() for one-shot hashing.
  - The main function demonstrates usage by computing MD4 hash of command-line input.

Notes:
  - MD4 is considered cryptographically weak and should not be used for security-critical applications.
  - The algorithm uses three rounds with different logical functions (F, G, H).
  - Input is padded to ensure length is congruent to 56 bytes modulo 64 before final processing.
  - Original message length is appended as 64-bit value before final transformation.

Time Complexity:
  - O(n): Processing time is linear with input size, where n is the number of input bytes.

Space Complexity:
  - O(1): Uses constant space for internal buffers.
"""

import struct
import sys
from typing import Union

class MD4:
    """
    Class: MD4
    ----------
    Context class to maintain MD4 algorithm state during computation.
    Stores internal buffers and counters needed for hash calculation.
    
    Attributes:
        _state: 4-element array of 32-bit state variables (A, B, C, D).
        _count: Number of bits processed for message length tracking.
        _buffer: 64-byte input buffer for incomplete blocks.
        _I0, _I1, _I2, _I3: Initial hash values (magic constants).
        _C2, _C3: Round constants for transformations 2 and 3.
        _S11-S34: Shift amounts for all three rounds of transformation.
    
    Notes:
        - All buffers are initialized with magic constants or zeros
        - State variables are modified during each 64-byte block transformation
        - Buffer accumulates input until 64-byte blocks are available
        - Count tracks total bits processed for padding calculation
        - Implements MD4 algorithm as specified in RFC 1320
    """
    
    # Initial hash values for MD4 buffer (little-endian)
    _I0 = 0x67452301
    _I1 = 0xefcdab89
    _I2 = 0x98badcfe
    _I3 = 0x10325476
    
    # Round constants
    _C2 = 0x5a827999  # Round 2 constant (square root of 2)
    _C3 = 0x6ed9eba1  # Round 3 constant (square root of 3)
    
    # Shift amounts for MD4Transform rounds
    _S11, _S12, _S13, _S14 = 3, 7, 11, 19    # Round 1
    _S21, _S22, _S23, _S24 = 3, 5, 9, 13     # Round 2
    _S31, _S32, _S33, _S34 = 3, 9, 11, 15    # Round 3
    
    def __init__(self):
        """
        Function: __init__
        ------------------
        Initializes an MD4 context structure for hash computation.
        Sets up the initial state values and resets bit counters.
        
        Parameters:
            None
        
        Returns:
            None
        
        Notes:
            - Resets bit counter to zero (no bits processed yet)
            - Loads magic initialization constants (I0, I1, I2, I3) into state
            - These constants are derived from fractional parts of square roots
            - Must be called before any update operations
        """
        # Initialize bit counter to zero (no bits processed yet)
        self._count = 0
        
        # Load magic initialization constants into state variables
        # These hex values represent the fractional parts of square roots
        self._state = [self._I0, self._I1, self._I2, self._I3]
        
        # Input buffer for incomplete blocks
        self._buffer = b''
    
    @staticmethod
    def _f(x: int, y: int, z: int) -> int:
        """
        Function: _f
        ------------
        MD4 auxiliary function F for Round 1: Selection function - if x then y else z.
        
        Parameters:
            x: First 32-bit input word
            y: Second 32-bit input word  
            z: Third 32-bit input word
        
        Returns:
            int: Result of F(x,y,z) = (x & y) | ((~x) & z)
        
        Notes:
            - Implements conditional selection: if x bit is 1, select y bit, else select z bit
            - Used in Round 1 of MD4 transformation
            - Result is masked to 32 bits
        """
        return ((x & y) | ((~x) & z)) & 0xffffffff
    
    @staticmethod
    def _g(x: int, y: int, z: int) -> int:
        """
        Function: _g
        ------------
        MD4 auxiliary function G for Round 2: Majority function - at least two of x, y, z are true.
        
        Parameters:
            x: First 32-bit input word
            y: Second 32-bit input word
            z: Third 32-bit input word
        
        Returns:
            int: Result of G(x,y,z) = (x & y) | (x & z) | (y & z)
        
        Notes:
            - Implements majority vote: output bit is 1 if at least 2 input bits are 1
            - Used in Round 2 of MD4 transformation
            - Result is masked to 32 bits
        """
        return ((x & y) | (x & z) | (y & z)) & 0xffffffff
    
    @staticmethod
    def _h(x: int, y: int, z: int) -> int:
        """
        Function: _h
        ------------
        MD4 auxiliary function H for Round 3: Parity function - XOR of x, y, z.
        
        Parameters:
            x: First 32-bit input word
            y: Second 32-bit input word
            z: Third 32-bit input word
        
        Returns:
            int: Result of H(x,y,z) = x ^ y ^ z
        
        Notes:
            - Implements XOR parity: output bit is 1 if odd number of input bits are 1
            - Used in Round 3 of MD4 transformation
            - Result is masked to 32 bits
        """
        return (x ^ y ^ z) & 0xffffffff
    
    @staticmethod
    def _rotate_left(x: int, n: int) -> int:
        """
        Function: _rotate_left
        ----------------------
        Performs left circular rotation of x by n bit positions.
        
        Parameters:
            x: 32-bit value to rotate
            n: Number of positions to rotate left (0-31)
        
        Returns:
            int: Result of rotating x left by n positions
        
        Notes:
            - Implements: (x << n) | (x >> (32-n)) for 32-bit words
            - Bits shifted out on left are rotated back in on right
            - Result is masked to 32 bits to prevent overflow
        """
        return ((x << n) | (x >> (32 - n))) & 0xffffffff
    
    def _ff(self, a: int, b: int, c: int, d: int, x: int, s: int) -> int:
        """
        Function: _ff
        -------------
        Round 1 transformation operations using function F.
        
        Parameters:
            a: First state variable to be updated
            b: Second state variable (used in F function)
            c: Third state variable (used in F function)
            d: Fourth state variable (used in F function)
            x: Message word from current block
            s: Number of left rotation positions
        
        Returns:
            int: Updated value for state variable a
        
        Notes:
            - Applies F(b,c,d) + a + x, then rotates left by s positions
            - Used in Round 1 of MD4 transformation (16 operations)
            - No additive constant used in Round 1
        """
        a = (a + self._f(b, c, d) + x) & 0xffffffff
        return self._rotate_left(a, s)
    
    def _gg(self, a: int, b: int, c: int, d: int, x: int, s: int) -> int:
        """
        Function: _gg
        -------------
        Round 2 transformation operations using function G with constant C2.
        
        Parameters:
            a: First state variable to be updated
            b: Second state variable (used in G function)
            c: Third state variable (used in G function)
            d: Fourth state variable (used in G function)
            x: Message word from current block
            s: Number of left rotation positions
        
        Returns:
            int: Updated value for state variable a
        
        Notes:
            - Applies G(b,c,d) + a + x + C2, then rotates left by s positions
            - Used in Round 2 of MD4 transformation (16 operations)
            - C2 = 0x5a827999 (constant derived from square root of 2)
        """
        a = (a + self._g(b, c, d) + x + self._C2) & 0xffffffff
        return self._rotate_left(a, s)
    
    def _hh(self, a: int, b: int, c: int, d: int, x: int, s: int) -> int:
        """
        Function: _hh
        -------------
        Round 3 transformation operations using function H with constant C3.
        
        Parameters:
            a: First state variable to be updated
            b: Second state variable (used in H function)
            c: Third state variable (used in H function)
            d: Fourth state variable (used in H function)
            x: Message word from current block
            s: Number of left rotation positions
        
        Returns:
            int: Updated value for state variable a
        
        Notes:
            - Applies H(b,c,d) + a + x + C3, then rotates left by s positions
            - Used in Round 3 of MD4 transformation (16 operations)
            - C3 = 0x6ed9eba1 (constant derived from square root of 3)
        """
        a = (a + self._h(b, c, d) + x + self._C3) & 0xffffffff
        return self._rotate_left(a, s)
    
    def _md4_transform(self, block: bytes) -> None:
        """
        Function: _md4_transform
        ------------------------
        Performs the core MD4 transformation on a single 64-byte block.
        This function implements the heart of the MD4 algorithm, performing
        three rounds of operations using logical functions F, G, and H.
        
        Parameters:
            block: 64-byte input block to process
        
        Returns:
            None
        
        Notes:
            - Decodes 64-byte block into sixteen 32-bit words in little-endian format
            - Performs 48 operations in three rounds (16 operations each)
            - Round 1: Uses F function with shift amounts S11-S14
            - Round 2: Uses G function with constant C2 and shift amounts S21-S24
            - Round 3: Uses H function with constant C3 and shift amounts S31-S34
            - Updates state by adding original values to transformed values
            - Prevents length extension attacks through state addition
        """
        # Decode 64-byte block into sixteen 32-bit words (little-endian)
        x = list(struct.unpack('<16I', block))
        
        # Initialize working variables with current state
        a, b, c, d = self._state
        
        # Round 1: Apply function F with message words in order (0,1,2,...,15)
        a = self._ff(a, b, c, d, x[0], self._S11)   # 1
        d = self._ff(d, a, b, c, x[1], self._S12)   # 2
        c = self._ff(c, d, a, b, x[2], self._S13)   # 3
        b = self._ff(b, c, d, a, x[3], self._S14)   # 4
        a = self._ff(a, b, c, d, x[4], self._S11)   # 5
        d = self._ff(d, a, b, c, x[5], self._S12)   # 6
        c = self._ff(c, d, a, b, x[6], self._S13)   # 7
        b = self._ff(b, c, d, a, x[7], self._S14)   # 8
        a = self._ff(a, b, c, d, x[8], self._S11)   # 9
        d = self._ff(d, a, b, c, x[9], self._S12)   # 10
        c = self._ff(c, d, a, b, x[10], self._S13)  # 11
        b = self._ff(b, c, d, a, x[11], self._S14)  # 12
        a = self._ff(a, b, c, d, x[12], self._S11)  # 13
        d = self._ff(d, a, b, c, x[13], self._S12)  # 14
        c = self._ff(c, d, a, b, x[14], self._S13)  # 15
        b = self._ff(b, c, d, a, x[15], self._S14)  # 16
        
        # Round 2: Apply function G with message words in pattern (0,4,8,12,1,5,9,13,2,6,10,14,3,7,11,15)
        a = self._gg(a, b, c, d, x[0], self._S21)   # 17
        d = self._gg(d, a, b, c, x[4], self._S22)   # 18
        c = self._gg(c, d, a, b, x[8], self._S23)   # 19
        b = self._gg(b, c, d, a, x[12], self._S24)  # 20
        a = self._gg(a, b, c, d, x[1], self._S21)   # 21
        d = self._gg(d, a, b, c, x[5], self._S22)   # 22
        c = self._gg(c, d, a, b, x[9], self._S23)   # 23
        b = self._gg(b, c, d, a, x[13], self._S24)  # 24
        a = self._gg(a, b, c, d, x[2], self._S21)   # 25
        d = self._gg(d, a, b, c, x[6], self._S22)   # 26
        c = self._gg(c, d, a, b, x[10], self._S23)  # 27
        b = self._gg(b, c, d, a, x[14], self._S24)  # 28
        a = self._gg(a, b, c, d, x[3], self._S21)   # 29
        d = self._gg(d, a, b, c, x[7], self._S22)   # 30
        c = self._gg(c, d, a, b, x[11], self._S23)  # 31
        b = self._gg(b, c, d, a, x[15], self._S24)  # 32
        
        # Round 3: Apply function H with message words in pattern (0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15)
        a = self._hh(a, b, c, d, x[0], self._S31)   # 33
        d = self._hh(d, a, b, c, x[8], self._S32)   # 34
        c = self._hh(c, d, a, b, x[4], self._S33)   # 35
        b = self._hh(b, c, d, a, x[12], self._S34)  # 36
        a = self._hh(a, b, c, d, x[2], self._S31)   # 37
        d = self._hh(d, a, b, c, x[10], self._S32)  # 38
        c = self._hh(c, d, a, b, x[6], self._S33)   # 39
        b = self._hh(b, c, d, a, x[14], self._S34)  # 40
        a = self._hh(a, b, c, d, x[1], self._S31)   # 41
        d = self._hh(d, a, b, c, x[9], self._S32)   # 42
        c = self._hh(c, d, a, b, x[5], self._S33)   # 43
        b = self._hh(b, c, d, a, x[13], self._S34)  # 44
        a = self._hh(a, b, c, d, x[3], self._S31)   # 45
        d = self._hh(d, a, b, c, x[11], self._S32)  # 46
        c = self._hh(c, d, a, b, x[7], self._S33)   # 47
        b = self._hh(b, c, d, a, x[15], self._S34)  # 48
        
        # Add the transformed values back to the state (prevents length extension attacks)
        self._state[0] = (self._state[0] + a) & 0xffffffff
        self._state[1] = (self._state[1] + b) & 0xffffffff
        self._state[2] = (self._state[2] + c) & 0xffffffff
        self._state[3] = (self._state[3] + d) & 0xffffffff
    
    def update(self, data: Union[str, bytes]) -> None:
        """
        Function: update
        ----------------
        Processes input data through the MD4 algorithm.
        Handles buffering and calls _md4_transform for complete 64-byte blocks.
        
        Parameters:
            data: Input data to be processed (string or bytes)
        
        Returns:
            None
        
        Notes:
            - Converts string input to UTF-8 bytes if necessary
            - Updates bit count for total message length tracking
            - Processes complete 64-byte blocks immediately via transformation
            - Buffers remaining bytes for future processing
            - Can be called multiple times to process data incrementally
            - Automatically handles block boundaries and buffering
        """
        if isinstance(data, str):
            data = data.encode('utf-8')
        
        # Update bit count
        input_length = len(data)
        self._count += input_length * 8
        
        # Combine with buffered data
        data = self._buffer + data
        
        # Process complete 64-byte blocks
        for i in range(0, len(data) - 63, 64):
            self._md4_transform(data[i:i+64])
        
        # Buffer remaining bytes
        self._buffer = data[len(data) - (len(data) % 64):]
    
    def digest(self) -> bytes:
        """
        Function: digest
        ----------------
        Finalizes the MD4 hash computation and produces the final digest.
        Applies padding, appends message length, and performs final transformation.
        
        Parameters:
            None
        
        Returns:
            bytes: The 16-byte MD4 hash digest
        
        Notes:
            - Calculates current buffer position for padding
            - Adds padding to make total length ≡ 56 (mod 64) bytes
            - Appends original message length as 64-bit little-endian value
            - Performs final transformation on padded data
            - Returns final state as 16-byte digest in little-endian format
            - Context state is modified and cannot be reused without re-initialization
        """
        # Create a copy to preserve state for potential future operations
        mdi = self._count // 8 % 64
        length = self._count
        
        # Apply padding: append '1' bit followed by zeros
        if mdi < 56:
            pad_length = 56 - mdi
        else:
            pad_length = 120 - mdi
        
        padding = b'\x80' + (b'\x00' * (pad_length - 1))
        self.update(padding)
        
        # Append original message length in bits as 64-bit little-endian value
        length_data = struct.pack('<Q', length)
        self.update(length_data)
        
        # Produce the final hash value (little-endian)
        return struct.pack('<4I', *self._state)
    
    def hexdigest(self) -> str:
        """
        Function: hexdigest
        -------------------
        Returns the digest as a hexadecimal string representation.
        
        Parameters:
            None
        
        Returns:
            str: The digest as a 32-character lowercase hexadecimal string
        
        Notes:
            - Calls digest() to get binary hash result
            - Converts each byte to 2-digit hexadecimal representation
            - Returns 32-character string (16 bytes × 2 hex digits per byte)
            - Uses lowercase letters for hex digits a-f
        """
        return self.digest().hex()


def md4_hash(data: Union[str, bytes]) -> bytes:
    """
    Function: md4_hash
    ------------------
    Convenience function to compute MD4 hash of input data in one operation.
    
    Parameters:
        data: Input data to hash (string or bytes)
    
    Returns:
        bytes: The 16-byte MD4 hash digest
    
    Notes:
        - Creates new MD4 instance for one-time hashing
        - Automatically handles initialization, update, and finalization
        - Suitable for simple use cases where incremental hashing is not needed
        - Equivalent to: MD4().update(data).digest()
    """
    hasher = MD4()
    hasher.update(data)
    return hasher.digest()


def main() -> None:
    """
    Function: main
    --------------
    Demonstrates MD4 hash computation by processing command-line input.
    Validates arguments, computes MD4 hash, and displays result in hexadecimal format.
    
    Parameters:
        None (uses sys.argv for command-line arguments)
    
    Returns:
        None (exits with status code 1 on error)
    
    Notes:
        - Requires exactly one input string as command-line argument
        - Outputs 32-character hexadecimal representation of 128-bit hash
        - Demonstrates proper MD4 usage: initialization -> update -> digest
        - Provides usage examples when arguments are invalid
    """
    # Validate command line arguments - exactly one input string required
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <input_string>", file=sys.stderr)
        print(f"Example: {sys.argv[0]} \"hello world\"", file=sys.stderr)
        return 1
    
    input_string = sys.argv[1]
    
    # Compute MD4 hash
    hasher = MD4()
    hasher.update(input_string)
    digest = hasher.hexdigest()
    
    # Output the computed hash digest
    print(f"MD4 Digest: {digest}")


if __name__ == "__main__":
    main()
