#include<string>
using std::string;
#include<map>
using std::map;
#include<vector>
using std::vector;

// It might be worth nesting the classes

class Table;
class TypeInst;
class TypeDecl;
class TypeGen;


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
    Table( Table* parent );
    ~Table();
    // If successful, result points to an entry in the global type table and
    // returns true
    bool tryLookup( string name, TypeDecl* result );
    // If successful, result points to an entry in the tree of method
    // declarations and returns true
    bool tryLookup( string name, MethDecl* result );
    // Add variable to typetable
    bool tryAddEntry( string varName, string typeName );
    // Add method declaration to type table
    bool tryAddEntry( string methName, vector<string> argTypes );
private:
    Table* parent;
    map<string, TypeInst> typeTable;
    map<string, MethDecl> methTable;
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
    void print(){
        cout << name;
    }
private:
    string name;
    int width;
    bool forwardDecl;
}

// This covers the declaration of a method,
// methods get their own separate table, these also allow forward declarations
// for type checking, meth instances don't make sense
class MethDecl {
public:
    // vector passed by referenced because I want to avoid making two copies
    MethDecl( string name, vector<TypeDecl*>& argTypes, string retType, forwardDecl = true );
    bool isForward();
    vector<TypeDecl*> getArgTypes();
    void print(){
        cout << name << ": ";
        if(argTypes.size() == 0){
            cout << "() -> "
        }
        else{
            for(int i = 0; i < argTypes.size(); i++){
                argTypes[i]->print();
                cout << " -> ";
            }
        }
        cout << retType;
    }
private:
    string name;
    vector<TypeDecl*> argTypes;
    string retType;
    bool forwardDecl;
}