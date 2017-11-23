- During table dump, the type table is printed, then the symbol tables are 
printed in the format: 
scope
Local vars
methods
child tables
So a class table is like
Global::ClassName
Type: VarA;
MethA: arg1 -> returnType
    Global::ClassName::MethA
...


- method printing uses haskell-like syntax (I'm sorry) so it's of the form
 "name: argType1 -> argType2 -> argTypex -> returnType"
 or
 "name: () -> returnType" for functions with no parameters

- During the table dump, constructors are listed just like methods
and they list a return type of void.

- Arrays on types are stored as types, so they show up in the global type 
table.

- Method and constructor overloading isn't currently supported yet.

- There's a strange error where an assignment to an element in an array 
fails if the statement is the first statement in a block. so
"IntArray[1] = 1;" 
only works if it's not the first statement in a block.

- Error messages are vague and generally unhelpful.

- The grammar for function calls as statements is just plain weird. Following
the grammar exactly allows a function call to look like this:
"SomeArray[1]();" as well as this:
"this();"
Both of which are strange. They are treated as errors by the typechecker.

- There isn't currently a way for the program to verify if a return type
matches the return expression or if there even is a return expression.