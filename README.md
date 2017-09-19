**(WARNING: Large chunks of this README are outdated)**

Rubberband is a very minimalistic and customizable experimental programming
language. This manual introduces the language's goals and characteristics and,
then, presents it in more detail. This document tries to aim at the newest
version of Rubberband, but the language is not finished yet and it is constantly
changing, so, at the moment you're reading this, the document may be outdated
when compared to the actual implementation -- so beware.

# Building

The current project consists of the core library `librubberband` and the
reference interpreter `rbbs` (stands for **r**ub**b**er**b**and **s**cript).
Throughout the entirety of this manual, it is assumed that all examples are ran
as scripts from the `rbbs` program.

## Dependencies

Currently, the dependencies to build Rubberband are:

- CMake (>= 2.8)
- Make
- g++ and gcc (>= 4.8) or clang (>= 3.3)

No extra dependencies are needed. Note that it's possible that it works in a
different configuration -- e.g., an older version of clang or gcc --, but it has
never been tested.

## Compiling rbbs

If you just want to build and run `rbbs`, from inside the `src` subdirectory of
the repository, run:

    cmake . && make

The `rbbs` executable will be found at the `rbbs` subdir and then you can just
use it, passing the .rbb script as an argument. You can find examples in the
`codeexamples` directory in the repository -- there are two at the moment of
this writing.

For more info on how to use CMake, check out its own documentation.

## Running tests

You can also run the tests of the `librubberband` library. There's a
`runtests.sh` script inside the `src/test` dir, which can be run from any fairly
recent version of Bash. The following variables are used:

- `CXX`: The c++ compiler
- `CXX_FLAGS` (default: `"-g -O0"` when CXX is either g++ or clang++): Flags
passed to the compiler 
- `THREADS` (default: 1): Number of threads of the compilation process
- `TESTS` (default: all): Tests which will be run
- `RUBBERBAND_SRC` (default: `$PWD/../core`): Directory of rubberband's core
library source code

# Language Overview

This section will give an overview of the goals and the characteristics of
Rubberband.

## Design Goals

The Rubberband programming language is an experimental programming language
created with the main objective of being as customizable by the developer as
possible. Currently, the main design goals of Rubberband are:

 - **Core minimalism**: having _just a few basic elements_ instead of putting many
 features inside the core of the language makes it easier to build its features
 -- and the ones needed by the developer -- from the ground up, avoiding
 the redundancy of core language constructs.
 - **Orthogonality**: to provide the developer the flexibility for them to
 customize the language, it needs _allow composition_ between most elements of
 the language _in a way that makes sense_.
 - **Explicitness**: hidden or implicit elements or mechanisms in the language
 creates elements which, being out of reach from the runtime, cannot be
 directly customized -- thus, _explicitness increases the customizability of
 the language_.
 - **Conciseness**: although making a language concise solely to save keystrokes
 is considered a bad idea, since it may hurt readability, too much verbosity
 may also make the code hard to follow. Since the language is supposed to
 be explicit, many basic expressions will be visible in the code -- _making all
 these expressions too long leads to verbose code_, which can sacrifice
 readability more than conciseness. Aside from that, avoiding to use reserved
 words like "if", "for", "while" and others in favor of symbols like "$", "@",
 "~", etc. leaves room for the developer use the former for their own purposes.
 - **Freedom (and responsibility) to the developer**: the core Rubberband
 language intends to define as little policies and follow as little conventions
 as possible, because one of its goals is to _give the developer the ability to
 define their own conventions and policies_. This comes from the idea defining
 coding style, conventions, the design patterns in use and other kinds of
 project policies is a job of the _framework_ not one of the language, which
 should only be concerned in providing the constructs necessary to express
 those policies. That's why **minimalism** is also one of its core design
 goals: the core language is intended to provide a flexible foundation from
 which developers can develop their own frameworks, and having core features
 beyond the very basic will interfere with that.
 
There can be many more considerations on the design goals of Rubberband, but
those described above are enough to give an idea of why the language is like
it is.

## Characteristics

Now that the design goals have been set, the actual characteristics of the
latest implementation will be described. Rubberband is:

 - **Purely object-oriented**: object-orientation is a concept which needs no
 introduction. However, _pure_ in this context means that every value in the
 language is an object. There are some languages in which this isn't the case,
 i.e. not all values are objects -- namely, _C++_ and _Java_. That's why this
 distinction is made.
 - **Message-based**: unlike many object-oriented languages, though --
 languages like _C++_, _Java_, etc. --, Rubberband is based on _message passing_
 instead of function calling. This makes it more similar to _Smalltalk_ and
 other languages which inherited this characteristic from it, like _Object C_
 and _Ruby_, for instance. But, unlike Smalltalk, messages do not have
 arguments attached to them in Rubberband. More on that later.
 - **Classless**: One more thing that sets Rubberband apart from Smalltalk,
 though, is that it doesn't have classes -- at least, not as a concept built in
 the language. Most object-oriented languages implement a class vs instance or
 object duality, that is, every object is necessarily an instance of a class --
 which may or may not be an object itself, depending on the language. In
 Rubberband, though, objects are standalone and don't need a class to exist --
 although the programmer can inquire on what _type_ of object it's dealing
 with. In that regard, Rubberband is closer to languages like _Self_, _Lua_,
 _Javascript_, _Io_, etc.. Languages like those are also referred to as
 _prototype-based_ languages.
 - **Reflexive**: All of the basic objects of the language offer reflection
 capabilities which enable the developer to query metainformation about an
 object; they can query the type of the object and if a given message is valid.
 - **Duck-typed**: Duck typing is a concept common in dynamic languages
 like _Python_ and _Ruby_. Like in those languages, in Rubberband, if an object
 behaves like you want it to in the context you're using, then, for all
 practical intents, it's the object you want. Without duck typing, the
 programmer is required to check if the type of the object is the right type.
 
# Language Presentation

The previous section only passed by the features of the Rubberband, comparing
them with similar features in other languages. This section will present the
language and show its features on their own.

## Objects

As seen before, Rubberband is a purely object-oriented, message-based,
classless, reflexive, duck typed, experimental programming language. As a
purely object-oriented language, the _object_ is everywhere in the program.
The basic objects present in the language are:

 - **Numbers**: `10`, `200.555`, `-3`, `0`
 - **Symbols**, used to name variables, functions, operations, fields, etc..
 Basically, identifiers: `a`, `while`, `+`, etc.
 - **The empty object**, denoted `()`, used to indicate the absence of a value
 - **Booleans**: `?1` and `?0` (true and false, respectively)
 - **Arrays**. They start with `|`, are separated by commas and accept any
 object as an element -- but are not resizable: `|10, 20, 30`,
 `|a, 10, (|10, 20, 30), ?1`
 - **Tables**, which map symbols to functions. They start with `:`, followed
 by the symbol-object pairs, separated by commas: `:a -> 10, b -> 20, c -> 30`,
 `:yes -> ?1, twelve -> 12`,
 `:list -> (|2, 3, 5, 7), table -> (:happy -> ?1, age -> 25)`
 - **Blocks** of code. They are first-class citizens in Rubberband, so, just
 like arrays or a numbers, they can be put inside a table, an array, etc..
 They're enclosed between curly braces, whithin which the code is defined;
 currently, the code contained inside a Rubberband script file defines a block
 object, the only difference being the lack of curly braces. More details on
 blocks will be given in the following section.

## Messages and answers

Rubberband is a message-based message. This means that the programmer gets
information and modifies the state of an object by sending messages to it.
A message to an object can be any other object; to send a message, the
programmer just needs to put the message at the side of the object.

For example, suppose you store an array in a variable called `arr` like this:

    ~:arr -> |10, 20, 30, 40

Arrays are zero-indexed in rubberband; so, to get the third element in the
array -- which is 30 --, you would send the number `2` to the variable `arr`:

    ~:third_element -> ~arr 2

In the above example, the message `2` is being sent to the variable `arr`.
As a reaction to receiving this message, the array gives an _answer_, which is,
in this case, the third element of the array. There are two ways in which an
object can react to a message: it either gives an answer or it causes a runtime
error if the object doesn't respond to that message. For example, if you sent
`4` instead of `2` to `arr`, since that index is out of range, that would
generate a runtime error:

    terminate called after throwing an instance of 'rbb::in_statement_error'
      what(): In statement "~ arr 4": When sending message 4 to object (|10, 20, 30, 40): Message not recognized

Of course, to send a second message to the answer of a message send, just place
the second message at the right side of the first one, and the same for any
subsequent messages. For example:

    ~:nested -> 1, (|2, 3, (|4, 5, 6, (|7, 8, 9, 10, 11), 12), 13), 14
    ~:number -> ~nested 1 2 3 4

Let's interpret the above expression step by step: first, the number `1` is sent
to `nested` -- the answer is the second element of the array, that is,
`|2, 3, (|4, 5, 6, (|7, 8, 9, 10, 11), 12), 13`. Then, to that object, the
number `2` is sent, to which the third element -- that is,
`|4, 5, 6, (|7, 8, 9, 10, 11), 12` -- is given as the answer. From that, the
array `|7, 8, 9, 10, 11` is given as an answer to the message `3`, to which,
finally, the message `4` is sent, giving is the value of the full expression:
`11`, the fifth element of the array.

The message send is the only operation in Rubberband. Anything -- arithmetic,
control flow, accessing array elements, and other operations -- is done by
sending messages to objects. Here are some examples on how that works.

### Variable assignment and access

The message send is the only operation in the language, so, even when the
programming is, say, creating or accessing the value of a variable, they are
sending a message to an object.

You might have noticed that, in the previous examples, the syntax of the
declaration of the `arr` and `third_element` variables looks similar to the one
of tables presented in the previous section. That's not a coincidence: to
create a variable or modify the value of an existing one, you send a table
containing all the variables you want to declare or change to the current
context, which is denoted by `~`. Consider the following example:

    ~:pi -> 3.14159
    ~:circumference -> ~pi * 2 * 1.5

In the first line, you're simply sending `:pi -> 3.14159` (a table) to `~`,
which is the block's context (or scope) -- and also usually a table. Then, to
access the `pi` variable, as depicted in the second line, you just need to send
the `pi` symbol to the context.

### Arithmetic and Comparison

As well as variables, arithmetic operations are also based in the message send
operation. Here's how arithmetic looks like in Rubberband:

    ~:two_plus_two -> 2 + 2
    ~:my_bmi -> 93 / (1.8 * 1.8)
    ~:overweight -> ~my_bmi > 25

Messages in Rubberband do not have any parameters and any object can only
respond to one message at a time. However, it is possible to implement _curried
functions_ -- that is, functions that return other functions -- to emulate
message parameters the way they work in Smalltalk or Self.

Knowing this, let's interpret the `2 + 2` expression: the `+` message is sent
to the `2` object -- it's important to note that, in Rubberband, `+` is just a
regular symbol, with no difference from a syntactic or semantic standpoint to,
say, `plus` or `my_bmi`. The response to that message is a function that gets
any number sent to it and answers that number plus 2. Then, we send `2` to the
returned function, getting a `4`. For simplicity, from now on, function objects
that are answers of symbol messages will be called _methods_ -- for instance,
the object `2` has the `+` method, which accepts a number as a message.

Note that the operations in Rubberband do not have any precedence rules:
they're simply executed in left-to-right order. For instance, the expression
`1 + 2 * 3 + 4` evaluates to `13`, instead of `11`, as one might expect; to
evaluate it to the latter value, parentheses are needed, like this:
`1 + (2 * 3) + 4`.

Arithmetic comparison operations -- less than, greater than, etc. -- are also
methods of any number, going by the names of `<`, `>`, etc.. The difference is
that they result in a boolean object; in the example, the `overweight` variable
will be assigned `?1`, that is, true.

### Control Flow

There are no special keywords to handle control flow in the core language:
control flow, just like other operations, is also done by sending messages --
this time, to the boolean objects. For example, following the example from the
previous subsection:

    ~overweight?~ {
      %inspect_object i_am_overweight
    } {
      %inspect_object i_am_fine
    }

Any boolean object has the `?` method, which gets the context (this will be
explained later) and two _blocks_ of code: the first one is the block which gets
executed if the boolean is `?1`; the second one, if it is `?0`. In other words,
the `?` method gets the context, the `{ %inspect_object i_am_overweight }`
block, and the `{ %inspect_object i_am_fine }` block (`%inspect_object` prints
out the string representation of an object).

Loops are not present in the core language, but they can be done through
recursion (as we're going to see in the next section) and they're also provided
by the standard library, which will be introduced later.

## Blocks

Blocks are briefly introduced in the beginning of the language introduction
section and used in the previous example. Here, they're going to be explained in
more detail.

_Blocks_ define a sequence of instructions, optionally containing a response
value. Here's an example of a block:

    ~:factorial -> {
      ~:n -> $, self -> @
      
      !~n == 0?~ {
        !1
      } {
        !~n * (~self~(~n - 1))
      }
    }

This block contains a code that calculates the factorial of a number. Of course,
it still doesn't test if the received message is actually a number and not a
symbol or an array, for instance, but this isn't so important right now: as long
as the message is a natural number, it'll work. This example contains all
special symbols that can be used inside a block, which are:

 - `~`: This was already presented and it's the _context_, which is more or less
 like the scope of the code.
 - `$`: The _message_ received by the block. In our example, this would be
 the number for which we want to calculate the factorial.
 - `@`: A reference to the block itself, thus, a _self-reference_.
 - `!`: The exclamation mark defines the beggining of the _response expression_.
 The expression, in this case, is an "if else" statement, which responds
 the response of whichever block runs  -- either the "if" block or the "else"
 one. Here, if the received number is `0`, it responds `1`; otherwise, it
 responds `~n * (~self~(~n - 1))`.

A bit more of detail is needed to understand what a context is. Blocks aren't
actually executable -- at least, not directly. To execute the code inside a
block, you need to _instance_ it. A _block instance_ is bound to an object until
the end of its lifetime -- _that_ object is the context, that is, the object
which is represented by `~` inside the block. To create a new instance, you send
an object which you want to bind the block to -- that is, the context. So, for
instance, to calculate the factorial of some numbers using the code provided by
the block above, we could do the following:

    ~:factorial_instance -> ~factorial(:)
    %inspect_object(~factorial_instance 6)   # 720
    %inspect_object(~factorial_instance 10)  # 3628800
    %inspect_object(~factorial_instance 12)  # 479001600

First, we create a new instance of the `factorial` block bound to a new empty
table (which is used to store `n` inside the block). Then, in the following
lines, we send some numbers as messages to that instance -- which, as it's been
shown, are represented by `$` inside the block. One noteworthy difference
between `~` -- the context -- and `$` -- the message -- is that, while the
message may be different in each execution of the block instance, the context
_is the same object_ in all of its executions; in other words, any changes
to the context are kept in the object after the block instance finishes
executing. This didn't have an effect in the previous example, but consider
this:

    ~:add -> { ~:x -> ~x + $ !~x }
    ~:add_instance -> ~add:x -> 1
    %inspect_object(~add_instance 5)  # 6
    %inspect_object(~add_instance 6)  # 12
    %inspect_object(~add_instance 7)  # 19

`add` gets a number, adds it to the value of `x`, attributes the value to that
same variable and returns it. We, then, create an instance of it bound to the
block `:x -> 1` -- setting the initial value of `x` as `1`. As we call the
instance again and again with new values, `x` keeps growing because the changes
we do to the context in one call -- that is, the incrementation of the value --
are still available in the next calls.

Going back to the factorial example, we can now understand why we have to pass a
context object to the `?` method:

      !~n == 0?~ {
        !1
      } {
        !~n * (~self~(~n - 1))
      }

The two blocks sent to it (`{ !1 }` and `{ !~n * (~self~(~n - 1)) }`) are not
block instances and, thus, aren't bound to any context. That method, then, needs
a context to create an instance from one of those blocks to, then, execute it
properly. Since we're passing `~` as their context, they will run over the same
context of the `factorial` block; this means that will be able to directly
access and modify anything in the parent context -- that's why `n` and `self`
are accessible from inside the second block.

The self-reference, as introduced before, is denoted by `@` -- it was attributed
to the `self` variable for reasons which will be explained shortly. The 
self-reference is used in this case because we need it to do the recursive calls
that are necessary to calculate the factorial of a number.

For example: Suppose we send `2` to a `factorial` instance. `~n == 0` will be
false, so the block `{ !~n * ~self~(~n - 1) }` will be called. This block will
first get the value of `n`, then call itself with `~n - 1` as the message. Note
that we have to call `~self~(~n - 1)`; that's because the self-reference is a
reference to the _block_, not to the _current instance_ of it, so we need to
instance it in order to call it. From this, we recursively calculate the
factorial of `2`.

A final note on why it was necessary to declare the `n` and `self` variables.
That's needed because blocks don't automatically bind to their parent scope, so
if we, instead, did:

    ~:factorial -> {
      !~n == 0?~ {
        !1
      } {
        !$ * (@~(~n - 1))
      }
    }

the code wouldn't work as expected, since the `$`, in this case, would be the
message passed _to the else block_, which is `()`; since `()` has no `*` method,
this would result in a runtime error. Even if the runtime error didn't happen,
`@` is a reference to the else block, not to `factorial`; in this case, the
stopping condition would never be checked and the program would enter in an
endless loop.

## Object Metainformation

Metainformation -- or reflection -- is contextual information about the object
itself. _Any_ object provides metainfo; the methods used to acquired that
information are `<<` and `<<?`. Respectively, they provide information about
what messages an object responds and about the object's type -- or, rather, the
interfaces it conforms to.

### The `<<` Method

The first method, `<<`, can get any object as its message and answers a boolean
-- `?1` if the receiver responds to the message, `?0` otherwise. This is
important because, if an object gets a message it doesn't respond to, it will
raise a runtime error. For example:

    ~:plus_1 -> {
      !$ + 1
    }()

Here, we create a block and instance it in the same expression, then save it in
the `plus_1` variable -- we instance it with `()` because the context is not
needed in this case. `plus_1` obviously expects a number as its message, since
the method `+` is being called with a number. But if we, for instance, call it
with a symbol, like this:

    %inspect_object(~plus_1 five)

the runtime result will be:

    terminate called after throwing an instance of 'rbb::in_statement_error'
      what():  In statement "$ + 1": When sending message + to object five: Message not recognized

A runtime error happened because `five` is a symbol and symbols do not respond
to the `+` method. To prevent this from happening, we can check if the message
has that method using `<<`. This time, we have to set an empty table as the
context, since we're defining a variable inside the block:

    ~:checked_plus_1 -> {
      ~:msg -> $
      
      !~msg << +?~ {
        !~msg + 1
      } {
        !()
      }
    }(:)
    
    %inspect_object(~checked_plus_1 five)
    %inspect_object(~checked_plus_1 10)

Now, if we pass a message that does not have the `+` method -- like `five`, for
example --, `checked_plus_1` will answer `()` instead of raising a runtime
error, not interrupting the execution of the program:

    ()
    11

`<<` can also be used to check if an index is inside the bounds of an array or
if a given symbol is defined in a table:

    %inspect_object((|10, 20, 30) << 3)                 # ?0
    %inspect_object((:a -> 10, b -> 20, c -> 30) << b)  # ?1

### The `<<?` method

The second method, `<<?`, is used to gather information about the type of an
object or about the interfaces it conforms to. When an object conforms to some
interface, it means that it implements some behaviors expected from the objects
which conform to it. For instance, if an object conforms to the `[0]` interface,
it means that it responds properly to all number-related messages -- in other
words, it is a number.

For example, going back to `checked_plus_1`, we can see a flaw in its
implementation:

    %inspect_object(~checked_plus_1|10, 20, 30)
    
Executing it we get this:

    terminate called after throwing an instance of 'rbb::in_statement_error'
      what():  In statement "~ msg + 1": When sending message 1 to object [unknown data]: Array expected

This error happens because, inside `checked_plus_1`, we only check if the
message contains the `+` method. An array, which does contain the `+` method,
will pass that check, but the `+` method of the array does not mean "sum", but
rather "concatenate", and it expects another array, not a number -- which is why
the runtime error is raised.

To avoid that, we can check if the object being received by `checked_plus_1` is
_of the kind_ we want it to be, that is, if it is either a number or a
number-like object. To check that, we use the already mentioned `[0]` interface:

    ~:checked_plus_2 -> {
      ~:msg -> $
      
      !~msg <<? [0]?~ {
        !~msg + 1
      } {
        !()
      }
    }(:)

If we try to do the same as before with the new function:

    %inspect_object(~checked_plus_2|10, 20, 30)

We get what we were expecting -- that is, `()` is printed out.

## Raising Runtime Errors

In some situations, to prevent incorrect behavior, the programmer needs to be
able to raise an error and abort the execution of the program. In Rubberband,
the boolean object provides the `^` method for that. Let's take the factorial
example again:

    ~:factorial -> {
      ~:n -> $, self -> @
      
      !~n == 0?~ {
        !1
      } {
        !~n * (~self~(~n - 1))
      }
    }(:)

That function has a little problem: if it gets a negative number, it will run
forever -- or at least until it segfaults. Replacing the condition by `~n <= 0`
would help make the function not segfault, but then that definition is incorrect
-- the factorial of -1 is not 1; the factorial of -1 _does not exist_.

Raising an error will prevent wrong results from being yielded:

    ~:factorial -> {
      ~:n -> $, self -> @
      ~n < 0^ negative_numbers_dont_have_a_factorial
      
      # ...
    }(:)
    
    %inspect_object(~factorial -1)

You can pass any object to `^`; what's going to be done with it is up to the
implementation. `rbbs` simply doesn't treat it:

    terminate called after throwing an instance of 'rbb::runtime_error'
      what():  Runtime error (error object: negative_numbers_dont_have_a_factorial)

It's important to point out that runtime errors are _not_ exceptions. You can't
catch them in any way (they simply abort the execution of the program), and,
although you can attach any object you want to them, that object is only ever
going to be used by the implementation, if at all.

# Base Library

So far, the pure base language has been presented. But, since it lacks things
like a proper loop control structure, proper function argument handling, etc.,
using the core Rubberband language for normal tasks directly can be quite
difficult. But, as mentioned in the introduction, this isn't its purpose either:
the purpose of Rubberband -- aside from being an experimental language -- is
providing the most basic tools necessary to enable the creation of useful
constructs, libraries, frameworks, etc., which will, _then_, be used to peform
normal, daily tasks.

To exemplify that, Rubberband has its own base library implemented completely in
Rubberband. It features a limited set of functionalities, but it shows a bit of
what can be done with Rubberband currently.

The base library is located at the `src/mod` directory in the `rubberband`
repository. To use it, add that directory to the module lookup path with the
`-p` switch of `rbbs` and load it with `-m base` (`--help` will give you more
detailed information on command-line switches for `rbbs`).

## `while`

The `while` function implements a conventional while loop, preventing you to
mess with blocks and recursion by hand. The following example calculates the
closest power of two smaller than the number 999999:

    ~:c -> 1

    ~while~ { !~c * 2 < (~n) } {
      ~:c -> ~c * 2
    } 

    %inspect_object(~c)

You may have noticed that there's a second `~` after the call to `while`. That's
because `while` needs to pass a context to both blocks it receives so that the
programmer is able to properly use the variables they declare outside those
blocks. The context can be any object, but, usually, it will be the current
context, that is, `~`. This pattern will be present in all functions from the
base library presented here.

As for the actual usage of `while`, two arguments are passed aside from the
context: a block containing a condition expression and an execution block. The
second block will be repeatedly executed until the first block answers `?0`. In
the example above, the number `524288` is printed.

## `fn`

A "gotcha" mentioned before in the blocks section also applies in the case
presented here. If you try to, say, put the code from before into a function and
use raw blocks to do that, you have to be careful:

    ~:pow2_less_than -> {
      ~:c -> 1

      ~while~ { !~c * 2 < $ } {
        ~:c -> ~c * 2
      }

      !~c
    }(:)

    %inspect_object(~pow2_less_than 999999)

Couple of problems with this implementation: first, since `pow2_less_than` is a
block instance bound to an empty table, `while` is not available inside that
block, which gives us an error:

    terminate called after throwing an instance of 'rbb::in_statement_error'
      what():  In statement "~ while ~ { !~ c * 2 < $ } { ~ (:c -> ~ c * 2) }": When sending message while to object (:c -> 1): Message not recognized

We could try to "fix" it by binding the instance to `~` instead, but that would
make `c` leak to the main context, which is undesirable most of the time. The
best simple solution would be to create a copy of the context (by binding the
instance to `~  + (:)`), but other problems would arise from that (needlessly
copying every single thing, not being able to modify external objects, etc.).
And even then, that code would be wrong, because calls the blocks it receives
with `()` as the message, making the condition block `{ !~c * 2 < $ }` return
`?0` anytime it's called.

To avoid dealing with all of that manually, the standard library has a `fn`
function. Its purpose is to create a function that behaves more like a regular
function from other languages, in that:

1. You can pass it a list of arguments and not just one argument
  - All arguments you pass to it are already named (so no more fiddling with
     `$`)
2. Objects declared outside it can be accessed inside the function (and
   modified, if necessary)
3. Objects declared inside it are automatically destroyed after the function
   finishes
  - If an object declared inside a function has the same name as one outside
     it, the former elides the latter

For example, our function would be written as this:

    ~:pow2_less_than -> ~fn~ (|n) {
      ~:c -> 1

      ~while~ { !~c * 2 < (~n) } {
        ~:c -> ~c * 2
      }

      !~c
    }

Much simpler and less worrysome. The two arguments `fn` gets are a list
containing the names of the arguments and the function block. To call it, pass
the argument as a message:

    %inspect_object(~pow2_less_than 999999)

This will print, again, `524288`.

Functions with more arguments are called in a similar way: send the arguments as
messages sequentially:

    ~:avg3 -> ~fn~ (|num1, num2, num3) {
      !(~num1) + (~num2) + (~num3) / 3
    } 

    %inspect_object(~avg3 100 200 250)

Prints `183`.
