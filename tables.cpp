#include "tables.h"

TableManager::TableManager(){
    globalTypeTable = 0;
    currTable = 0;
    createGlobalTypeTable();

    // Add int and void to table
    voidType = new TypeDecl("void", 0, false);
    forwardEntryGlobalTypeTable("int");
    resolveForwardGlobalTypeTable("int", 4);
    
}

TableManager::~TableManager(){
    // Return to highest scope and then deallocate entire tree
    while(currTable->getParent() != 0 ){
        exitScope();
    }
    delete currTable;
    delete voidType;
}

void TableManager::enterScope(){
    Table* newScope = new Table( currTable );
    if(currTable != 0 ){
        currTable->registerChild( newScope );
    }
    currTable = newScope;
}

void TableManager::exitScope(){
    if(currTable != 0){
        currTable = currTable->getParent();
    }
}

bool TableManager::addTypeInst( string type, string name ){
    // First check to see if type is valid
    TypeDecl* t = 0;
    if(tryLookup(type, t)){
        // Let table handle adding it
        TypeInst ti( name, t );
        if(currTable->tryAddEntry(name, ti)){
            return true;
        }
    }
    return false;
}

bool TableManager::addMethDecl( string name, vector<string> argTypes,
string returnType ){
    // Get all arg types
    vector<TypeDecl*> args;
    TypeDecl* temp;
    for(unsigned int i = 0; i < argTypes.size(); i++){
        if(tryLookup(argTypes[i], temp)){
            args.push_back(temp);
        }
        else{
            return false;
        }
    }
    // Lookup return type
    TypeDecl* ret = 0;
    if(returnType == "void"){
        ret = voidType;
    }
    else if(!tryLookup(returnType, ret)){
        return false;
    }

    // Let table handle adding
    if(currTable->tryAddEntry(name, args, ret, false)){
        return true;
    }

    return false;
}

bool TableManager::tryLookup( string name, TypeDecl*& result ){
    if(globalTypeTable->tryLookup(name, result)){
        return true;
    }
    return false;
}

bool TableManager::tryLookup( string name, MethDecl*& result ){
    if(currTable != 0){
        if(currTable->tryLookup(name, result)){
            return true;
        }
    }
    return false;
}

bool TableManager::tryLookup( string name, TypeInst*& result ){
    if(currTable != 0){
        if(currTable->tryLookup(name, result)){
            return true;
        }
    }
    return false;
}


bool TableManager::createGlobalTypeTable(){
    // Creates a table with int as the only entry
    if( globalTypeTable == 0 ){
        globalTypeTable = new GlobalTypeTable();
        return true;
    }
    return false;
}

bool TableManager::forwardEntryGlobalTypeTable( string name ){
    TypeDecl temp(name, 0, true);
    if(globalTypeTable->tryAddEntry(name, temp)){
        return true;
    }
    return false;
}

bool TableManager::resolveForwardGlobalTypeTable( string name, int width ){
    TypeDecl* result = 0;
    if(globalTypeTable->tryLookup(name, result) && result->isForward()){
        result->setWidth(width);
        result->resolveForward();
        return true;
    }
    return false;    
}

void TableManager::dump(){
    if( globalTypeTable != 0 ){
        globalTypeTable->print();
    }
    // Save current scope to return to it after printing
    if( currTable != 0 ){
        Table* currScope = currTable;
        while( currTable->getParent() != 0 ){
            currTable = currTable->getParent();
        }
        // Dump the entire table
        currTable->print();

        // Return to scope
        currTable = currScope;
    }
}
/////////////////////////////////////////

bool GlobalTypeTable::tryAddEntry( string typeName, TypeDecl t ){
    // Can be used later for more verbose errors
    TypeDecl* temp = 0;
    if(tryLookup(typeName, temp)){
        return false;
    }

    types[typeName] = t;
    return true;
}

bool GlobalTypeTable::tryLookup( string typeName, TypeDecl*& result ){
    // Newer c++ standards may have a lot of garbage in them but at least
    // They give you ways to eal with this trash.
    map<string, TypeDecl>::iterator it;
    if( (it = types.find(typeName)) != types.end() ){
        result = &(it->second);
        return true;
    }
    return false;
}

void GlobalTypeTable::print(){
    cout << "Global Type Table" << endl;
    for(map<string, TypeDecl>::iterator it = types.begin(); it != types.end(); it++){
        it->second.print();
        cout << endl;
    }
    cout << endl << endl;
}

/////////////////////////////////////////
Table::Table( Table* parent ){
    this->parent = parent;
}

Table::~Table(){
    for(unsigned int i = 0; i < children.size(); i++){
        delete children[i];
    }
}

bool Table::tryLookup( string name, MethDecl*& result ){
    // Check local table
    map<string, MethDecl>::iterator it;
    if( (it = methTable.find(name)) != methTable.end() ){
        result = &(it->second);
        return true;
    }
    // Check parents tables
    if(parent && parent->tryLookup(name, result)){
        return true;
    }

    return false;
}

bool Table::tryLookup( string name, TypeInst*& result ){
    map<string, TypeInst>::iterator it;
    if( (it = typeTable.find(name)) != typeTable.end() ){
        result = &(it->second);
        return true;
    }
    // check parent tables
    if(parent && parent->tryLookup(name, result)){
        return true;
    }
    return false;
}

bool Table::tryAddEntry( string varName, TypeInst t ){
    TypeInst* temp = 0;
    if(tryLookup(varName, temp)){
        return false;
    }
    typeTable[varName] = t;
    return true;
}

bool Table::tryAddEntry( string methName, vector<TypeDecl*> argTypes,
TypeDecl* returnType, bool forwardDecl ){
    MethDecl* lookup = 0;
    MethDecl toInsert(methName, argTypes, returnType, forwardDecl);
    if(!tryLookup(methName, lookup)){
        methTable[methName] = toInsert;
        return true;
    }
    else{
        if(lookup->isForward() && *lookup == toInsert){
            // Resolve forwardDecl
            lookup->resolveForward();
            return true;
        }
        else{
            return false;
        }
    }
    // Something strange happened
    cout << "Hope you got insurance" << endl;
    return false;
}

Table* Table::getParent(){
    return parent;
}

void Table::registerChild( Table* c ){
    children.push_back(c);
}

void Table::print(){
    cout << "Table" << endl;
    for( map<string, TypeInst>::iterator it = typeTable.begin();
    it != typeTable.end(); it++){
        it->second.print();
    }
    for( map<string, MethDecl>::iterator it = methTable.begin();
    it != methTable.end(); it++){
        it->second.print();
    }
    cout << endl;

    // Print children
    for(unsigned int i = 0; i < children.size(); i++){
        children[i]->print();
    }


}

/////////////////////////////////////////

TypeInst::TypeInst(){
    type = 0;
}

TypeInst::TypeInst( string name, TypeDecl* type ){
    this->name = name;
    this->type = type;
}

void TypeInst::print(){
    type->print();
    cout << ": " << name << endl;
}

string TypeInst::getName(){
    return name;
}

/////////////////////////////////////////

TypeDecl::TypeDecl(){
    width = 0;
    forwardDecl = true;
}

TypeDecl::TypeDecl( string name, int width, bool forwardDecl ){
    this->name = name;
    this->width = width;
    this->forwardDecl = forwardDecl;
}

string TypeDecl::getName(){
    return name;
}

void TypeDecl::setWidth( int width ){
    this->width = width;
}

int TypeDecl::getWidth(){
    return width;
}

bool TypeDecl::isForward(){
    return forwardDecl;
}

void TypeDecl::resolveForward(){
    forwardDecl = false;
}

void TypeDecl::print(){
     cout << name;
}

/////////////////////////////////////////

MethDecl::MethDecl(){
    retType = 0;
    forwardDecl = true;
}

MethDecl::MethDecl( string name, vector<TypeDecl*> argTypes, 
TypeDecl* retType, bool forwardDecl ){
    this->name = name;
    this->argTypes = argTypes;
    this->forwardDecl = forwardDecl;
    this->retType = retType;
}

void MethDecl::resolveForward(){
    forwardDecl = false;
}

bool MethDecl::isForward(){
    return forwardDecl;
}

vector<TypeDecl*> MethDecl::getArgTypes(){
    return argTypes;
}

void MethDecl::print(){
    cout << name << ": ";
    if(argTypes.size() == 0){
        cout << "() -> ";
    }
    else{
        for(unsigned int i = 0; i < argTypes.size(); i++){
            argTypes[i]->print();
            cout << " -> ";
        }
    }
    retType->print();
    cout << endl;
}

// this is used for the purpose of setting forwardDecl so comparing them
// doesn't make sense
bool MethDecl::operator==(const MethDecl& rhs) const{
    if(this->name == rhs.name &&
       this->argTypes == rhs.argTypes &&
       this->retType == rhs.retType){
           return true;
       }
       return false;
}