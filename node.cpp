#include"node.h"

// These are organized as closely to the grammar sheet as I could get.
// All of the checking pointer vaildity might seem a little over the
// top but I really don't trust myself with pointers.


// Node declarations
Node::Node( Node* l, Node* r ){
    left = l;
    right = r;
}

Node::~Node(){
    if(left) { delete left; }
    if(right){ delete right; }
}

void Node::setLeft( Node* l ){
    if(l){ left = l; }
}

void Node::setRight( Node* r ){
    if(r){ right = r; }
}

void Node::print(){
    // Base node, no special print
    if(left) { left->print(); }
    if(right){ right->print(); }
}

bool Node::registerType( TableManager* tm ){
    cout << "Error: Malformed syntax tree" << endl;
    return false;
}

void Node::populateTables( TableManager* tm ){
    cout << "Error: Malformed syntax tree" << endl;
}

bool Node::checkConstructorNames(){
    cout << "Error malformed syntax tree" << endl;
    return false;
}

bool Node::passConstructorNames( string name ){
    cout << "Error: malformed syntax tree" << endl;
    return false;
}

bool Node::getID( string& result ){
    cout << "Error: malformed syntax tree" << endl;
    return false;
}

bool Node::getTypeID( string& type, string& ID ){
    cout << "Error: malformed syntax tree" << endl;
    return false;
}

bool Node::gatherParams( vector<string>& types, vector<string>& ids ){
    cout << "Error: malformed syntax tree" << endl;
    return false;
}

int Node::gatherBrackets(){
    cout << "Error: malformed syntax tree" << endl;
    return -1;
}

bool Node::typeCheck( TableManager* tm ){
    cout << "Error: malformed syntax tree" << endl;
    return false;
}

bool Node::tryGetType( TableManager* tm, TypeDecl*& result ){
    cout << "Error: Malformed syntax tree" << endl;
    return false;
}

bool Node::isBlock(){
    return false;
}

/////////////////////////////////////////

IdNode::IdNode( string v ): Node( 0, 0 ){
    val = v;
}

bool IdNode::getID( string& result ){
    result = val;
    return true;
}

TypeIdNode::TypeIdNode( Node* type, Node* id ): Node( type, id ){}

bool TypeIdNode::getTypeID( string& type, string& ID ){
    string typeTemp;
    string nameTemp;
    if(left && left->getID(typeTemp) &&
    right && right->getID(nameTemp)){
        type = typeTemp;
        ID = nameTemp;
        return true;
    }
    else{
        cout << "Error: malformed tree" << endl;
        return false;
    }

}

void TypeIdNode::print(){
    cout << "<typeid> -> <type> id" << endl;
    if(left){ left->print(); }
}

/////////////////////////////////////////
// Classdec node
ClassDecNode::ClassDecNode( Node* classbody, Node* id ): Node( classbody, id )
{}

void ClassDecNode::print(){
    cout << "<classdec> -> class ID <classbody>" << endl;
    if(left){ left->print(); }
}

bool ClassDecNode::registerType( TableManager* tm ){
    string name;
    if(right->getID(name)){
        tm->forwardEntryGlobalTypeTable(name);
    }

    return true;
}

bool ClassDecNode::checkConstructorNames(){
    string name;
    if( right->getID(name) ){
        if( !left ){
            cout << "Error: malformed syntax tree" << endl;
            return false;
        }
        if( !left->passConstructorNames(name) ){
            return false;
        }
    }
    else{
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    return true;
}

void ClassDecNode::populateTables( TableManager* tm ){
    // Name table
    string name;
    if(!right->getID(name)){ cout << "Error: Malformed syntax tree" << endl; }
    // Check constructor names
    checkConstructorNames();
    // Populate table
    tm->enterScope(name);
    if(left){ left->populateTables(tm); }
    else{ cout << "Error: Malformed syntax tree" << endl; }
    tm->exitScope();
}

bool ClassDecNode::typeCheck( TableManager* tm ){
    // Move to current scope
    string name;
    if(!right->getID(name)){
        cout << "Error: malformed syntax tree" << endl;
    }
    if( !tm->navigateTo(name) ){
        cout << "Error: malformed type table" << endl;
        return false;
    }
    // Begin type check cascade
    if(left && left->typeCheck( tm )){
        tm->exitScope();
        return true;
    }
    // Leave scope
    tm->exitScope();
    return false;
}

/////////////////////////////////////////
// Classbody node
ClassBodyNode::ClassBodyNode( Node* vardecls, Node* condecls, Node* methdecls )
: Node( vardecls, condecls ){
    this->methdecls = methdecls;
}

ClassBodyNode::~ClassBodyNode(){
    delete methdecls;
}

bool ClassBodyNode::passConstructorNames( string name ){
    if( right && right->passConstructorNames( name ) ){
        return true;
    }
    return false;
}

void ClassBodyNode::populateTables( TableManager* tm ){
    if(left){ left->populateTables( tm ); }
    if(right){ right->populateTables( tm ); }
    if(methdecls){ methdecls->populateTables( tm ); }
}

bool ClassBodyNode::typeCheck( TableManager* tm ){
    // Variable declarations don't need type checking
    // Returning has to be delayed to allow everything to get type checked
    bool toRet = true;
    if(right){
        if(!right->typeCheck(tm)){
            toRet = false;
        }
    }
    if(methdecls){
        if(!methdecls->typeCheck(tm)){
            toRet =  false;
        }
    }
    return toRet;
}

void ClassBodyNode::print(){
    cout << "<classbody> -> { ";
    if(left){ cout << "<vardecls>"; }
    if(right){ cout << " <condecls>"; }
    if(methdecls){ cout << " <methdecls>"; }
    cout << " }" << endl;

    if(left){ left->print(); }
    if(right){ right->print(); }
    if(methdecls){ methdecls->print(); }
}
/////////////////////////////////////////
// grouping nodes. Vardecls, condecls, methdecls
VarDeclsNode::VarDeclsNode( Node* vardecl, Node* next ):
Node( vardecl, next ){}

void VarDeclsNode::populateTables( TableManager* tm ){
    // List is backwards
    if(right){ right->populateTables(tm); }
    if(left){ left->populateTables(tm); }
}

void VarDeclsNode::print(){
    cout << "<vardecls> -> ";
    if(right){
        cout << "<vardecls> <vardecl>" << endl;
    }
    else{
        cout << "<vardecl>" << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

ConDeclsNode::ConDeclsNode( Node* condecl, Node* next ):
Node( condecl, next ){}

bool ConDeclsNode::passConstructorNames( string name ){
    if( right && !right->passConstructorNames( name ) ){
        return false;
    }
    if(left && !left->passConstructorNames( name ) ){
        return false;
    }
    return true;
}

void ConDeclsNode::populateTables( TableManager* tm ){
    if(right){ right->populateTables(tm); }
    if(left){ left->populateTables(tm); }
}

bool ConDeclsNode::typeCheck( TableManager* tm ){
    // These if statements are different to imply that right doesn't have
    // to exist but left must exist.
    // Returning must be delayed to allow all decls to be processed
    bool toRet = true;
    if(right){
        if(!right->typeCheck(tm)){
            toRet = false;
        }
    }
    if(!left || !left->typeCheck(tm)){
        toRet = false;
    }
    return toRet;
}

void ConDeclsNode::print(){
    cout << "<condecls> -> ";
    if(right){
        cout << "<condecls> <condecl>" << endl;
    }
    else{
        cout << "<condecl>" << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

MethDeclsNode::MethDeclsNode( Node* methdecl, Node* next ):
Node( methdecl, next ){}

void MethDeclsNode::populateTables( TableManager* tm ){
    if(right){ right->populateTables(tm); }
    if(left){ left->populateTables(tm); } 
}

bool MethDeclsNode::typeCheck( TableManager* tm ){
    // These if statements are different to imply that right doesn't have
    // to exist but left must exist.
    // Returning must be delayed to allow all decls to be processed
    bool toRet = true;
    if(right){
        if(!right->typeCheck(tm)){
            toRet = false;
        }
    }
    if(!left || !left->typeCheck(tm)){
        toRet = false;
    }
    return toRet;
}

void MethDeclsNode::print(){
    cout << "<methdecls> -> ";
    if(right){
        cout << "<methdecls> <methdecl>" << endl;
    }
    else{
        cout << "<methdecl>" << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Var Decl Nodes
VarTypeIdNode::VarTypeIdNode( Node* tid ): Node( tid, 0 ){}

void VarTypeIdNode::populateTables( TableManager* tm ){
    if(left){ 
        string type;
        string id;
        if(left->getTypeID(type, id)){
            // Since type always goes to int, this only allows 
            // array types to be registered
            tm->forwardEntryGlobalTypeTable(type);
            tm->addTypeInst(type, id);
        }
        else{
            cout << "Error: malformed syntax tree" << endl;
        }
    }
}

void VarTypeIdNode::print(){
    cout << "<vardecl> -> <typeid>;" << endl;
    if(left){ left->print(); }
}

////

IdIdNode::IdIdNode( Node* l, Node* r ): Node( l, r ){}

void IdIdNode::populateTables( TableManager* tm ){
    string type;
    string name;
    if( left && left->getID(type) &&
    right && right->getID(name) ){
        tm->addTypeInst(type, name);
    }
    else{
        cout << "Error: Malformed syntax tree" << endl;
    }
}

void IdIdNode::print(){
    cout << "<vardecl> -> id id;" << endl;
}

////

IdMultiIdNode::IdMultiIdNode( Node* type, Node* b, Node* id ):
Node( type, id ){
    brackets = b;
}

IdMultiIdNode::~IdMultiIdNode(){
    delete brackets;
}

void IdMultiIdNode::populateTables( TableManager* tm ){
    int numBrackets = 0;
    if(brackets){ numBrackets = brackets->gatherBrackets(); }
    
    string type;
    string name;
    if( left && left->getID(type) &&
    right && right->getID(name) ){
        // Construct the array type and put it in the GTT, then use it
        // If it already exists, no problem
        for(int i = 0; i < numBrackets; i++){
            type = type + "[]";
        }
        tm->forwardEntryGlobalTypeTable(type);
        tm->addTypeInst(type, name);
    }
    else{
        cout << "Error: Malformed syntax tree" << endl;
    }
}

void IdMultiIdNode::print(){
    cout << "<vardecl> -> id <multibrackets> id" << endl;
    if(brackets){ brackets ->print(); }
}

/////////////////////////////////////////
// Type Nodes
TypeNode::TypeNode( Node* t, Node* brackets ): Node( t, brackets ){}

bool TypeNode::getID( string& result ){
    string temp;
    // type always goes to int
    if(left){ temp = "int"; }
    else{ return false; }
    if(right){ 
        int b = right->gatherBrackets();
        for(int i = 0; i < b; i++){
            temp += "[]";
        }
    }
    result = temp;
    return true;
}

void TypeNode::print(){
    cout << "<type> -> ";
    if(right){
        cout << "<type> <multibrackets>" << endl;
    }
    else{
        cout << "<simpletype>" << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

SimpleTypeNode::SimpleTypeNode(): Node( 0, 0 ){}

void SimpleTypeNode::print(){
    cout << "<simpletype> -> int" << endl; 
}

/////////////////////////////////////////
// Constructor Declaration Node
ConstructorDecNode::ConstructorDecNode( Node* id, Node* plist, Node* block ):
Node( id, plist ){
    this->block = block;
}

bool ConstructorDecNode::passConstructorNames( string name ){
    string conName;
    if( !left->getID(conName) ){
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    if( name != conName ){
        cout << "Error: Constructor name '" << conName 
        << "' does not match class name '" << name << "'" << endl;
        return false;
    }
    return true;
}

void ConstructorDecNode::populateTables( TableManager* tm ){
   if(left){ 
        string id;
        if(left->getID( id )){
            // Process params
            vector<string> types;
            vector<string> ids;
            if(right){ right->gatherParams( types, ids ); }
            else{ cout << "Error: malformed syntax tree" << endl; }
            if( tm->verifyTypes( types ) ){
                tm->addTypes( types );
            }
            if(!tm->addMethDecl( id, types, "void" )){ 
                cout << "Error: Cannot add '" << id << "' to symbol table."
                     << endl;
            }
            tm->enterScope(id);
            if(block){ block->populateTables( tm ); }
            else{ cout << "Error: Malformed syntax tree" << endl; }
            tm->exitScope();
        } 
   }  
}

bool ConstructorDecNode::typeCheck( TableManager* tm ){
    // Enter scope
    string name;
    if(!left->getID(name)){
        cout << "Error: malformed syntax tree" << endl;
    }
    if( !tm->navigateTo(name) ){
        cout << "Error: malformed type table" << endl;
        return false;
    }
    // Continue type check
    if( !block || !block->typeCheck(tm)){
        tm->exitScope();
        return false;
    }
    // Exit scope
    tm->exitScope();
    return true;
}

ConstructorDecNode::~ConstructorDecNode(){
    delete block;
}

void ConstructorDecNode::print(){
    cout << "<condecl> -> id ( <plist> ) <block>" << endl;
    if(right){ right->print(); }
    if(block){ block->print(); }
}


/////////////////////////////////////////
// Method Declaration Nodes
MethodDecNode::MethodDecNode( Node* tid, Node* plist, Node* block ):
Node( tid, plist  ){
    this->block = block;
}

MethodDecNode::~MethodDecNode(){
    delete block;
}

void MethodDecNode::populateTables( TableManager* tm ){
   if(left){ 
        string type;
        string id;
        if(left->getTypeID( type, id )){
            // Since type always goes to int, this only allows 
            // array types to be registered
            tm->forwardEntryGlobalTypeTable( type );
            // Process params
            vector<string> types;
            vector<string> ids;
            if(right){ right->gatherParams( types, ids ); }
            else{ cout << "Error: malformed syntax tree" << endl; }
            if( tm->verifyTypes( types ) ){
                tm->addTypes( types );
            }
            if(!tm->addMethDecl( id, types, type )){ 
                cout << "Error: Cannot add '" << id << "' to symbol table."
                     << endl;
            }
            tm->enterScope(id);
            if(block){ block->populateTables( tm ); }
            else{ cout << "Error: Malformed syntax tree" << endl; }
            tm->exitScope();
        } 
   }     
}

bool MethodDecNode::typeCheck(  TableManager* tm ){
    // Enter scope
    string name;
    // uneeded but used to break up typeid
    string type;
    if(!left->getTypeID(type, name)){
        cout << "Error: malformed syntax tree" << endl;
    }
    if( !tm->navigateTo(name) ){
        cout << "Error: malformed type table" << endl;
        return false;
    }
    // Continue type check
    if( !block || !block->typeCheck(tm) ){
        tm->exitScope();
        return false;
    }
    tm->exitScope();
    return true;
}

void MethodDecNode::print(){
    cout << "<methdecl> -> <typeid> ( <plist> ) <block>" << endl;
    
    if(left){ left->print(); }
    if(right){ right->print(); }
    if(block){ block->print(); }

}

////

VoidMethodDecNode::VoidMethodDecNode( Node* id, Node* plist, Node* block ):
Node( id, plist  ){
    this->block = block;
}

VoidMethodDecNode::~VoidMethodDecNode(){
    delete block;
}

void VoidMethodDecNode::populateTables( TableManager* tm ){
    if(left){ 
        string id;
        if(!left->getID(id)){ cout << "Error: malformed syntax tree"; }
        // Process params
        vector<string> types;
        vector<string> ids;
        if(right){ right->gatherParams( types, ids ); }
        else{ cout << "Error: malformed syntax tree" << endl; }
        if( tm->verifyTypes( types ) ){
            tm->addTypes( types );
        }
        if(!tm->addMethDecl( id, types, "void" )){ 
            cout << "Error: Cannot add '" << id << "' to symbol table."
                    << endl;
        }
        tm->enterScope(id);
        if(block){ block->populateTables( tm ); }
        else{ cout << "Error: Malformed syntax tree" << endl; }
        tm->exitScope();
   }     
}

bool VoidMethodDecNode::typeCheck(  TableManager* tm ){
    // Enter scope
    string name;
    if(!left->getID(name)){
        cout << "Error: malformed syntax tree" << endl;
    }
    if( !tm->navigateTo(name) ){
        cout << "Error: malformed type table" << endl;
        return false;
    }
    // Continue type check
    if( !block || !block->typeCheck(tm) ){
        tm->exitScope();
        return false;
    }
    tm->exitScope();
    return true;
}


void VoidMethodDecNode::print(){
    cout << "<methdecl> -> void ID ( <plist> ) <block>" << endl;

    if(right){ right->print(); }
    if(block){ block->print(); }
}

////

IDMethodDecNode::IDMethodDecNode( Node* result, Node* id, Node* plist,
Node* block ): Node( id, plist ){
    this->block = block;
    this->result = result;
}

IDMethodDecNode::~IDMethodDecNode(){
    delete block;
    delete result;
}

void IDMethodDecNode::populateTables( TableManager* tm ){
   if(left){ 
        string id;
        string retType;
        if(!left->getID(id)){ cout << "Error: malformed syntax tree" << endl; }
        if(!result->getID(retType)){
            cout << "Error: malformed syntax tree" << endl;
        }
        // Process params
        vector<string> types;
        vector<string> ids;
        if(right){ right->gatherParams( types, ids ); }
        else{ cout << "Error: malformed syntax tree" << endl; }
        if( tm->verifyTypes( types ) ){
            tm->addTypes( types );
        }
        if(!tm->addMethDecl( id, types, retType )){ 
            cout << "Error: Cannot add '" << id << "' to symbol table."
                    << endl;
        }
        tm->enterScope(id);
        if(block){ block->populateTables( tm ); }
        else{ cout << "Error: Malformed syntax tree" << endl; }
        tm->exitScope();
   }
}

bool IDMethodDecNode::typeCheck(  TableManager* tm ){
    // Enter scope
    string name;
    if(!left->getID(name)){
        cout << "Error: malformed syntax tree" << endl;
    }
    if( !tm->navigateTo(name) ){
        cout << "Error: malformed type table" << endl;
        return false;
    }
    // Continue type check
    if( !block || !block->typeCheck(tm) ){
        tm->exitScope();
        return false;
    }
    tm->exitScope();
    return true;
}


void IDMethodDecNode::print(){
    cout << "<methdecl> -> ID ID ( <plist> ) <block>" << endl;

    if(right){ right->print(); }
    if(block){ block->print(); }
}

/////////////////////////////////////////
// Result Type Nodes
// ResultTypeNode::ResultTypeNode( Node* type ): Node( type, 0 ){} 

// void ResultTypeNode::print(){
//     cout << "<result> -> <type>" << endl;
//     if(left){ left->print(); }
// }

// ////

// ResultVoidNode::ResultVoidNode(): Node( 0, 0 ){}

// void ResultVoidNode::print(){
//     cout << "<result> -> void" << endl;
// }

/////////////////////////////////////////
// Parameter List Node
ParameterListNode::ParameterListNode( Node* param ): Node( param, 0 ){}

bool ParameterListNode::gatherParams( vector<string>& types,
vector<string>& ids ){
    if(left){ return left->gatherParams( types, ids ); }
    return true;
}

void ParameterListNode::print(){
    if(left){
        cout << "<plist> -> <param>" << endl;
        left->print();
    }
    else{
        cout << "<plist> -> epsilon" << endl;
    }
}

/////////////////////////////////////////
// Parameter Nodes

ParameterNode::ParameterNode( Node* type, Node* id, Node* next ):
Node( type, id ){
    this->next = next;
}

ParameterNode::~ParameterNode(){
    delete next;
}

bool ParameterNode::gatherParams( vector<string>& types, vector<string>& ids ){
    string type;
    string id;
    if(!left || !left->getID(type)){ return false; }
    if(!right || !right->getID(id)){ return false; }
    types.push_back(type);
    ids.push_back(id);

    if(next){ return next->gatherParams(types, ids); }
    return true;
}

void ParameterNode::print(){
    cout << "<param> -> <type> id";
    if(next){
        cout << ", <param>";
    }
    cout << endl;
    if(left){ left->print(); }
    if(next){ next->print(); }
}

////

ParameterIDNode::ParameterIDNode( Node* type, Node* mb, Node* id, Node* next ):
Node( type, id ){
    this->mb = mb;
    this->next = next;
}

ParameterIDNode::~ParameterIDNode(){
    delete mb;
    delete next;
}

bool ParameterIDNode::gatherParams( vector<string>& types,
vector<string>& ids ){
    string type;
    string id;
    if(!left || !left->getID(type)){ return false; }
    if(mb){
        // Add brackets to type, plist node will check for valid type
        int b = mb->gatherBrackets();
        for(int i = 0; i < b; i++){
            type += "[]";
        }
    }
    if(!right || !right->getID(id)){ return false; }
    types.push_back(type);
    ids.push_back(id);

    if(next){ return next->gatherParams(types, ids); }
    return true;
}

void ParameterIDNode::print(){
    cout << "<param> -> id";
    if(mb){ cout << " <multibrackets>"; }
    cout << " id";
    if(next){ cout << ", <param>"; }
    cout << endl;
    if(mb){ mb-> print(); }
    if(next){ next->print(); }
}

/////////////////////////////////////////
// Statement Nodes

EmptyStatementNode::EmptyStatementNode(): Node( 0, 0 ){}

bool EmptyStatementNode::typeCheck( TableManager* tm ){
    return true;
}

void EmptyStatementNode::print(){
    cout << "<stmt> -> ;" << endl;
}

////

EQStatementNode::EQStatementNode( Node* name, Node* expr ):
Node( name, expr ){}

bool EQStatementNode::typeCheck( TableManager* tm ){
    TypeDecl* nameType = 0;
    TypeDecl* exprType = 0;
    if( !left || !right ){ 
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    if(left->tryGetType(tm, nameType) && right->tryGetType(tm, exprType)){
        // Do comparison
        if(nameType == exprType){
            return true;
        }
        else{
            string name;
            if(left->getID(name)){
                cout << "Error: Invalid assignment to symbol '" 
                << name << "'." << endl;
            }
            else{
                cout << "Error: malformed syntax tree" << endl;
            }
            return false;
        }
    }
    else{
        string name;
        if(left->getID(name)){
            cout << "Symbol '" << name 
            << "' is used before it is defined." << endl;
        }
        else{
            cout << "Error: malformed syntax tree" << endl;
        }
        return false;
    }
}

void EQStatementNode::print(){
    cout << "<stmt> -> <name> = <expr>;" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

FuncStatementNode::FuncStatementNode( Node* name, Node* arglist ):
Node( name, arglist ){}

bool FuncStatementNode::typeCheck( TableManager* tm ){
    string funcName;
    left->getID(funcName);
    MethDecl* mptr = 0;
    // check name
    if( !tm->tryLookup(funcName, mptr) ){
        cout << "Error: symbol '" << funcName << "' undefined." << endl;
        return false;
    }
    // gather and check args
    vector<TypeDecl*> argTypes;
    right->gatherArgs(argTypes);
    vector<TypeDecl*> paramTypes = mptr->getArgTypes();
    if( argTypes.size() != paramTypes.size() ){
        cout << "Error: Incorrect argument count in function call "
        << funcName << endl;
        return false;
    }
    for(unsigned int i = 0; i < argTypes.size(); i++){
        if(argTypes[i] != paramTypes[i]){
            cout << "Error: argument " << i << " in function " << funcName
            << endl << "Expected type " << paramTypes[i]->getName()
            << "Actual type " << argTypes[i]->getName() << endl;
            return false;
        }
    }
    return true;
}

void FuncStatementNode::print(){
    cout << "<stmt> -> <name> ( <arglist> );" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

PrintStatementNode::PrintStatementNode( Node* arglist ): Node( arglist, 0 ){}

// Print statement doesn't really need to be checked
bool PrintStatementNode::typeCheck( TableManager* tm ){
    return true;
}

void PrintStatementNode::print(){
    cout << "<stmt> -> print( <arglist> );" << endl;
    if(left){ left->print(); }
}

////

WhileStatementNode::WhileStatementNode( Node* expr, Node* stmt ):
Node( expr, stmt ){}

bool WhileStatementNode::typeCheck( TableManager* tm ){
    if( !left || !right ){
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    TypeDecl* temp = 0;
    if( !left->tryGetType(tm, temp) || temp != tm->getIntType() ){
        cout << "Error: Invalid condition in while loop" << endl;
        return false;
    }
    // If there's an anonymous block, create a new table and populate it, then
    // type check it. Very hacked
    if( right->isBlock() ){
        // Create anonymous table, populate and typecheck.
        // Population is done here because renavigating to the anonymous 
        // table would be difficult
        tm->enterAnonymousScope();
        string aScope = tm->getCurrentScope();
        right->populateTables( tm );
        if( right->typeCheck( tm ) ){
            tm->exitScope();
            return true;
        }
        tm->exitScope();
        return false;
    }
    else {
        // No block no new table
        if( right->typeCheck(tm) ){
            return true;
        }
        return false;
    }
    
}

void WhileStatementNode::print(){
    cout << "<stmt> -> while ( <expr> ) <stmt>;" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

ReturnStatementNode::ReturnStatementNode( Node* optexpr ): Node( optexpr, 0 ){}

bool ReturnStatementNode::typeCheck( TableManager* tm ){
    if( !left ){
        return true;
    }
    else{
        return left->typeCheck(tm);
    }
}

void ReturnStatementNode::print(){
    cout << "<stmt> -> return <optexpr>;" << endl;
    if(left){ left->print(); }
}

/////////////////////////////////////////
// Block Nodes
BlockNode::BlockNode( Node* localvars, Node* stmts ):
Node( localvars, stmts ){}

void BlockNode::populateTables( TableManager* tm ){
    if(left){ left->populateTables( tm ); }
    // Statements aren't needed to populate table
}

bool BlockNode::typeCheck( TableManager* tm ){
    // Vardecls don't need to be typechecked
    if(right){
        if(!right->typeCheck(tm)){
            return false;
        }
    }
    return true;
}

bool BlockNode::isBlock(){ return true; }

void BlockNode::print(){
    cout << "<block> -> { ";
    if(left){ cout << "<localvars>"; }
    if(right){ cout << " <stmts>"; }    cout <<  " }" << endl;

    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

LocalVarsNode::LocalVarsNode( Node* localvar, Node* next ):
Node( localvar, next ){}

void LocalVarsNode::populateTables( TableManager* tm ){
    // Might need to reverse
    if(left){ left->populateTables( tm ); }
    if(right){ right->populateTables( tm ); }
}

void LocalVarsNode::print(){
    cout << "<localvars> -> <localvar>";
    if(right){ cout << " <localvars>"; }
    cout << endl;
    
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

StatementsNode::StatementsNode( Node* stmt, Node* next ): Node( stmt, next ){}

bool StatementsNode::typeCheck( TableManager* tm ){
    bool toRet = true;
    if(right){
        if(!right->typeCheck(tm)){
            toRet = false;
        }
    }
    if(!left || !left->typeCheck(tm)){
        toRet = false;
    }
    return toRet;
}

void StatementsNode::print(){
    cout << "<stmts> -> <stmt>";
    if(right){ cout << " <stmts>"; }
    cout << endl;
    
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Local Var Nodes
LocalVarDecNode::LocalVarDecNode( Node* type, Node* id ): Node( type, id ){}

void LocalVarDecNode::populateTables( TableManager* tm ){
    string type;
    string id;

    if(left){ left->getID(type); }
    else{ cout << "Error: Malformed Syntax Tree" << endl; }
    // this node only handles int types
    tm->forwardEntryGlobalTypeTable(type);
    
    if(right){ right->getID(id); }
    else{ cout << "Error: Malformed Syntax Tree" << endl; }
    
    tm->addTypeInst(type, id);
}

void LocalVarDecNode::print(){
    cout << "<localvar> -> <type> id;" << endl;

    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

LocalVarDecIDNode::LocalVarDecIDNode( Node* type, Node* id, Node* mb ):
Node( type, id ){
    this->mb = mb;
}

void LocalVarDecIDNode::populateTables( TableManager* tm ){
    string type;
    string id;

    if(left){ left->getID(type); }
    else{ cout << "Error: malformed syntax tree" << endl; }
    if(right){ right->getID(id); }
    else{ cout << "Error: malformed syntax tree" << endl; }

    TypeDecl* temp = 0;
    if( tm->tryLookup(type, temp) ){
        // Create array type and add to table
        if(mb){
            int b = mb->gatherBrackets();
            for(int i = 0; i < b; i++){
                type += "[]";
            }
            tm->forwardEntryGlobalTypeTable(type);
        }
        tm->addTypeInst(type, id);
    }
    else{
        cout << "Error: Invalid type '" << id << "'" << endl;
    }
}

LocalVarDecIDNode::~LocalVarDecIDNode(){
    delete mb;
}

void LocalVarDecIDNode::print(){
    cout << "<localvar> -> id";
    if(mb){ cout << " <multibrackets>"; }
    cout << " id" << endl;

    if(mb){ mb->print(); }
}

/////////////////////////////////////////
// name Nodes
ThisNode::ThisNode(): Node ( 0, 0 ){}

bool ThisNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( tm->tryResolveThis( result ) ){
        return true;
    }
    return false;

}

void ThisNode::print(){
    cout << "<name> -> this" << endl;
}

////

NameIdNode::NameIdNode( Node* id ): Node ( id, 0 ){}

bool NameIdNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    TypeInst* temp = 0;
    string id;
    if(left && left->getID(id)){
        if(tm->tryLookup(id, temp)){
            result = temp->getType();
            return true;
        }
        else{
            cout << "Error: symbol '" << id << "' undefined." << endl;
            return false;
        }
    }
    else{
        cout << "Error: Malformed syntax tree" << endl;
        return false;
    }
}

void NameIdNode::print(){
    cout << "<name> -> id" << endl;
}

////

NameDotIdNode::NameDotIdNode( Node* name, Node* Id ): Node( name, Id ){}

bool NameDotIdNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    // Use table manager Name resolution. Resolve on type from name node
    TypeDecl* name = 0;
    if(!left){
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    if(!left->tryGetType(tm, name) ){
        cout << "Error: cannot resolve name" << endl;
        return false;
    }
    string typeName = name->getName();
    
    if(!right){
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    string ID;
    if(!right->getID(ID)){
        cout << "Error: malformed sytnax tree" << endl;
        return false;
    }
    if( tm->searchLocalTable(typeName, ID, result) ){
        return true;
    }
    return false;
}

void NameDotIdNode::print(){
    cout << "<name> -> <name> . id" << endl;
    if(left){ left->print(); }
}

////

// This assigns the name subtree to left and the expr subtree to right
NameExprNode::NameExprNode( Node* name, Node* expr ): Node( name, expr ){}

bool NameExprNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    // check expr type then set result as name type
    TypeDecl* nameType = 0;
    if(!left){
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    if( !left->tryGetType(tm, nameType) ){
        cout << "Undefined symbol" << endl;
        return false;
    }
    // Check table for nonarray type
    // Remove one level of braces (array access type): 
    // Int[0] -> Int; Int[0][] -> Int[]
    string typeName = nameType->getName();
    typeName = typeName.substr(0, typeName.size() - 2);
    if( !tm->tryLookup(typeName, result) ){
        cout << "Error: type '" << typeName << "' does not exist" << endl;
        return false;
    }

    // make sure expr type is int
    if(!right){
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
    TypeDecl* temp = 0;
    if( !right->tryGetType(tm, temp) || temp != tm->getIntType() ){
        cout << "Error: index for '" << typeName << "' is not an int" << endl;
        return false;
    }
    return true;
}

void NameExprNode::print(){
    cout << "<name> -> <name> [ <expr> ]" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Arglist Nodes
ArgListNode::ArgListNode( Node* args ): Node( args, 0 ){}

void ArgListNode::print(){
    if(left){
        cout << "<arglist> -> <arg>" << endl;
        left->print();
    }
    else{
        cout << "<arglist> -> epsilon" << endl;
    }
}

ArgNode::ArgNode( Node* expr, Node* next ): Node( expr, next ){}

void ArgNode::print(){
    if(right){
        cout << "<arg> -> <expr> , <arg>" << endl;
    }
    else{
        cout << "<arg> -> <expr>" << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Conditional Expression Node
CondStatementNode::CondStatementNode( Node* expr, Node* stmt, Node* estmt ):
Node( expr, stmt ){
    this->estmt = estmt;
}

CondStatementNode::~CondStatementNode(){
    delete estmt;
}

void CondStatementNode::print(){
    cout << "<stmt> -> <cond>" << endl
         << "<cond> -> if ( <expr> ) <stmt>";
    if(estmt){ cout << " else <stmt>"; }
    cout << endl;

    if(left){ left->print(); }
    if(right){ right->print(); }
    if(estmt){ estmt->print(); }
}

/////////////////////////////////////////
// Optional Expression Nodes

OptExprNode::OptExprNode( Node* expr ): Node( expr, 0 ){}

bool OptExprNode::typeCheck( TableManager* tm ){
    if( !left ){
        cout << "Error: malformed syntax tree" << endl;
    }
    else{
        return left->typeCheck( tm ); 
    }
}

void OptExprNode::print(){
    if(left){
        cout << "<optexpr> -> <expr>" << endl;
        left->print();
    }
    else{
        cout << "<optexpr> -> epsilon" << endl;
    }
}

/////////////////////////////////////////
// Expression Nodes
ExprNameNode::ExprNameNode( Node* name ): Node( name, 0 ){}

bool ExprNameNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( left && left->tryGetType(tm, result) ){
        return true;
    }
    return false;
}

void ExprNameNode::print(){
    cout << "<expr> -> <name>" << endl;
    if(left){ left->print(); }
}

////

NumNode::NumNode(int v){
    val = v;
}

bool NumNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    result = tm->getIntType();
    return true;
}

void NumNode::print(){
    cout << "<expr> -> number" << endl;
}

////

NullNode::NullNode(): Node( 0, 0 ){}


bool NullNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    // No clue how to deal with this
    result = tm->getIntType();
    return true;
}

void NullNode::print(){
    cout << "<expr> -> null" << endl;
}

////

ReadNode::ReadNode(): Node( 0, 0 ){}


bool ReadNode::tryGetType( TableManager* tm, TypeDecl*& result ){ 
    result = tm->getIntType();
    return true;
}

void ReadNode::print(){
    cout << "<expr> -> read ( )" << endl;
}

////

NewExprNode::NewExprNode( Node* newexpr ): Node( newexpr, 0 ){}

bool NewExprNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( left && left->tryGetType(tm, result) ){
        return true;
    }
    return false;
}

void NewExprNode::print(){
    cout << "<expr> -> <newexpr>" << endl;
    if(left){ left->print(); }
}

////

MethodCallNode::MethodCallNode( Node* name, Node* arglist ):
Node( name, arglist ){}


bool MethodCallNode::tryGetType( TableManager* tm, TypeDecl*& result ){ 
    MethDecl* temp = 0;
    string name;
    left->getID(name);
    if( tm->tryLookup(name, temp) ){
        result = temp->getRetType();
        return true;
    }
    else {
        cout << "Error: symbol '" << name << "' undefined." << endl;
        return false;
    }
}

void MethodCallNode::print(){
    cout << "<expr> -> <name> ( <arglist> )" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

// Sum nodes
SumNode::SumNode( Node* l, Node* r ): Node( l, r ){}

bool SumNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void SumNode::print(){
    cout << "<expr> -> <expr> + <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

MinusNode::MinusNode( Node* l, Node* r ): Node( l, r ){}

bool MinusNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
}

void MinusNode::print(){
    cout << "<expr> -> <expr> - <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

ORNode::ORNode( Node* l, Node* r ): Node( l, r ){}

bool ORNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
}

void ORNode::print(){
    cout << "<expr>-> <expr> || <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Relation Nodes
EqNode::EqNode( Node* l, Node* r ): Node( l, r ){}

bool EqNode::tryGetType( TableManager* tm, TypeDecl*& result ){ 
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void EqNode::print(){
    cout << "<expr> -> <expr> == <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

NeqNode::NeqNode( Node* l, Node* r ): Node( l, r ){}

bool NeqNode::tryGetType( TableManager* tm, TypeDecl*& result ){ 
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void NeqNode::print(){
    cout << "<expr> -> <expr> != <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

LeqNode::LeqNode( Node* l, Node* r ): Node( l, r ){}

bool LeqNode::tryGetType( TableManager* tm, TypeDecl*& result ){ 
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void LeqNode::print(){
    cout << "<expr> -> <expr> <= <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

GeqNode::GeqNode( Node* l, Node* r ): Node( l, r ){}

bool GeqNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
}

void GeqNode::print(){
    cout << "<expr> -> <expr> >= <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

LessNode::LessNode( Node* l, Node* r ): Node( l, r ){}

bool LessNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
}

void LessNode::print(){
    cout << "<expr> -> <expr> < <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//

GreaterNode::GreaterNode( Node* l, Node* r ): Node( l, r ){}

bool GreaterNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
}

void GreaterNode::print(){
    cout << "<expr> -> <expr> > <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Product nodes

TimesNode::TimesNode( Node* l, Node* r ): Node( l, r ){}

bool TimesNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void TimesNode::print(){
    cout << "<expr> -> <expr> * <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

DivNode::DivNode( Node* l, Node* r ): Node( l, r ){}

bool DivNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
}

void DivNode::print(){
    cout << "<expr> -> <expr> / <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

ModNode::ModNode( Node* l, Node* r ): Node( l, r ){}

bool ModNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void ModNode::print(){
    cout << "<expr> -> <expr> % <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

ANDNode::ANDNode( Node* l, Node* r ): Node( l, r ){}

bool ANDNode::tryGetType( TableManager* tm, TypeDecl*& result ){ 
    if(left && right){
        result = tm->getIntType();
        return true;
    }
    else{
        return false;
    }
 }

void ANDNode::print(){
    cout << "<expr> -> <expr> && <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Unary nodes

UMinusNode::UMinusNode( Node* r ): Node( 0, r ){}

bool UMinusNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( right && right->tryGetType(tm, result) ){
        return true;
    } 
    return false; 
}

void UMinusNode::print(){
    cout << "<expr> -> -<expr>" << endl;
    if(right){ right->print(); }
}

//// 

UPlusNode::UPlusNode( Node* r ): Node( 0, r ){}

bool UPlusNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( right && right->tryGetType(tm, result) ){
        return true;
    } 
    return false;
}

void UPlusNode::print(){
    cout << "<expr> -> +<expr>" << endl;
    if(right){ right->print(); }
}

//// 

UExclNode::UExclNode( Node* r ): Node( 0, r ){}

bool UExclNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( right && right->tryGetType(tm, result) ){
        return true;
    }
    return false;
 }

void UExclNode::print(){
    cout << "<expr> -> !<expr>" << endl;
    if(right){ right->print(); }
}

////

ParenNode::ParenNode( Node* l ): Node( l, 0 ){}

bool ParenNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    if( left && left->tryGetType(tm, result) ){
        return true;
    }
    return false;
}

void ParenNode::print(){
    cout << "<expr> -> ( <expr> )" << endl;
    if(left){ left->print(); }
}

/////////////////////////////////////////
// New Expression node(s)

NewIdArgsNode::NewIdArgsNode( Node* type, Node* args ): Node( type, args ){}

bool NewIdArgsNode::tryGetType( TableManager* tm , TypeDecl*& result ){
    string name;
    if( left && left->getID(name) && tm->tryLookup(name, result) ){
        return true;

    }
    else{
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
}

void NewIdArgsNode::print(){
    cout << "<newexpr> -> new id ( <arglist> )" << endl;
    if(right){ right->print(); }
}

NewIdNode::NewIdNode( Node* type , Node* bracket ): Node( type, bracket ){}

bool NewIdNode::tryGetType( TableManager* tm , TypeDecl*& result ){
    string name;
    TypeDecl* temp = 0;
    if( left && left->getID(name) && tm->tryLookup(name, temp) ){
        // Check for array type
        int b = 0;
        if(right){
            b = right->gatherBrackets();
        }
        for(int i = 0; i < b; i++){
            name += "[]";
        }
        if(tm->tryLookup(name, result)){
            return true;
        }
        return false;
    }
    else{
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
}

void NewIdNode::print(){
    cout << "<newexpr> -> new id <bracketset>" << endl;
    if(right){ right->print(); }
}

////

NewSimpleNode::NewSimpleNode( Node* type, Node* bracket ):
Node( type, bracket ){}

bool NewSimpleNode::tryGetType( TableManager* tm, TypeDecl*& result ){
    string name;
    TypeDecl* temp = 0;
    if( left && left->getID(name) && tm->tryLookup(name, temp) ){
        // Check for array type
        int b = 0;
        if(right){
            b = right->gatherBrackets();
        }
        for(int i = 0; i < b; i++){
            name += "[]";
        }
        if(tm->tryLookup(name, result)){
            return true;
        }
        return false;
    }
    else{
        cout << "Error: malformed syntax tree" << endl;
        return false;
    }
}

void NewSimpleNode::print(){
    cout << "<newexpr> -> new <simpletype>";
    if(right){
        cout << " <bracketset>" << endl;
    }
    else{
        cout << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }

}

/////////////////////////////////////////
// Bracket nodes
BracketSetNode::BracketSetNode( Node* exprb, Node* b ): Node ( exprb, b ){}

int BracketSetNode::gatherBrackets(){
    int ret = 0;
    if(left){ ret += left->gatherBrackets(); }
    if(right){ ret += right->gatherBrackets(); }
    return ret;
}

void BracketSetNode::print(){
    cout << "<bracketset> -> ";
    if( left && right ){
        cout << "<exprbrackets> <multibrackets>" << endl;
        left->print();
        right->print();
    }
    else{
        if(left){
            cout  << "<exprbrackets>" << endl;
            left->print();
        }
        if(right){
            cout << "<multibrackets>" << endl;
            right->print();
        }
    }
}

BracketNode::BracketNode( Node* next ): Node( next, 0 ) {}

int BracketNode::gatherBrackets(){
    if(left){ return 1 + left->gatherBrackets(); }
    return 1;
}

void BracketNode::print(){
    cout << "<multibrackets> -> ";
    if(left){
        cout << "<multibrackets> [ ]" << endl;
    }
    else{
        cout << "[ ]" << endl;
    }
    if(left){ left->print(); }
}

BracketExprNode::BracketExprNode( Node* next, Node* expr ):
Node( next, expr ) {}

int BracketExprNode::gatherBrackets(){
    if(left){ return 1 + left->gatherBrackets(); }
    return 1;
}

void BracketExprNode::print(){
    cout << "<exprbrackets> -> ";
    if(left){
        cout << "<exprbrackets> [ <expr> ]" << endl;
    }
    else{
        cout << " [ <expr> ]" << endl;
    }
    if(left){ left->print(); }
    if(right){ right->print(); }

}