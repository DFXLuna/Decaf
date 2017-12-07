- If there are multiple definitions of main, the program will give errors
but still add them to the table.

- When the program can't add a method to the symbol table, it still creates
a table for the method's scope and typechecks that.