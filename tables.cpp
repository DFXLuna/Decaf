#include "tables.h"

TableManager::TableManager(){
    globalTypeTable = 0;
    currTable = 0;
    createGlobalTypeTable();
}

TableManager::~TableManager(){}
void TableManager::enterScope(){
    Table* newScope = new Table( currTable );
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
        if(currTable->tryAddEntry(name, t)){
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
    if(!tryLookup(returnType, ret)){
        return false;
    }

    // Let table handle adding
    if(currTable->tryAddEntry(name, args, ret)){
        return true;
    }

    return false;
}

bool TableManager::tryLookup( string name, TypeDecl* result ){
    if(globalTypeTable->tryLookup(name, result)){
        return true;
    }
    return false;
}

bool TableManager::tryLookup( string name, MethDecl* result ){
    if(currTable != 0){
        if(currTable->tryLookup(name, result)){
            return true;
        }
    }
    return false;
}

bool TableManager::tryLookup( string name, TypeInst* result ){
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

bool TableManager::forwardEntryGlobalTypeTable( string name, TypeDecl* t ){
    if(globalTypeTable->tryAddEntry(name, t)){
        return true;
    }
    return false;
}

bool TableManager::setWidthGlobalTypeTable( string name, int width ){
    TypeDecl* result = 0;
    if(globalTypeTable->tryLookup(name, result)){
        result->setWidth(width);
        return true;
    }
    return false;    
}
/////////////////////////////////////////

bool GlobalTypeTable::tryAddEntry( string typeName, TypeDecl* result ){
    return false;
}

bool GlobalTypeTable::tryLookup( string typeName, TypeDecl* result ){
    return false;
}

/////////////////////////////////////////
Table::Table( Table* parent ){

}

bool Table::tryLookup( string name, TypeDecl* result ){
    return false;
}

bool Table::tryLookup( string name, MethDecl* result ){
    return false;
}

bool Table::tryLookup( string name, TypeInst* result){
    return false;
}

bool Table::tryAddEntry( string varName, TypeDecl* type ){
    return false;
}

bool Table::tryAddEntry( string methName, vector<TypeDecl*> argTypes,
TypeDecl* returnType ){
    return false;
}

Table* Table::getParent(){
    return parent;
}

/////////////////////////////////////////

TypeInst::TypeInst( string name, TypeDecl* type ){
    this->name = name;
    this->type = type;
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

void TypeDecl::print(){
     cout << name;
}

/////////////////////////////////////////

MethDecl::MethDecl( string name, vector<TypeDecl*>& argTypes, string retType, bool forwardDecl ){
    this->name = name;
    this->argTypes = argTypes;
    this->forwardDecl = forwardDecl;
    this->retType = retType;
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
    cout << retType;
}