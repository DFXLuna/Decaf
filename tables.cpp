#include "tables.h"

TableManager::TableManager(){}
TableManager::~TableManager(){}
bool TableManager::addTable(){
    return false;
}

void TableManager::exitScope(){}
bool TableManager::addEntry(){
    // TypeDecl* ret = 0;
    // if(!tm->tryLookup(type, ret)){
    //     return false;
    // }
    // tm->addEntry()
    return false;
}

bool TableManager::tryLookup( TypeInst& in ){
    return false;
}


bool createTypeTable(){
    return false;
}

bool addType(){
    return false;
}
/////////////////////////////////////////

Table::~Table(){}
bool Table::tryLookup( TypeInst& in ){
    return false;
}
bool Table::addEntry(){
    return false;
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

int TypeDecl::getWidth(){
    return width;
}

bool TypeDecl::isForward(){
    return forwardDecl;
}

/////////////////////////////////////////

MethDecl::MethDecl( string name, vector<TypeDecl*>& argTypes, forwardDecl ){
    this->name = name;
    this->argTypes = argTypes
    this->forwardDecl = forwardDecl;
    this->retType = retType;
}

bool MethDecl::isForward(){
    return forwardDecl;
}

vector<TypeDecl*> MethDecl::getArgTypes(){
    return argTypes;
}