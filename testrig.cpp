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
    }
    else{
        cout << "FET test failed" << endl;
    }

    // Set width test

    if(t.setWidthGlobalTypeTable("TypeA", 4) && typea->getWidth() == 4){
        cout << "SWT test passed" << endl;
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
        cout << "EMT test failed" << endl;
    }
    else{
        cout << "EMT test passed" << endl;
    }


    // Failed lookup test
    typea = 0;
    if(t.tryLookup("TypeB", typea) || typea != 0){
        cout << "FLT test failed" << endl;
    }
    else{
        cout << "FLT test passed" << endl;
    }

    ///////////////////////////////////////////////////
    // General Table tests
    // Basic Insert test
    t.enterScope();
    if(t.addTypeInst("TypeA", "VarA")){
        cout << "BIT test passed" << endl;
    }
    else{
        cout << "BIT test failed" << endl;
    }

    // Basic Lookup test(No Parent)
    TypeInst* vara = 0;
    if(t.tryLookup("VarA", vara) && vara->getName() == "VarA"){
        cout << "BLT test passed" << endl;
    }
    else{
        cout << "BLT test failed" << endl;
    }

    // Parent Lookup test
    vara = 0;
    t.enterScope();
    if(t.tryLookup("VarA", vara) && vara->getName() == "VarA"){
        cout << "PLT test passed" << endl;
    }
    else{
        cout << "PLT test failed" << endl;
    }

    //2 Parents
    vara = 0;
    t.enterScope();
    if(t.tryLookup("VarA", vara) && vara->getName() == "VarA"){
        cout << "2LT test passed" << endl;
    }
    else{
        cout << "2LT test failed" << endl;
    }

    // var in child test
    TypeInst* varb = 0;
    t.addTypeInst("TypeA", "VarB");
    t.exitScope();
    if(!t.tryLookup("VarB", varb) || varb != 0){
        cout << "VIC test passed" << endl;
    }
    else{
        cout << "VIC test failed" << endl;
    }

    return 0;
}