# DOCUMENTATION

Charm has an extremely simple syntax. Everything is space delimited, and there is only one special construct - the function definition. Functions are defined using `function name := function body` and are tail call optimized for most use cases. Lists are defined using `[ ]`. Strings are defined using `" "`. Numbers can be either integers (`long long`s) or floats (`long double`s). The stack is initialized to 20000 (to be changed) zero integers.

Everything in Charm is a function - there are number functions, string functions, and list functions.

Many functions are preprogrammed (in C++) into Charm. This includes object and stack manipulation, arithmetic, and some combinators. But, others are in the standard library of Charm, called `Prelude.charm`. The syntax for the documentation is as follows (taking `swap`, as an example):

function name - function description - function arguments

- `swap` - swaps two functions - s[1]: the first function's pointer, s[2]: the second function's pointer

All arguments are popped off the stack, and explained in the order that you should pass them (reverse order from how they actually are on the stack).

## PREPROGRAMMED FUNCTIONS

### CONTROL FLOW

- `ifthen` - takes a condition to run, then a list to run if the top of the stack is greater than 0 and a list to run if the top of the stack is less than 0 - s[2]: condition (as a list of functions), s[1]: truthy list (as a list of functions), s[2]: falsy list (as a list of functions).

NOTE: `ifthen` doesn't pop the condition afterwards.

ANOTHER NOTE: `ifthen` is tail call optimized, meaning that if you call your function from the very end (tail) of an `ifthen` call, you won't pollute your computer's call stack and it will be as fast as a C `for` loop.

- `i` - like Lisp's `unquote`, pop a list off the stack then run it - s[0]: the list to `i`nterpret

### IO

- `pp` - pops a value and prints it - s[0]: function to print

- `newline` - prints a newline

### STACK MANIPULATION

- `dup` - duplicates a function - s[0]: function to duplicate

- `pop` - pops a function - s[0]: function to pop

- `swap` - swaps two functions n elements from the top of the stack - s[1]: the first function's pointer integer, s[0]: the second function's pointer integer

### LIST / STRING MANIPULATION

- `len` - returns length of list or string - s[0]: list or string to measure length of

- `at` - returns element at index of list or string - s[1]: list or string to index, s[0]: index integer

- `insert` - inserts element at index of function - s[2]: list to be inserted in, s[1]: function to insert, s[0]: index to insert at

- `concat` - concatenates two lists or strings - s[1]: first list or string to concatenate, s[0]: second list or string to concatenate

### ARITHMETIC (NOTE: floating point operations aren't implemented yet)

- `+` - adds - s[1], s[0]

- `-` - subtracts - s[1], s[0]

- `*` - multiplies - s[1], s[0]

### STACK CREATION / DESTRUCTION

NOTE: The default stack is christened the name `0`. Any stack that tries to create a new stack of name `0` will face _undefined behavior_.

- `createstack` - creates a new stack with a specified name and length, but does not switch to it - s[1]: the length of the stack, s[0]: the name of the stack (any type here is permissible!)

NOTE: creating a large stack may take a longer time! Stacks are implemented as `std::vector`s of a fairly large struct, which has to be initialized on creation.

- `switchstack` - switch to the specified stack - s[0]: the name of the stack to switch to

### REFERENCE GETTING / SETTING

- `getref` - gets a reference to an object by the name specified, puts it on the top of the stack - s[0]: the name of the object

NOTE: getting an undefined reference will just push a `0` to the top of the stack, just like if you were to pop off of an empty stack.

- `setref` - sets the value of an object referred to by the name specified - s[1]: the name of the object, s[0]: the value of the object

### RIPPED OUT OF `PredefinedFunctions.CPP`

```
const std::vector<std::string> PredefinedFunctions::cppFunctionNames = {
	//INPUT / OUTPUT
	"pp", "newline",
	//STACK MANIPULATIONS
	"dup", "pop", "swap",
	//TRAVERSABLE (STRING/LIST) MANIPULATIONS
	"len", "at", "insert", "concat",
	//LIST MANIPULATIONS
	//TODO
	"fold", "map", "zip",
	//STRING MANIPULATIONS
	//TODO
	"tocharlist", "fromcharlist"
	//CONTROL FLOW
	"i", "ifthen",
	//BOOLEAN OPS - TRUE: >=1, FALSE: <1 - INTEGER ONLY
	"nor",
	//TYPE INSPECIFIC MATH
	"abs",
	//INTEGER OPS
	"+", "-", "/", "*", "%", "toint",
	//FLOAT OPS
	"+f", "-f", "/f", "*f", "tofloat",
	//STACK CREATION/DESTRUCTION
	"createstack", "switchstack",
	//REF GETTING/SETTING
	"getref", "setref"
};
```

## PRELUDE FUNCTIONS



```
" OUTPUT FUNCTIONS " pop
" ================ " pop

p := dup pp
print := p newline

" STACK MANIPULATION " pop
" ================== " pop

flip := 0 1 swap

swapnth := dup 1 + swap

" <stack index> copyfrom " pop
copyfrom := " copyfromref " flip setref 0 " copyfromref " getref swap dup 1 " copyfromref " getref 1 + swap

" <object> <stack index> pushto " pop
_pushto_args := 0 flip
_pushto_cond := dup 2 copyfrom -
_pushto_inc  := flip 1 + flip
" FIXME " pop
pushto       := [ _pushto_cond ] [ 1 copyfrom 2 + swapnth _pushto_inc ] [ pop pop ] ifthen

" <object> <number of copies> stack " pop
stack := [ 1 - dup ] [ flip dup 0 2 swap stack ] [ pop ] ifthen


" LIST MANIPULATION " pop
" ================= " pop

" [ list ] <from> <to> cut " pop
_cut_distance := dup 2 copyfrom -
cut           := [ _cut_distance ] [  ] [ ] ifthen

" [ list ] <number> repeat " pop
_repeat_args := flip dup 0 2 swap 1 -
_repeat_iter := 0 2 swap dup 0 2 swap concat flip 2 0 swap 1 -
_repeat      :=  [ dup ] [ _repeat_iter _repeat ] [ pop flip pop ] ifthen
repeat       := _repeat_args _repeat

" [ list ] [ function ] map " pop
" FIXME " pop
_map_args := [ ] flip
_map_iter := dup
map       := dup

```

# OTHER NOTES

Charm uses a self-written optimizing interpreter. I'm very interested in the use cases and the effectiveness of the optimizations. The interpreter performs two optimizations: inlining and tail-call.

Inlining optimization is enabled by default through the compilation option `-DOPTIMIZE_INLINE=true`. Inlining optimization occurs if the interpreter detects that a function isn't recursive. If it isn't, the interpreter writes in the contents of the function wherever it is called, instead of writing the function itself (like a text macro). This removes 1 (or more, depending on how deep the inlining goes) layer of function redirection.

Tail-call optimization is necessary for this language, as there are no other ways to achieve a looping construct but recursion. There are a few cases which get tail-call optimized into a loop. These few cases are:

* `f := <code> f`
* `f := [ <cond> ] [ <code> f ] [ <code> ] ifthen`
* `f := [ <cond> ] [ <code> ] [ <code> f ] ifthen`
* `f := [ <cond> ] [ <code> f ] [ <code> f ] ifthen` (gets unrolled into a loop of the first form, ends up looking like `f := [ <cond> ] [ <code> ] [ <code> ] ifthen f`)

(If you can think of any other cases or a more general case, please open an issue!). These optimizations should allow for looping code that does not smash the calling stack and significant speedups. If there are any cases where these optimizations seem to be causing incorrect side effects, please create an issue or get into contact with me.