- method printing uses haskell-like syntax (I'm sorry) so it's of the form
 "name: argType1 -> argType2 -> argTypex -> returnType"
 or
 "name: () -> returnType" for functions with no parameters

- Method and constructor overloading isn't currently supported yet.

- During the table dump, constructors are listed just like methods
and they list a return type of void.

- Arrays on types are stored as types, so they show up in the global type 
table.

- I wasn't paying very close attention to where I output error messages so 
one error in the program may cause up to 5 - 10 error messages to appear.