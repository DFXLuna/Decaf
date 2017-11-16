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
ClassDecNode::ClassDecNode( Node* classbody, Node* id ): Node( classbody, id ){}

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
    name = tm->getCurrentScope() + "::" + name;
    // Check constructor names
    checkConstructorNames();
    // Populate table
    tm->enterScope(name);
    if(left){ left->populateTables(tm); }
    tm->exitScope();
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

IdMultiIdNode::IdMultiIdNode( Node* type, Node* b, Node* id ): Node( type, id ){
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
            string scope = tm->getCurrentScope();
            scope += ("::" + id);
            tm->enterScope(scope);
            if(block){ block->populateTables( tm ); }
            else{ cout << "Error: Malformed syntax tree" << endl; }
            tm->exitScope();
        } 
   }  
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
            string scope = tm->getCurrentScope();
            scope += ("::" + id);
            tm->enterScope(scope);
            if(block){ block->populateTables( tm ); }
            else{ cout << "Error: Malformed syntax tree" << endl; }
            tm->exitScope();
        } 
   }     
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
        string scope = tm->getCurrentScope();
        scope += ("::" + id);
        tm->enterScope(scope);
        if(block){ block->populateTables( tm ); }
        else{ cout << "Error: Malformed syntax tree" << endl; }
        tm->exitScope();
   }     
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
        if(!result->getID(retType)){ cout << "Error: malformed syntax tree" << endl; }
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
        string scope = tm->getCurrentScope();
        scope += ("::" + id);
        tm->enterScope(scope);
        if(block){ block->populateTables( tm ); }
        else{ cout << "Error: Malformed syntax tree" << endl; }
        tm->exitScope();
   }
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

bool ParameterListNode::gatherParams( vector<string>& types, vector<string>& ids ){
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

bool ParameterIDNode::gatherParams( vector<string>& types, vector<string>& ids ){
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
void EmptyStatementNode::print(){
    cout << "<stmt> -> ;" << endl;
}

////

EQStatementNode::EQStatementNode( Node* name, Node* expr ):
Node( name, expr ){}

void EQStatementNode::print(){
    cout << "<stmt> -> <name> = <expr>;" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

FuncStatementNode::FuncStatementNode( Node* name, Node* arglist ):
Node( name, arglist ){}

void FuncStatementNode::print(){
    cout << "<stmt> -> <name> ( <arglist> );" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

PrintStatementNode::PrintStatementNode( Node* arglist ): Node( arglist, 0 ){}

void PrintStatementNode::print(){
    cout << "<stmt> -> print( <arglist> );" << endl;
    if(left){ left->print(); }
}

////

WhileStatementNode::WhileStatementNode( Node* expr, Node* stmt ):
Node( expr, stmt ){}

void WhileStatementNode::print(){
    cout << "<stmt> -> while ( <expr> ) <stmt>;" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

ReturnStatementNode::ReturnStatementNode( Node* optexpr ): Node( optexpr, 0 ){}

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

void ThisNode::print(){
    cout << "<name> -> this" << endl;
}

////

NameIdNode::NameIdNode( Node* id ): Node ( id, 0 ){}

void NameIdNode::print(){
    cout << "<name> -> id" << endl;
}

////

NameDotIdNode::NameDotIdNode( Node* name, Node* Id ): Node( name, Id ){}

void NameDotIdNode::print(){
    cout << "<name> -> <name> . id" << endl;
    if(left){ left->print(); }
}

////

// This assigns the name subtree to left and the expr subtree to right
NameExprNode::NameExprNode( Node* name, Node* expr ): Node( name, expr ){}

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

void ExprNameNode::print(){
    cout << "<expr> -> <name>" << endl;
    if(left){ left->print(); }
}

////

NumNode::NumNode(int v){
    val = v;
}

////

NullNode::NullNode(): Node( 0, 0 ){}

void NullNode::print(){
    cout << "<expr> -> null" << endl;
}

////

ReadNode::ReadNode(): Node( 0, 0 ){}

void ReadNode::print(){
    cout << "<expr> -> read ( )" << endl;
}

////

NewExprNode::NewExprNode( Node* newexpr ): Node( newexpr, 0 ){}

void NewExprNode::print(){
    cout << "<expr> -> <newexpr>" << endl;
    if(left){ left->print(); }
}

////

MethodCallNode::MethodCallNode( Node* name, Node* arglist ):
Node( name, arglist ){}

void MethodCallNode::print(){
    cout << "<expr> -> <name> ( <arglist> )" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

////

void NumNode::print(){
    cout << "<expr> -> number" << endl;
}

////

// Sum nodes
SumNode::SumNode( Node* l, Node* r ): Node( l, r ){}

void SumNode::print(){
    cout << "<expr> -> <expr> + <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

MinusNode::MinusNode( Node* l, Node* r ): Node( l, r ){}

void MinusNode::print(){
    cout << "<expr> -> <expr> - <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

ORNode::ORNode( Node* l, Node* r ): Node( l, r ){}

void ORNode::print(){
    cout << "<expr>-> <expr> || <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Relation Nodes
EqNode::EqNode( Node* l, Node* r ): Node( l, r ){}

void EqNode::print(){
    cout << "<expr> -> <expr> == <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

NeqNode::NeqNode( Node* l, Node* r ): Node( l, r ){}

void NeqNode::print(){
    cout << "<expr> -> <expr> != <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

LeqNode::LeqNode( Node* l, Node* r ): Node( l, r ){}

void LeqNode::print(){
    cout << "<expr> -> <expr> <= <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

GeqNode::GeqNode( Node* l, Node* r ): Node( l, r ){}

void GeqNode::print(){
    cout << "<expr> -> <expr> >= <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

LessNode::LessNode( Node* l, Node* r ): Node( l, r ){}

void LessNode::print(){
    cout << "<expr> -> <expr> < <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//

GreaterNode::GreaterNode( Node* l, Node* r ): Node( l, r ){}

void GreaterNode::print(){
    cout << "<expr> -> <expr> > <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Product nodes

TimesNode::TimesNode( Node* l, Node* r ): Node( l, r ){}

void TimesNode::print(){
    cout << "<expr> -> <expr> * <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

DivNode::DivNode( Node* l, Node* r ): Node( l, r ){}

void DivNode::print(){
    cout << "<expr> -> <expr> / <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

ModNode::ModNode( Node* l, Node* r ): Node( l, r ){}

void ModNode::print(){
    cout << "<expr> -> <expr> % <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

//// 

ANDNode::ANDNode( Node* l, Node* r ): Node( l, r ){}

void ANDNode::print(){
    cout << "<expr> -> <expr> && <expr>" << endl;
    if(left){ left->print(); }
    if(right){ right->print(); }
}

/////////////////////////////////////////
// Unary nodes

UMinusNode::UMinusNode( Node* r ): Node( 0, r ){}

void UMinusNode::print(){
    cout << "<expr> -> -<expr>" << endl;
    if(right){ right->print(); }
}

//// 

UPlusNode::UPlusNode( Node* r ): Node( 0, r ){}

void UPlusNode::print(){
    cout << "<expr> -> +<expr>" << endl;
    if(right){ right->print(); }
}

//// 

UExclNode::UExclNode( Node* r ): Node( 0, r ){}

void UExclNode::print(){
    cout << "<expr> -> !<expr>" << endl;
    if(right){ right->print(); }
}

////

ParenNode::ParenNode( Node* l ): Node( l, 0 ){}

void ParenNode::print(){
    cout << "<expr> -> ( <expr> )" << endl;
    if(left){ left->print(); }
}

/////////////////////////////////////////
// New Expression node(s)

NewIdArgsNode::NewIdArgsNode( Node* type, Node* args ): Node( type, args ){}

void NewIdArgsNode::print(){
    cout << "<newexpr> -> new id ( <arglist> )" << endl;
    if(right){ right->print(); }
}

NewIdNode::NewIdNode( Node* type , Node* bracket ): Node( type, bracket ){}

void NewIdNode::print(){
    cout << "<newexpr> -> new id <bracketset>" << endl;
    if(right){ right->print(); }
}

////

NewSimpleNode::NewSimpleNode( Node* type, Node* bracket ):
Node( type, bracket ){}

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