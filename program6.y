/* Matt Grant
 * Program 6
 * Dec 3 2017
 */
%{
    #include"node.h"
    #include<FlexLexer.h>
    #include<vector>
    #include<string>
    using std::vector;
    using std::string;
    extern yyFlexLexer scanner;
    extern vector<Node*> tree;

    #define yylex() scanner.yylex()

    string getyytext();
    int getLine();
    int getCol();
    void yyerror(const char*);
    void printSemanticError(string e, int column, int line);
%}

%union{ 
    Node* ttype;
}

%destructor { delete $$; } <ttype>

%type<ttype> newexpr expr vardecl name
%type<ttype> multibrackets exprbrackets bracketset
%type<ttype> program stmt arglist arg optexpr cond type simpletype block
%type<ttype> localvars stmts localvar methdecl plist param condecl typeid
%type<ttype> classbody vardecls methdecls condecls classdec
%token<ttype> NUM ID
%token SEMICOLON DOT COMMA
%token THIS NULLOP READ INT NEW PRINT WHILE RETURN IF VOID CLASS
%right THEN ELSE
%token LPAREN RPAREN LBRACE RBRACE LSBRACE RSBRACE
%left EQ NEQ LEQ GEQ LESS GREATER
%left PLUS MINUS OR
%left TIMES DIV MOD AND
%token EXCL
%precedence UNARYOP

%%

program: classdec {
         // Because I store the class decs in a vector, program doesn't need a
         // value but I'm leaving it open just in case
         $$ = 0;
         tree.push_back($1);
     }
     | program classdec {
         // This stops bison from complaining about unused values
         // it will always be 0
         $$ = $1;
         tree.push_back($2);
     }
;

classdec: CLASS ID classbody {
            $$ = new ClassDecNode($3, $2);
        }
;

classbody: LSBRACE vardecls condecls methdecls RSBRACE {
             $$ = new ClassBodyNode($2, $3, $4);
         }
         | LSBRACE condecls methdecls RSBRACE {
             $$ = new ClassBodyNode(0, $2, $3);
         }
         | LSBRACE vardecls methdecls RSBRACE {
             $$ = new ClassBodyNode($2, 0, $3);
         }
         | LSBRACE vardecls condecls RSBRACE {
             $$ = new ClassBodyNode($2, $3, 0);
         }
         | LSBRACE vardecls RSBRACE {
             $$ = new ClassBodyNode($2, 0, 0);
         }
         | LSBRACE condecls RSBRACE {
             $$ = new ClassBodyNode(0, $2, 0);
         }
         | LSBRACE methdecls RSBRACE {
             $$ = new ClassBodyNode(0, 0, $2);
         }
         | LSBRACE RSBRACE {
             $$ = new ClassBodyNode(0, 0, 0);
         }
         | LSBRACE error RSBRACE {
            $$ = 0;
            printSemanticError( getyytext(),
                                getCol(),
                                getLine());
            yyerror(0);
         }
;

vardecls: vardecl {
            $$ = new VarDeclsNode($1, 0);
        }
        | vardecls vardecl {
            $$ = new VarDeclsNode($2, $1);
        }
;

condecls: condecl {
            $$ = new ConDeclsNode($1, 0);
        }
        | condecls condecl {
            $$ = new ConDeclsNode($2, $1);
        }
;

methdecls: methdecl {
             $$ = new MethDeclsNode($1, 0);
         }
         | methdecl methdecls {
             $$ = new MethDeclsNode($1, $2);
         }
;

vardecl: typeid SEMICOLON {
           $$ = new VarTypeIdNode($1);
       }
       | ID ID SEMICOLON {
           $$ = new IdIdNode($1, $2);
       }
       | ID multibrackets ID SEMICOLON {
           $$ = new IdMultiIdNode($1, $2, $3);
       }
       | error SEMICOLON {
            $$ = 0;
            printSemanticError( getyytext(),
                                getCol(),
                                getLine());
            yyerror(0);
       }
;
    
type: simpletype {
        $$ = new TypeNode($1, 0);
    }
    | simpletype multibrackets {
        $$ = new TypeNode($1, $2);
    }
;

simpletype: INT {
              $$ = new SimpleTypeNode();
          }
;

condecl: ID LPAREN plist RPAREN block {
          $$ = new ConstructorDecNode($1, $3, $5);
       }
       | error block {
           $$ = 0;
           delete $2;
           printSemanticError( getyytext(),
                               getCol(),
                               getLine());
           yyerror(0);
       }
       | ID LPAREN error RPAREN block {
           $$ = 0;
           delete $1;
           $5 = 0;
           printSemanticError( getyytext(),
                               getCol(),
                               getLine());
           yyerror(0);
       }
       | ID LPAREN plist RPAREN error {
           $$ = 0;
           delete $1;
           delete $3;
           printSemanticError( getyytext(),
                               getCol(),
                               getLine());
           yyerror(0);
       }
;

methdecl: typeid LPAREN plist RPAREN block {
           $$ = new MethodDecNode($1, $3, $5);
       }
       | VOID ID LPAREN plist RPAREN block {
           $$ = new VoidMethodDecNode($2, $4, $6);
       }
       | ID ID LPAREN plist RPAREN block {
           $$ = new IDMethodDecNode($1, $2, $4, $6);;
       }
       | ID ID LPAREN error RPAREN block {
            $$ = 0;
            delete $1;
            delete $2;
            delete $6;
            printSemanticError( getyytext(),
                                getCol(),
                                getLine() );
            yyerror(0);
       }
       | ID ID LPAREN plist RPAREN error {
            $$ = 0;
            delete $1;
            delete $2;
            delete $4;
            printSemanticError( getyytext(),
                                getCol(),
                                getLine());
            yyerror(0);           
       }
;

typeid: type ID {
          $$ = new TypeIdNode($1, $2);
      }
;

plist: %empty {
         $$ = new ParameterListNode(0);
     }
     | param {
         $$ = new ParameterListNode($1);
     }
;

param: type ID {
         $$ = new ParameterNode($1, $2, 0);
     }
     | type ID COMMA param {
         $$ = new ParameterNode($1, $2, $4); 
     }
     | ID ID {
         $$ = new ParameterIDNode($1, 0, $2, 0);
     }
     | ID ID COMMA param {
         $$ = new ParameterIDNode($1, 0, $2, $4);
     }
     | ID multibrackets ID {
         $$ = new ParameterIDNode($1, $2, $3, 0);
     }
     | ID multibrackets ID COMMA param {
         $$ = new ParameterIDNode($1, $2, $3, $5);
     }
     
;

block: LSBRACE RSBRACE {
         $$ = new BlockNode(0, 0);
     }
     | LSBRACE localvars RSBRACE {
         $$ = new BlockNode($2, 0);
     }
     | LSBRACE stmts RSBRACE {
         $$ = new BlockNode(0, $2);
     }
     | LSBRACE localvars stmts RSBRACE {
         $$ = new BlockNode($2, $3);
     }
;

localvars: localvar {
             $$ = new LocalVarsNode($1, 0);
         }
         | localvars localvar {
             $$ = new LocalVarsNode($2, $1);
         }
;

localvar: type ID SEMICOLON {
            $$ = new LocalVarDecNode($1, $2);
        }
        | ID ID SEMICOLON {
            $$ = new LocalVarDecIDNode($1, $2, 0);
        }
        | ID multibrackets ID SEMICOLON {
            $$ = new LocalVarDecIDNode($1, $3, $2);
        }
;

stmts: stmt {
         $$ = new StatementsNode($1, 0);
     }
     | stmt stmts {
         $$ = new StatementsNode($1, $2);
     }
;

stmt: SEMICOLON {
        $$ = new EmptyStatementNode();
    }
    | name EQ expr SEMICOLON {
        $$ = new EQStatementNode($1, $3);
    }
    | name LPAREN arglist RPAREN SEMICOLON {
        $$ = new FuncStatementNode($1, $3);
    }
    | PRINT LPAREN arglist RPAREN SEMICOLON {
        $$ = new PrintStatementNode($3);
    }
    | cond {
        $$ = $1;
    }
    | WHILE LPAREN expr RPAREN stmt {
        $$ = new WhileStatementNode($3, $5);
    }
    | RETURN optexpr SEMICOLON {
        $$ = new ReturnStatementNode($2);
    }
    | block {
        $$ = $1;
    }
;

name: THIS {
        $$ = new ThisNode();
    }
    | ID {
        $$ = new NameIdNode($1);
    }
    | name DOT ID {
        $$ = new NameDotIdNode($1, $3);
    }
    | name LBRACE expr RBRACE {
        $$ = new NameExprNode($1, $3);
    }
;

arglist: %empty { 
      $$ = new ArgListNode(0);
  }
  | arg {
      $$ = new ArgListNode($1);
  }
;

arg: expr {
        $$ = new ArgNode($1, 0);
    }
    | expr COMMA arg {
        $$ = new ArgNode($1, $3);
    }
;

cond: IF LPAREN expr RPAREN stmt %prec THEN {
         $$ = new CondStatementNode($3, $5, 0);
     }
     | IF LPAREN expr RPAREN stmt ELSE stmt {
         $$ = new CondStatementNode($3, $5, $7);
     }
;

optexpr: %empty {
           $$ = 0;
       }
       | expr {
           $$ = new OptExprNode($1);
       }
;

expr: name {
        $$ = new ExprNameNode($1);
    }
    |
    NUM {
        $$ = $1;
    }
    | NULLOP {
        $$ = new NullNode();
    }
    | name LPAREN arglist RPAREN {
        $$ = new MethodCallNode($1, $3);
    }
    | READ LPAREN RPAREN {
        $$ = new ReadNode();
    }
    | newexpr{
        $$ = new NewExprNode($1);
    }
    | expr PLUS expr  {
        $$ = new SumNode($1, $3);
    }
    | expr MINUS expr {
        $$ = new MinusNode($1, $3);
    }
    | expr OR expr {
        $$ = new ORNode($1, $3);
    }
    | expr EQ expr {
        $$ = new EqNode($1, $3);
    }
    | expr NEQ expr {
        $$ = new NeqNode($1, $3);
    }
    | expr LEQ expr {
        $$ = new LeqNode($1, $3);
    }
    | expr GEQ expr {
        $$ = new GeqNode($1, $3);
    }
    | expr LESS expr {
        $$ = new LessNode($1, $3);
    }
    | expr GREATER expr {
        $$ = new GreaterNode($1, $3);
    }
    | expr TIMES expr {
        $$ = new TimesNode($1, $3);
    }
    | expr DIV expr {
        $$ = new DivNode($1, $3);
    }
    | expr MOD expr {
        $$ = new ModNode($1, $3);
    }
    | expr AND expr {
        $$ = new ANDNode($1, $3);
    }
    | PLUS expr %prec UNARYOP {
        $$ = new UPlusNode($2);
    }
    | MINUS expr %prec UNARYOP {
        $$ = new UMinusNode($2);
    }
    | EXCL expr %prec UNARYOP {
        $$ = new UExclNode($2);
    }
    | LPAREN expr RPAREN {
        $$ = new ParenNode($2);
    }
;

newexpr: NEW ID LPAREN arglist RPAREN {
           $$ = new NewIdArgsNode($2, $4); 
       }       
       | NEW ID bracketset {
           $$ = new NewIdNode($2, $3);
       }
       | NEW simpletype {
           $$ = new NewSimpleNode($2, 0); 
       }
       | NEW simpletype bracketset {
           $$ = new NewSimpleNode($2, $3);
       }
;

bracketset: exprbrackets {
               $$ = new BracketSetNode($1, 0);
           }
           | multibrackets {
               $$ = new BracketSetNode(0, $1);
           }
           | exprbrackets multibrackets{
               $$ = new BracketSetNode($1, $2);
           }
;

multibrackets: LBRACE RBRACE {
                 $$ = new BracketNode(0);
             }
             | multibrackets LBRACE RBRACE {
                 $$ = new BracketNode($1);
             }
;

exprbrackets: LBRACE expr RBRACE {
                $$ = new BracketExprNode(0, $2);
            }
            | exprbrackets LBRACE expr RBRACE {
                $$ = new BracketExprNode($1, $3);
            }


%%