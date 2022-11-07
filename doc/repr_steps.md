# Representer steps

The representer follows several serial steps in order to produce an IR file.

At first it may look like a long document full of details but I think it's readable with ease. Take your time to understand each step.

## 0 — Considrations

The code snippets in this document don't use the C90 standard library and replaces its functions with comments. Feel free to use the standard library if needed.

WISE UP! While reading this document you will notice that the syntax of the C language is purposely made to be easy to understand by both *compilers* and *humans*. This, however, requires seeing the C syntax from *another perspective*.

### UTF-8 compliance

The representer parses the file as a *UTF-8* string. However, the code snippets in this document treat the file as an *ASCII* string, for the sake of readability. To replace ASCII parsing with UTF-8 parsing (a character is called *code point* in Unicode terminology):

 - Allocate strings in heap, unless you are really sure that they won't be changed for their entire lifetime.
 - Stop thinking that 1 rendered character maps to 1 byte. In UTF-8, a code point it may be from 1 up to 4 bytes long.
 - DO NOT manually change strings (e.g. `str[i] = '...'`), it may corrupt the UTF-8 encoding. Instead, create (allocate) a new string each time you want to modify one.
 - Make functions to handle UTF-8 strings, such as:
   - A function that gets the nth code point of a UTF-8 string. Use it instead of `str[n]` (except on `str[n] = ...`) where `str` is a UTF-8 encoded string and `n` is the nth code point.
   - A function that, given a string, creates a substring starting from an index `i` to an index `j` (closed interval) and returns the new string.

Obviously, handling a UTF-8 string is more expensive in computation compared to an ASCII string.

## 1 — Preprocessing

This step consists of figuring out two things: the *type* of preprocessor instruction (e.g. "define", "include", "pragma", etc...) and the *value* of the instruction (e.g. "<someheader.h>", ""myheader.h"", "LENGTH 123", "once", etc...).

The type of preprocessor instruction is delimited by the "#" character and the first whitespace character. A clever move is to go through the types of instructions and see the one which has the *longest* name ("include", 7 characters).

```C
int i, j;
char* src_file;
char preproc_type[8];

/* some code that goes through src_file using i as index... */

/* now src_file[i] = "#" */
j = 0;
while (/* is src_file[i] a whitespace character? (e.g. a space, a tab, a newline, etc...) */) {
	if (j > 6) {
		/* ERROR! Unknown directive (too long) */
	}
	preproc_type[j] = src_file[i];
	j++
}

/* now, unless an error occurred, preproc_type contains the directive type */
```

The value of the instruction is contained between the first space after the "#" character and the next non-escaped newline character "\n". Since you cannot know exactly how long is the value, you have two options: you either use a for loop to read how many characters you need or you process the value as you read it, without allocating anything. For simplicity, I will use the latter.

```C
/* this code continues the previous one, which means that src_file[i] contains a whitespace character */

char* preproc_value = &src_file[i+1];
int temp_term; /* index of the temporary null terminator */

/* now read the string until you find a newline and replace it with a temporary null terminator */
i++;
while (src_file[i] != "\n") { i++; }
temp_term = i;
```

Now, unless any error occurred, you have everything you need: the type and the value. Time to do the stuff.
