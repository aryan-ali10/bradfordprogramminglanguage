# What

This is a programming language created using slang from Bradford, a city in the UK that is notorious for what many people would consider the worst accent in the country(though I heavily disagree... ***cough*** ***cough*** East London).

This is an interpreter (not a compiler) so no machine code or executable is produced. Running the files should lex, parse and execute the program in one step. It is similar to Python in the sense that it creates tokens and then parses them into a parse tree and then walks the tree to run it.

# Why

The main reason for this project is to help me get more comfortable with using C++. This is my first open source C++ project. It is also a farwell homage to Bradford, a temporary final gift before moving out to go to University.

# The Language
Every key word is a slang word taken from regular vocabulary found in common phrases.

| Slang | Meaning | Use |
|---------|---------|-----|
| wagwan cuz wys | A common phrase used to greet someone, similar to "What's going on bro?". Literally means "What's going on cousin, what're you saying?". | Used to start a function definition. |
| send it | Typically used when driving to suggest to the driver to speed up; similar to "floor it". | Used to return a value from a function and also doubles as the assignment operator. |
| innit yara | Typically used interchangeably, but occasionally together. "innit" is short for "isn't it", and "yara" (sometimes "yaar") means friend. Stereotypically known for being used at the end of every sentence. | Used similarly to a semicolon (ends a statement). |
| rs3 | Refers to the Audi RS3, the stereotypical dream car of someone from Bradford (as someone from Bradford I have to admit I prefer an M3). | Used to declare an `if` statement. |
| m140i | Refers to the BMW M140i, another stereotype about cars in Bradford. | Used to declare an `else` statement. |
| rev it | Exactly what it means | Used to declare a `while` loop |
| kasme | Originally used as a way of expressing a promise in Urdu, but in Bradford has turned into a way of saying "I swear", even when telling a lie. | Used to close a `while` block. |
| numba | Just the way that people from Yorkshire tend to pronounce "number". | Used to declare an `integer` type. |
| charva | Similar to the word "chav". Can be used to greet someone, e.g. "Yes charva". |Used to declare a `string` type|
| wallahi | Literally means "I swear to God". Originally used only for the most serious promises, but now commonly used in trivial situations. | Used to express the boolean value `True`. |
| badtameez | A way to describe someone who is poorly behaved. | Used to express the boolean value `False`. |
| benchod | An extremely common insult used in Bradford. You can search up what it means. | Used to express `<`. |
| benchod= | An extremely common insult used in Bradford. You can search up what it means. | Used to express `<=`. |
| vicked | The way "wicked" is commonly pronounced in Bradford. An extremely common word used to describe something good. | Used to express `>`. |
| vicked= | The way "wicked" is commonly pronounced in Bradford. An extremely common word used to describe something good. | Used to express `>=`. |
| lala | A way of saying "bro" is the best way I can describe it. | Used to express the boolean operator `NOT`. |
| geezer | A geezer is just a way to say 'guy' | Used to express the boolean operator `AND`. |
| top man | Calling someone a 'top man' is a way of complimenting someone and is usually used after someone does a favour for you. | Used to express the boolean operator `OR`. |
| munch | A 'munch' is just something to eat. A common phrase would be "grab a quick munch". Equivalent to saying "Grab something to eat" | Used to `print`. |
| mush | Used as a way of greeting someone. An example of this would be "Alright mush" | Used as a comment marker (anything after this is ignored) |
| gora | A term used to describe someone white| Used to declare a variable |
| + - * / | | Arithmetic operators remain unchanged |


# Goals
- Right now, just to make it work!

- Once it works polish it and optimise the hell out of it

- When it works, maybe use Emscripten to compile to WebAssembly so I can run it on the web.

# Changelog

21/06/2026 v0.01

I took far too long trying to decide the syntax and just decided to wing it and change it as I go if I need to... (hope I don't regret it later) My initial commit shows my initial plan for the syntax. 

21/06/2026 v0.02

I've completed the first version of the lexer. I've also included `benchod=` and `vicked=` in the README.md file as I had forgotten to add it before. I know its inefficient. I'm just trying to get it to work first with the limited C++ I know. I will optimise it though!

22/06/2026 v0.03
I've started to implement the parser. It is a little difficult to understand but who doesn't like a challenge! I've also updated the README to include `gora` which I had also forgotten before.

22/06/2026 v.0.04
I've completed the first version of the parser. I'm going to start working on the actual interpreter now! 

23/06/2026 v.0.05
I've finished the first half of the interpreter but still need to fully finish callFunction to handle execution blocks, proper argument mapping, and ReturnSignal exception unwinding for functions. It's a bit of a puzzle handling the scoped environments, but it's coming together!








