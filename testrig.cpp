#include<iostream>
#include"tables.h"
using namespace std;

void basicTests();
void dumpEmpty();

int main(){
    basicTests();
    // cout << "Dump on Empty" << endl;
    // dumpEmpty();

    return 0;
}

void dumpEmpty(){
    TableManager t;
    t.dump();
}

void basicTests(){
    TableManager t;

    // GTT TESTS

    // forward entry test
    t.forwardEntryGlobalTypeTable( "TypeA" );
    TypeDecl* typea = 0;
    if(t.tryLookup( "TypeA", typea )){
        cout << "FET test passed" << endl;
    }
    else{
        cout << "FET test failed---" << endl;
    }

    // Set width test

    if(t.resolveForwardGlobalTypeTable("TypeA", 4) && typea->getWidth() == 4){
        cout << "SWT test passed" << endl;
    }
    else{
        cout << "SWT test failed---" << endl;
    }

    // Add existing member test
    typea = 0;
    t.forwardEntryGlobalTypeTable( "TypeA" );
    t.tryLookup( "TypeA", typea );
    if(typea->getName() != "TypeA"){
        cout << "EMT test failed---" << endl;
    }
    else{
        cout << "EMT test passed" << endl;
    }


    // Failed lookup test
    typea = 0;
    if(t.tryLookup("TypeB", typea) || typea != 0){
        cout << "FLT test failed---" << endl;
    }
    else{
        cout << "FLT test passed" << endl;
    }

    ///////////////////////////////////////////////////
    // TypeInst tests
    // Basic Insert test
    t.enterScope("Global::BIT");
    if(t.addTypeInst("TypeA", "VarA")){
        cout << "BIT test passed" << endl;
    }
    else{
        cout << "BIT test failed---" << endl;
    }

    // Basic Lookup test(No Parent)
    TypeInst* vara = 0;
    if(t.tryLookup("VarA", vara) && vara->getName() == "VarA"){
        cout << "BLT test passed" << endl;
    }
    else{
        cout << "BLT test failed---" << endl;
    }

    // Parent Lookup test
    vara = 0;
    t.enterScope("Global::BIT::PLT");
    if(t.tryLookup("VarA", vara) && vara->getName() == "VarA"){
        cout << "PLT test passed" << endl;
    }
    else{
        cout << "PLT test failed---" << endl;
    }

    //2 Parents
    vara = 0;
    t.enterScope("Global::BIT::PLT::2LT");
    if(t.tryLookup("VarA", vara) && vara->getName() == "VarA"){
        cout << "2LT test passed" << endl;
    }
    else{
        cout << "2LT test failed---" << endl;
    }

    // var in child test
    TypeInst* varb = 0;
    t.addTypeInst("TypeA", "VarB");
    t.exitScope();
    if(!t.tryLookup("VarB", varb) || varb != 0){
        cout << "VIC test passed" << endl;
    }
    else{
        cout << "VIC test failed---" << endl;
    }
    t.exitScope();

    ///////////////////////////////////////////////////
    // MethDecl Tests

    // Basic Method test
    vector<string> methaArgs;
    if( t.addMethDecl("MethA", methaArgs, "TypeA") ){
        cout << "BMT test passed" << endl;
    }
    else{
        cout << "BMT test failed---" << endl;
    }

    // Method Lookup test
    MethDecl* metha = 0;
    if( t.tryLookup("MethA", metha) && metha != 0 ){
        cout << "MLT test passed" << endl;
    }
    else{
        cout << "MLT test failed---" << endl;
    }

    // Parent Table Lookup test
    t.enterScope("Global::BIT::PTL");
    metha = 0;
    if( t.tryLookup("MethA", metha) && metha != 0 ){
        cout << "PTL test passed" << endl;
    }
    else{
        cout << "PTL test failed---" << endl;
    }

    // Failed Child Lookup test
    MethDecl* methb = 0;
    t.addMethDecl("MethB", methaArgs, "TypeA");
    t.exitScope();
    if(!t.tryLookup("MethB", methb) && methb == 0){
        cout << "FCL test passed" << endl;
    }
    else{
        cout << "FCL test failed---" << endl;
    }

    // Method args lookup test
    methb = 0;
    methaArgs.push_back("TypeA");
    t.addMethDecl("MethB", methaArgs, "TypeA");
    if(t.tryLookup("MethB", methb) && 
    methb != 0 && 
    methb->getArgTypes().size() == 1){
        cout << "MAL test passed" << endl;
    }
    else{
        cout << "MAL test failed---" << endl;
    }

    // Int type Test
    TypeInst* inta = 0;
    if( t.addTypeInst("int", "IntA")  && 
    t.tryLookup("IntA", inta) &&
    inta->getName() == "IntA"){
        cout << "ITT test passed" << endl;
    }
    else{
        cout << "ITT test failed---" << endl;
    }

    // Void retType test
    MethDecl* voida = 0;
    if( t.addMethDecl("VoidA", methaArgs, "void") && 
    t.tryLookup("VoidA", voida) &&
    voida != 0){
        cout << "VRT test passed" << endl;
    }
    else{
        cout << "VRT test failed---" << endl;
    }
    t.exitScope();

    // This resolution test
    TypeDecl* thisTest = 0;
    t.forwardEntryGlobalTypeTable("TRT");
    t.enterScope("Global::TRT");
    if( t.tryResolveThis( thisTest ) ){
        cout << "TRT test passed" << endl;
    }
    else {
        cout << "TRT test failed---" << endl;
    }

    // local Search test
    TypeDecl* lst = 0;
    if(t.searchLocalTable( "BIT", "VarA", lst ) && lst){
        cout << "LST test passed" << endl;
    }
    else{
        cout << "LST test failed---" << endl;
    }

    t.dump();
}