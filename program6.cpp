// Matt Grant
// Program 6
// Dec 3 2017

#include"tables.h"
#include"node.h"
#include<FlexLexer.h>
#include"program6.tab.h"
#include<vector>
using std::vector;


yyFlexLexer scanner;
vector<Node*> tree;

int main(int argc, char* argv[]){
    yyparse();

    TableManager t;
    bool typeCheck = true;

    for( unsigned int i = 0; i < tree.size(); i++ ){
        if( tree[i]->registerType( &t ) == false ){
            cout << "Error: Type registration failed, typechecking "
            << "will not commence." << endl;
            typeCheck = false;
        }
    }

    if(typeCheck){
        // Register all var declarations
        for( unsigned int i = 0; i < tree.size(); i++ ){
            tree[i]->populateTables( &t );
        }
        // Type check
        for( unsigned int i = 0; i < tree.size(); i++ ){
            tree[i]->typeCheck( &t );
        } 
    }

    t.dump();

    return 0;
}
