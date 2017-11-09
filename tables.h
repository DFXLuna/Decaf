// This holds everything needed for symbol tables: tablemanaber, table and typeinfo
class Table;
class TypeInfo;


class TableManager{
public:
    TableManager();
    ~TableManager();
    bool addTable();
    void exitScope();
    bool addEntry( /* STUFF NEEDED FOR TYPEINFO CREATION*/ );

    // Takes an empty typeinfo reference
    // Populates it and returns true if found or false if not
    bool tryLookup( TypeInfo& /* AND STUFF FOR FINDING THE TYPE */ );
private:
    Table* tables;
    Table* currTable;
};

class Table{
public:
    ~Table();
    bool tryLookup( TypeInfo& /* AND STUFF FOR FINDING THE TYPE */ );
    bool addEntry(/* STUFF NEEDED FOR TYPEINFO CREATION*/);
private:
    Table* parent;
};

class TypeInfo{
    TypeInfo();
    ~TypeInfo();
};
