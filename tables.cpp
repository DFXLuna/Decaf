#include "tables.h"

TableManager::TableManager(){
    globalTypeTable = 0;
    currTable = 0;
    anonymousBlockNum = 0;
    mainCount = 0;
    createGlobalTypeTable();

    // Add int and void to table
    voidType = new TypeDecl("void", 0, false);
    forwardEntryGlobalTypeTable("int");
    resolveForwardGlobalTypeTable("int", 4);

   // Add the global scope
   enterScope( "Global" ); 
}

TableManager::~TableManager(){
    // Return to highest scope and then deallocate entire tree
    if(currTable != 0){
        while(currTable->getParent() != 0 ){
            exitScope();
        }
        delete currTable;
    }
    delete voidType;
}

void TableManager::enterScope( string name ){
    string scope;
    if( name != "Global" ){
        scope = getCurrentScope() + "::" + name;
    }
    else{
        scope = name;
    }

    Table* newScope = new Table( currTable, scope );
    if( currTable != 0 ){
        currTable->registerChild( newScope );
    }
    currTable = newScope;
}


void TableManager::enterAnonymousScope(){
    string scopeName = getCurrentScope() + "::" + "Block";
    scopeName = appendInt(scopeName, anonymousBlockNum);
    anonymousBlockNum++;
    Table* newScope = new Table(currTable, scopeName);
    if( currTable != 0 ){
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
    if( currTable->tryAddEntry(name, args, ret, false) ){
        return true;
    }

    return false;
}

bool TableManager::tryLookup( string name, TypeDecl*& result ){
    if( name == "void" ){
        result = voidType;
        return true;
    }
    if( globalTypeTable->tryLookup( name, result ) ){
        return true;
    }
    return false;
}

bool TableManager::tryLookup( string name, vector<TypeDecl*> args, MethDecl*& result ){
    if( currTable != 0 ){
        if( currTable->tryLookup( name, args, result ) ){
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

bool TableManager::verifyTypes( vector<string> types ){
    TypeDecl* temp = 0;
    string sub;
    for( unsigned int i = 0; i < types.size(); i++ ){
        // check for array type
        size_t loc = types[i].find("[");
        if( loc == string::npos ){
            // Not array, check normally
            if( !tryLookup(types[i], temp) ){
                return false;
            }
        }
        else{
            // Check substring that is not array
            sub = types[i].substr( 0, loc );
            if( !tryLookup(sub, temp) ){
                return false;
            }
        }
    }
    return true;
}

void TableManager::addTypes( vector<string> types ){
    // This might try to add duplicate entries, but the table 
    // can handle it.
    for(unsigned int i = 0; i < types.size(); i++){
        forwardEntryGlobalTypeTable(types[i]);
    }
}

string TableManager::getCurrentScope(){
    if(currTable != 0){
        return currTable->getName();
    }
    else{
        // shouldn't ever happen
        return "-1";
    }
}

bool TableManager::tryResolveThis( TypeDecl*& result ){
    // Save current scope, then navigate to the class scope level
    Table* currScope = currTable;
    // This is safe because 'this' is only allowed at the method scope level
    while(currTable->getParent()->getParent() != 0){
        currTable = currTable->getParent();
    }
    // Get class name from table name
    // Tablename is of the form "Global::ClassName"
    string temp = currTable->getName();
    temp = temp.substr(8);
    if(tryLookup(temp, result)){
        currTable = currScope;
        return true;
    }
    currTable = currScope;
    return false;
}

bool TableManager::searchLocalTable( string tableName, string varid, TypeDecl*& result ){
    // Find table
    Table* currScope = currTable;
    string qualifiedTableName = "Global::" + tableName;
    while( currTable->getParent() != 0 ){
        currTable = currTable->getParent();
    }
    Table* localTable = 0;
    vector<Table*> c = currTable->getChildren();
    for(unsigned int i = 0; i < c.size(); i++){
        if(c[i]->getName() == qualifiedTableName ){
            localTable = c[i];
            break;
        }
    }
    // search for varid
    if( !localTable || !localTable->localSearch(varid, result) ){
        currTable = currScope;
        return false;
    }
    currTable = currScope;
    return true;
}

bool TableManager::searchLocalTable( string tableName, string methid, vector<TypeDecl*> args, MethDecl*& result ){
    // Find table
    Table* currScope = currTable;
    string qualifiedTableName = "Global::" + tableName;
    while( currTable->getParent() != 0 ){
        currTable = currTable->getParent();
    }
    Table* localTable = 0;
    vector<Table*> c = currTable->getChildren();
    for(unsigned int i = 0; i < c.size(); i++){
        if(c[i]->getName() == qualifiedTableName ){
            localTable = c[i];
            break;
        }
    }
    // search for methid
    if( !localTable || !localTable->localSearch(methid, args, result) ){
        currTable = currScope;
        return false;
    }
    currTable = currScope;
    return true;
}

TypeDecl* TableManager::getIntType(){
    TypeDecl* toRet = 0;
    globalTypeTable->tryLookup("int", toRet);
    return toRet;
}

bool TableManager::navigateTo( string nameOfChild ){
    string qualifiedName = getCurrentScope() + "::" + nameOfChild;
    vector<Table*> c = currTable->getChildren();
    for( unsigned int i = 0; i < c.size(); i++ ){
        if( c[i]->getName() == qualifiedName ){
            currTable = c[i];
            return true;
        }
    }
    return false;
}

void TableManager::registerMain(){
    mainCount++;
}

int TableManager::getMainCount(){
    return mainCount;
}

bool TableManager::verifyConstructor( string name, vector<TypeDecl*> args ){
    TypeDecl* temp = 0;
    if(!globalTypeTable->tryLookup( name, temp ) ){
        // Not a constructor
        return false;
    }
    // navigate to global table and then find child with name
    Table* currScope = currTable;
    while( currTable->getParent() != 0 ){
        currTable = currTable->getParent();
    }
    if( !navigateTo( name ) ){
        return false;
    }
    MethDecl* mTemp = 0;
    if( !currTable->tryLookup( name, args, mTemp ) ){
        currTable = currScope;
        return false;
    }
    currTable = currScope; 
    return true;
}

string TableManager::appendInt( string str, int i ){
    stringstream s;
    s << str << i;
    return s.str();
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
        currTable->print(0);

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
Table::Table( Table* parent, string name ){
    this->name = name;
    this->parent = parent;
}

Table::~Table(){
    for(unsigned int i = 0; i < children.size(); i++){
        delete children[i];
    }
}

bool Table::tryLookup( string name, vector<TypeDecl*> args, MethDecl*& result ){
    // Check local table
    map< string, vector<MethDecl> >::iterator it;
    if( (it = methTable.find(name)) != methTable.end() ){
        for(unsigned int i = 0; i < it->second.size(); i++){
            if( it->second[i].getArgTypes() == args ){
                result = &(it->second[i]);
                return true;
            }
        }
    }
    // Check parents tables
    if( parent ){ return parent->tryLookup(name, args, result); }
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
    MethDecl toInsert( methName, argTypes, returnType, forwardDecl );
    map< string, vector<MethDecl> >::iterator it;
    if( ( it = methTable.find(methName) ) != methTable.end() ){
        for( unsigned int i = 0; i < it->second.size(); i++ ){
            if( it->second[i].getArgTypes() == argTypes ){
                return false;
            }
        }
        it->second.push_back( toInsert );
    }
    else{
        vector<MethDecl> temp;
        temp.push_back( toInsert );
        methTable[methName] = temp;
    }
    return true;
}

Table* Table::getParent(){
    return parent;
}

string Table::getName(){
    return name;
}

void Table::registerChild( Table* c ){
    children.push_back(c);
}

vector<Table*> Table::getChildren(){
    return children;
}

bool Table::localSearch( string varid, TypeDecl*& result ){
    map<string, TypeInst>::iterator it;
    if( (it = typeTable.find(varid)) != typeTable.end() ){
        TypeInst* temp = 0;
        temp = &(it->second);
        result = temp->getType();
        return true;
    }
    return false;
}

bool Table::localSearch( string methid, vector<TypeDecl*> args, MethDecl*& result ){
    map< string, vector<MethDecl> >::iterator it;
    if( (it = methTable.find(methid)) != methTable.end() ){
        for(unsigned int i = 0; i < it->second.size(); i++){
            if( it->second[i].getArgTypes() == args ){
                result = &(it->second[i]);
                return true;
            }
        }
    }
    return false;
}

void Table::print( int indent ){
    for(int i = 0; i < indent; i++){ cout << "  "; }
    cout << getName() << endl;
    for( map<string, TypeInst>::iterator it = typeTable.begin();
    it != typeTable.end(); it++){
        for(int i = 0; i < indent; i++){ cout << "  "; }
        it->second.print();
    }
    for( map< string, vector<MethDecl> >::iterator it = methTable.begin();
    it != methTable.end(); it++){
        for(unsigned int j = 0; j < it->second.size(); j++){
        for(int i = 0; i < indent; i++){ cout << "  "; }
            it->second[j].print();
        }
    }
    cout << endl;

    // Print children
    for(unsigned int i = 0; i < children.size(); i++){
        children[i]->print(indent + 1);
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
    cout << name << ": ";
    type->print();
    cout << endl;
    
}

TypeDecl* TypeInst::getType(){
    return type;
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

TypeDecl* MethDecl::getRetType(){
    return retType;
}

string MethDecl::getName(){
    return name;
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