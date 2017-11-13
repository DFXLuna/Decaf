#include<iostream>
#include"tables.h"
using namespace std;


int main(){
    TableManager t;
    TypeDecl* ty = new TypeDecl( "TypeA" );

    // GTT TESTS

    // forward entry test
    TypeDecl* typea = 0;
    t.forwardEntryGlobalTypeTable( "TypeA", ty );
    if(t.tryLookup( "TypeA", typea )){
        cout << "FET test passed" << endl;
        cout << "FET: ";
        typea->print();
        cout << endl;
    }
    else{
        cout << "FET test failed" << endl;
    }

    // Set width test

    if(t.setWidthGlobalTypeTable("TypeA", 4)){
        cout << "SWT test passed" << endl;
        cout << "SWT: ";
        typea->print();
        cout << endl << "Width: " << typea->getWidth() << endl;
    }
    else{
        cout << "SWT test failed" << endl;
    }

    // Add existing member test
    typea = 0;
    TypeDecl* ty2 = new TypeDecl( "TypeB" );
    t.forwardEntryGlobalTypeTable( "TypeA", ty2 );
    t.tryLookup( "TypeA", typea );
    if(typea->getName() != "TypeA"){
        cout << "AEMT test failed" << endl;
        cout << "AEMT: ";
        typea->print();
        cout << endl;
    }
    else{
        cout << "AEMT test passed" << endl;
        cout << "AEMT: ";
        typea->print();
        cout << endl;
    }


    // Failed lookup test
    typea = 0;
    if(t.tryLookup("TypeB", typea) || typea != 0){
        cout << "FLT test failed" << endl;
        cout << "FLT: ";
        typea->print();
        cout << endl;  
    }
    else{
        cout << "FLT test passed" << endl;
    }

    ///////////////////////////////////////////////////
    // General Table tests
    // Basic Insert Test
    t.enterScope();
    if(t.addTypeInst("TypeA", "VarA")){
        cout << "BIT Test passed" << endl;
    }
    else{
        cout << "BIT Test failed" << endl;
    }


    return 0;
}