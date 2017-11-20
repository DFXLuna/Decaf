- method printing uses haskell-like syntax (I'm sorry) so it's of the form
 "name: argType1 -> argType2 -> argTypex -> returnType"
 or
 "name: () -> returnType" for functions with no parameters

- Method and constructor overloading isn't currently supported yet.

- This uses the C style of true and false

- During the table dump, constructors are listed just like methods
and they list a return type of void.

- Arrays on types are stored as types, so they show up in the global type 
table.

- There's a strange error where an assignment to an element in an array 
fails if the statement is the first statement in a block. so
IntArray[1] = 1; only works if it is preceded by another statement in the 
block. This only affects arrays.

- Error messages are vague and generally unhelpful.