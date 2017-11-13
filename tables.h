#include<string>
using std::string;
#include<map>
using std::map;
#include<vector>
using std::vector;
#include<iostream>
using std::cout;
using std::endl;

class Table;
class GlobalTypeTable;
class TypeInst;
class TypeDecl;
class MethDecl;

// Most of the arguments should probably be by reference to prevent
// unnecessary copying

class TableManager{
public:
    TableManager();
    ~TableManager();
    void enterScope();
    void exitScope();

    bool addTypeInst( string type, string name );
    bool addMethDecl( string name, vector<string> argTypes, 
                      string returnType );


    // If successful, result points to an entry in the global type table and
    // returns true
    bool tryLookup( string name, TypeDecl*& result );
    
    // If successful, result points to an entry in the type instance tree  and
    // returns true
    bool tryLookup( string name, TypeInst* result );
    
    // If successful, result points to an entry in the method declaration tree
    // and returns true
    bool tryLookup( string name, MethDecl* result );

    // These control the global type table
    // Declare a type without a width
    bool forwardEntryGlobalTypeTable( string name, TypeDecl*& t );
    // Set width of forward declared entry
    bool setWidthGlobalTypeTable( string name, int width );

private:
    bool createGlobalTypeTable();
    GlobalTypeTable* globalTypeTable;
    Table* currTable;
};

class Table{
public:
    Table( Table* parent );
    // If successful, result points to an entry in the global type table and
    // returns true
    bool tryLookup( string name, TypeDecl* result );
    // If successful, result points to an entry in the type instance tree  and
    // returns true
    bool tryLookup( string name, TypeInst* result );
    // If successful, result points to an entry in the tree of method
    // declarations and returns true
    bool tryLookup( string name, MethDecl* result );
    // Add variable to typetable
    bool tryAddEntry( string varName, TypeDecl* type );
    // Add method declaration to type table
    bool tryAddEntry( string methName, vector<TypeDecl*> argTypes,
                      TypeDecl* returnType );
    
    Table* getParent();
private:
    Table* parent;
    map<string, TypeInst> typeTable;
    map<string, MethDecl> methTable;
};

class GlobalTypeTable {
public:
    bool tryAddEntry( string typeName, TypeDecl* t );
    bool tryLookup( string typeName, TypeDecl*& result );
private:
    map<string, TypeDecl*> types;
};

// This covers instances of a type
class TypeInst {
public:
    TypeInst( string name, TypeDecl* type );
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
    TypeDecl( string name, int width = 0, bool forwardDecl = true );

    string getName();
    int getWidth();
    void setWidth( int );
    bool isForward();
    void print();
private:
    string name;
    int width;
    bool forwardDecl;
};

// This covers the declaration of a method,
// methods get their own separate table, these also allow forward declarations
// for type checking, meth instances don't make sense
class MethDecl {
public:
    // vector passed by referenced because I want to avoid making two copies
    MethDecl( string name, vector<TypeDecl*>& argTypes, string retType, bool forwardDecl = true );
    bool isForward();
    vector<TypeDecl*> getArgTypes();
    void print();
private:
    string name;
    vector<TypeDecl*> argTypes;
    string retType;
    bool forwardDecl;
};