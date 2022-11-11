# Representer steps

The representer follows several serial steps in order to produce an IR file.

At first it may look like a long document full of details but I think it's readable with ease. Take your time to understand each step.

## 0 — Considrations

WISE UP! While reading this document you will notice that the syntax of the C language is purposely made to be easy to understand by both *compilers* and *humans*. This, however, requires seeing the C syntax from *another perspective*.

### 0.1 — Code snippets

The code snippets in this document don't use the C90 standard library and replaces its functions with comments. Feel free to use the standard library if needed.

Inside of a section, there may be more than one code snippet. In that case, the snippets are meant to be seen as a single, larger code block composed of all that section's snippets in chronological order, except when they are examples.

### 0.2 — UTF-8 compliance

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

Now, unless any error occurred, you have everything you need: the type and the value of a preprocessor directive. Time to do the stuff. Now, if I was writing it using a higher-level language, I would just compare the strings using a switch statement but in C you cannot compare them because they are pointers. So, what to do?

A clever move is to, instead of comparing the whole strings, just focus on one or few characters in the right positions. In this case, what are the possible strings for the directive type? "include", "pragma", "define", "undef", "ifdef", "ifndef" and "endif". Let's start from comparing the first character: we can say that 'p', 'd', 'u' and 'e' are the first characters of one word only, while 'i' is shared among many words ("include", "ifdef", etc...). Now take a look at the following code.

```C
switch (preproc_type[0]) {
case 'p': /* pragma */
	/* ... */
	break;

case 'd': /* define */
	/* ... */
	break;

case 'u': /* undef */
	/* ... */
	break;

case 'e': /* endif */
	/* ... */
	break;

case 'i':
	switch (preproc_type[2]) {
	case 'c': /* include */
		/* ... */
		break;

	case 'd': /* ifdef */
		/* ... */
		break;

	case 'n': /* ifndef */
		/* ... */
		break;

	default:
		/* unknown */
		break;
	}

default:
	/* unknown */
	break;
}
```

In this way, by comparing just one or, at most, two characters, it is possible to guess the whole word. However, the downside of this approach is that an unknown word may still match in one of the cases, which may still be useful in case the user writes a typo.

You should know what each directive does.

For better code readability, it is advised to split each directive execution in *separate functions* (e.g. a function which executes the `include` directive, another one for the `define`, directive, etc...).

## 2 — Line reconstruction

Line reconstruction helps in parsing the rest of the file and consists of removing all the whitespace characters.

It is not always possible to remove whitespace so let's take a moment to understand how and where to do it. Consider the following line of code.

```C
int i = 3;
```

It is possible to remove all the whitespace after `i` but not between `int` and `i` because it would fuse the type with the name and it would not be possible to understand where one ends and the other begins. This consideration leads to a rule: *a word, meant as a string of visible characters between two whitespaces, can be joined with another word by removing the whitespace in-between only if the last character of the first word and the first character of the second word are not both alphanumeric or underscores*.

Using the rule above, the line becomes...

```C
int i=3;
```

A more elaborate example:

```C
if (i != 3) {
	int _j = 42;
	i += _j;
}
```

Which becomes...

```C
if(i!=3){int _j=42;i+=_j;}
```

In the following steps, for the sake of readability, code snippets are *not* going to be shown as if they were subjected to line reconstruction.

During line reconstruction, it is also useful to remove comments.

## 3 — Replacement

This is the point where 2pcc really diverges from other compilers, which would tokenize the source file now. Instead, 2pcc takes a different approach. More details ahead.

### 3.1 — Type replacement

Did you know that writing *this*

```C
struct mystruct {
	int i1;
	int i2;
	char c1;
}

void myfunc() {
	struct mystruct var;
}
```

is exactly the same as *this*?

```C
void myfunc() {
	struct mystruct {
		int i1;
		int i2;
		char c1;
	} var;
}
```

So that's what it does, it *replaces type names with their actual structure*. This is also valid for `enum`s and `union`s.

On the other hand, `typedef`s create type aliases. If one type alias is found, the representer will search for the original type name (or another alias, in case of multiple cascading `typedef`s in which only one refers to the original) before replacing.

```C
/* an example of multiple cascading typedefs in which only one refers to the original type */

struct mystruct { /* ... */ }

/* struct mystruct <- myalias */
typedef struct mystruct myalias;

/* struct mystruct <- myalias <- youralias */
typedef myalias youralias;
```

As type definitions are replaced, they are removed from the source file.

During this step, it is also possible to check for undefined types.

### 3.2 — Expression validity check and replacement

Expressions are the next thing to replace. There are 5 places to search in:

 - *`if` and `switch` conditions*, between "if(" and ")" and "switch(" and ")"
 - *`while` and `do-while` conditions*, between "while(" and ")" and "do{...}while(" and ")"
 - *`for` conditions*, between "for(...;" and ";"
 - *function arguments in calls*, between '(' and ')', '(' and ',', ',' and ',' or ',' and ')'

To check the validity of an expression (and possibly replace it), a syntax tree would be the best option.

The tree is built in this way:

 1. search for the innermost expressions between parentheses '(' and ')' (not required if the expression does not use parentheses)
 2. check the validity of those expressions, in particular:
    - check that the number(s) are actually valid numbers (no other characters unless to specify the number base or the integer suffix)
    - check that whether the operator is unary or binary
    - add the number(s) to a [BST](https://en.wikipedia.org/wiki/Binary_search_tree) as leaves and the operator as their parent node
    - continue by examining the outer expressions and attaching them to the same BST (it is important to pay attention to the operators' precedence)
 3. reduce the BST tree by calculating constant expressions

In case of an expression made of constants only, the result is constant.

For example this code:

```C
int j;

/* assign a value to j... */

/* expression */
int i = 12 + 34 * (567 - 89 * (- j)) - 42;
```

produces the following BST.

```
     -
    / \
   +   42
  / \
12   *
    / \
  34   -
      / \
   567   *
        / \
      89   -
          / \
         0   j
```

Once one expression's BST is completed, it can be converted to IR code.

```
# push variable j and i

(assign i 0)
(sub i j)
(mul i 89)
(sub i 567)
(mul i -1) # multiply by -1 because the subtraction was done in reversed order
(mul i 34)
(add i 12)
(sub i 42)
```

It is worth saying that, while the IR code above is completely valid, it can be simplified because it contains a constant expression. Here is the BST of the reduced expression:

```
    +
   / \
-30   *
     / \
   34   -
       / \
    567   *
         / \
       89   -
           / \
          0   j
```

because 12 - 42 = -30.

Also, keep in mind that expressions may also contain *function calls*, in which case we just need to call the function, store the returned value in a temporary variable and use that variable as part of the expression.

The BST can be implemented as an array of the following `struct _operand`.

```C
/* operator type */
enum oprtype {
	ASSIGNMENT,         /* = */
	ADDASSIGN,          /* += */
	SUBASSIGN,          /* -= */
	TIMESASSIGN,        /* *= */
	DIVIDEASSIGN,       /* /= */
	MODULOASSIGN,       /* %= */
	ANDASSIGN,          /* &= */
	ORASSIGN,           /* |= */
	XORASSIGN,          /* ^= */
	LSHIFTASSIGN,       /* <<= */
	RSHIFTASSING,       /* >>= */
	PREINCR,            /* ++a */
	PREDECR,            /* --a */
	POSTINCR,           /* a++ */
	POSTDECR,           /* a-- */
	UNPLUS,             /* +a */
	UNMINUS,            /* -a */
	ADD,                /* a + b */
	SUB,                /* a - b */
	TIMES,              /* a * b */
	DIVIDE,             /* a / b */
	MODULO,             /* a % b */
	BITNOT,             /* ~a */
	BITAND,             /* a & b */
	BITOR,              /* a | b */
	BITXOR,             /* a ^ b */
	BITLSHIFT,          /* a << b */
	BITRSHIFT,          /* a >> b */
	LOGICNOT,           /* !a */
	LOGICAND,           /* a && b */
	LOGICOR,            /* a || b */
	EQUALTO,            /* a == b */
	NOTEQUALTO,         /* a != b */
	LESSTHAN,           /* a < b */
	GREATERTHAN,        /* a > b */
	LESSTHANEQUALTO,    /* a <= b */
	GREATERTHANEQUALTO, /* a >= b */
	ARRAYSUBSCRIPT,     /* a[b] */
	DEREFERENCE,        /* *a */
	REFERENCE,          /* &a */
	MEMBACCESS,         /* a.b */
	PTRMEMBACCESS,      /* a->b */
	FNCALL,             /* a(...) */
	COMMA,              /* a, b */
	TYPECAST,           /* (b) a */
	COND,               /* a ? b : c */
	SIZEOF              /* sizeof(a), sizeof a */
}

struct _operand {
	int isopr; /* is operator? */
	union {
		enum oprtype oprtype;
		struct {
			int islit; /* is literal? */
			char* strvalue; /* value as string */
		}
	}
}
```

#### 3.2.1 — Mismatching types and type casts

There may be scenarios where types do not match. This may be caused by 2 factors:

 - usage of variables or constant values with mismatching types (e.g. `53 + 4.2` or `a + b` where `a` is `int` and `b` is `float`)
 - usage of type casts resulting in mismatching types

In case of type casts, after applying them, there may be two scenarios: one in which the types match (best) and one in which they don't (worst). In case all types match, the expression can proceed with evaluation. In case at least one type does not match, the types should converge into a single common type by following a primitive type hierarchy.

In case of 2 mismatching types:

 - in case of two integer types (including `char`), both types are promoted to the *next larger type* (if any) and, if at least one of them is signed, the larger type is signed as well.
 - in case of one integer and one floating point type, *the floating point type is promoted to a signed integer* (`float` is promoted to `int`, `double` is promoted to `long int`)
 - in case of two floating point types, the smallest of the two is promoted to the type of other one

In case of an expression containing more than 2 mismatching types, the types are resolved two by two, following the order of execution.

Assignment operations break the rules: the assigned value is *always* casted to the variable's type, except when the two types are incompatible (the variable is smaller than the space needed by the value and/or the variable is unsigned and the value is signed).

