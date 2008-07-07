#ifndef BASE85_H_INCLUDED
#define BASE85_H_INCLUDED

/*
A compact ASCII encoding suitable for embedding in source files.

The character set specifically omits:
 - all control characters (0-31)
 - the delete character (127)
 - the space character (32)
 - the backslash character \ (92), which may be used as an escape character
 - the single and double quote characters ' (39) and " (34) which are often
   used to delimit string literals
 - the question mark ? (63) which can be used in a trigraph sequence in C/C++
 - six other punctuation marks (".,!?#/") chosen more or less arbitrarily to
   arrive at a set of 85 characters.

The 85-character set (in order):
    10 digits:              0123456789
    26 uppercase letters:   ABCDEFGHIJKLMNOPQRSTUVWXYZ
    26 lowercase letters:   abcdefghijklmnopqrstuvwxyz
    23 other characters:    $%&()*+-:;<=>@[]^_`{|}~

The input is split into blocks of (at most) four bytes. Each block is
interpreted as an unsigned integer in a 4-digit base-256 representation, with
the least significant byte coming first (little-endian byte order), and output
in a 5-digit base-85 format (again, least significant byte first), where each
digit is represented by an ASCII character from the set above.

The final block may contain less than four bytes (if the total number of input
bytes is not a multiple of four). In that case, a block of three, two or one
byte(s) is encoded with four, three or two base-85 digits, respectively.
*/

/* The base-85 character set, as an 85-character zero-terminated string. */
extern const char *base85_chars;

/* Returns the codepoint for character `c' if it belongs to the base85
   character set, or -1 otherwise. */
extern const signed char base85_codes[128];
#define base85_code(c) (base85_codes[(c)&0x7f])

/* Returns the length required to encode `len' input bytes, or -1 if `len`
   is negative or the result cannot be represented as an integer. */
int base85_encode_len(int len);

/* Returns the length required to decode `len' base-85 characters, or -1 if
   `len` is negative. */
int base85_decode_len(int len);

/* Similar to base85_decode_len, but takes characters that do not belong to
   the base85 character set into account (these are ignored by base85_decode).
*/
int base85_real_decode_len(char *buf, int len);

/* Encodes `len_in` bytes referenced by `buf_in` and writes the base-85
   characters to `buf_out`. The return value is the number of base-85
   characters encoded; it's the caller's responsibility to ensure enough space
   is available to store the characters. */
int base85_encode(const char *buf_in, int len_in, char *buf_out);

/* Decodes `len_in` characters referenced by `buf_in` and writes the decoded
   bytes to `buf_out`. Characters in `buf_in` that do not belong to the
   base-85 character set are ignored. The return value is the number of bytes
   decoded; it's the caller's responsibility to ensure enough space is
   available to store the characters. */
int base85_decode(const char *buf_in, int len_in, char *buf_out);

#endif /* ndef BASE85_H_INCLUDED */
