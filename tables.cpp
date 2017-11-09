#include "tables.h"

TableManager::TableManager(){}
TableManager::~TableManager(){}
bool TableManager::addTable(){
    return false;
}
void TableManager::exitScope(){}
bool TableManager::addEntry(){
    return false;
}
bool TableManager::tryLookup( TypeInfo& in ){
    return false;
}

/////////////////////////////////////////

Table::~Table(){}
bool Table::tryLookup( TypeInfo& in ){
    return false;
}
bool Table::addEntry(){
    return false;
}

/////////////////////////////////////////
TypeInfo::TypeInfo(){}
TypeInfo::~TypeInfo(){}