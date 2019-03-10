
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "jsmalloc.h"

/* reverse_string: Returns a new string with the characters reversed.

It is the caller's responsibility to jsfree the result.

s: string
returns: string
*/
char *reverse_string(char *s) {
  int i = 0;
  int j = 0;
  /* if array of s is bigger than 80, only first 79 characters
    will be reversed & returned. */
  while (s[i] != '\0' && i < 79) {
    i++;
  }

  /* Create a non-volatile pointer to space for the reversed string */
  char *str;
  str = (char *) jsmalloc(80);

  /* Create a buffer and reverse the string */
  char reversed[80];
  reversed[i] = '\0';
  while (i > 0) {
    i--;
    reversed[i] = s[j];
    j++;
  }
  /* Copy the result into the space allocated earlier */
  strcpy(str, reversed);
  return str;
}

/* ctoi: Converts a character to integer.

c: one of the characters '0' to '9'
returns: integer 0 to 9
*/
int ctoi(char c) {
    assert(isdigit(c));
    return c - '0';
}

/* itoc: Converts an integer to character.

i: integer 0 to 9
returns: character '0' to '9'
*/
char itoc(int i) {
    assert(i % 1 == 0);
    assert(0 <= i && i < 10);
    char nums[10] = "0123456789";
    return nums[i];
}

/* add_digits: Adds two decimal digits, returns the total and carry.

For example, if a='5', b='6', and carry='1', the sum is 12, so
the output value of total should be '2' and carry should be '1'

a: character '0' to '9'
b: character '0' to '9'
c: character '0' to '9'
total: pointer to char
carry: pointer to char

*/
void add_digits(char a, char b, char c, char *total, char *carry) {
    int A = ctoi(a);
    int B = ctoi(b);
    int C = ctoi(c);
    int sum = A+B+C;
    char Total = itoc(sum % 10);
    char Carry = itoc(sum / 10);
    *total = Total;
    *carry = Carry;
}

/* Define a type to represent a BigInt.
   Internally, a BigInt is a string of digits, with the digits in
   reverse order.
*/
typedef char * BigInt;

/* add_bigint: Adds two BigInts

Stores the result in z.

x: BigInt
y: BigInt
carry_in: char
z: empty buffer
*/
void add_bigint(BigInt x, BigInt y, char carry_in, BigInt z) {
    char total, carry_out;
    int dx=1, dy=1, dz=1;
    char a, b;

    /* OPTIONAL TODO: Modify this function to allocate and return z
    *  rather than taking an empty buffer as a parameter.
    *  Hint: you might need a helper function.
    */

    if (*x == '\0') {
        a = '0';
        dx = 0;
    }else{
        a = *x;
    }
    if (*y == '\0') {
        b = '0';
        dy = 0;
    }else{
        b = *y;
    }

    add_digits(a, b, carry_in, &total, &carry_out);

    // if total and carry are 0, we're done
    if (total == '0' && carry_out == '0') {
        *z = '\0';
        return;
    }
    // otherwise store the digit we just computed
    *z = total;

    // and make a recursive call to fill in the rest.
    add_bigint(x+dx, y+dy, carry_out, z+dz);
}

/* print_bigint: Prints the digits of BigInt in the normal order.

big: BigInt
*/
void print_bigint(BigInt big) {
    char c = *big;
    if (c == '\0') {
      puts("\n");
      return;
    }
    print_bigint(big+1);
    printf("%c", c);
}

/* make_bigint: Creates and returns a BigInt.

Caller is responsible for jsfreeing.

s: string of digits in the usual order
returns: BigInt
*/
BigInt make_bigint(char *s) {
    char *r = reverse_string(s);
    return (BigInt) r;
}

void test_reverse_string() {
    char *s = "123";
    char *t = reverse_string(s);
    if (strcmp(t, "321") == 0) {
        printf("reverse_string passed\n");
    } else {
        printf("reverse_string failed\n");
    }
}

void test_itoc() {
    char c = itoc(3);
    if (c == '3') {
        printf("itoc passed\n");
    } else {
        printf("itoc failed\n");
    }
}

void test_add_digits() {
    char total, carry;
    add_digits('7', '4', '1', &total, &carry);
    if (total == '2' && carry == '1') {
        printf("add_digits passed\n");
    } else {
        printf("add_digits failed\n");
    }
}

void test_add_bigint() {
    char *s = "1";
    char *t = "99999999999999999999999999999999999999999999";
    char *res = "000000000000000000000000000000000000000000001";

    BigInt big1 = make_bigint(s);
    BigInt big2 = make_bigint(t);
    BigInt big3 = jsmalloc(100);

  	add_bigint(big1, big2, '0', big3);

    if (strcmp(big3, res) == 0) {
        printf("add_bigint passed\n");
    } else {
        printf("add_bigint failed\n");
    }
}

int main (int argc, char *argv[])
{
    test_reverse_string();
    test_itoc();
    test_add_digits();
    test_add_bigint();
    traverse_blocks();
    return 0;
}
