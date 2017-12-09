- If there are multiple definitions of main, the program will give errors
but still add them to the table.

- When the program can't add a method to the symbol table, it still creates
a table for the method's scope and typechecks that.

- array dimensions are not checked at all

- There's stil a strange error where, if an assignment to an array is the 
first statement in a block, it fails.