#include<string>
using std::string;

// It might be worth nesting the classes

class Table;
class TypeInst;
class TypeDecl;


class TableManager{
public:
    TableManager();
    ~TableManager();
    bool addTable();
    void exitScope();
    bool addEntry( string type, string name );

    // Takes an empty TypeInst reference
    // Populates it and returns true if found or false if not
    bool tryLookup( string name, TypeDecl* ret );

    // These control the global type table
    bool createTypeTable();
    bool addType();
private:
    Table* typeTable;
    Table* declTables;
    Table* currTable;
};

class Table{
public:
    ~Table();
    bool tryLookup( string name, TypeDecl* ret );
    bool addEntry(/* STUFF NEEDED FOR TypeInst CREATION*/);
private:
    Table* parent;
};

// This covers instances of a type
class TypeInst {
public:
    TypeInst( TypeDecl* type, string name );
private:
    // Var name
    string name;
    // Pointer to entry in typetable
    TypeDecl* type;
};

// This covers the declaration of a type
// Allows implicit forward declaration so that all types will be in type table
// before they are actually processed.
class TypeDecl {
    public:
    TypeDecl( string name, int width = 0, forwardDecl = true );

    string getName();
    int getWidth();
    bool isForward();
private:
    string name;
    int width;
    bool forwardDecl;
}