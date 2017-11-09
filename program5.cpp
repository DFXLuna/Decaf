// Matt Grant
// Program 5
// Nov 9 2017

#include"node.h"
#include<FlexLexer.h>
#include"program5.tab.h"
#include<vector>
using std::vector;


yyFlexLexer scanner;
vector<Node*> tree;

int main(int argc, char* argv[]){
    yyparse();
    // Vector needed because multiple programs per run
    for( unsigned int i = 0; i < tree.size(); i++){
        tree[i]->print();
    }
    return 0;
}
