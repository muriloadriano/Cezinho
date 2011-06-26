%{
/*
 *
 *  Created by   Murilo Adriano Vasconcelos	  || Matrícula : 094816 
 *				& Paulo Cezar Pereira Costa   || Matrícula : 080153
 *
 *  Copyright 2011 Universidade Federal de Goias. All rights reserved.
 *
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>

#include "ast/ast.h"

#define TRACE printf("reduce at line %d\n", __LINE__);

extern int yylex(void);
void yyerror(const char *s);
void yyerror(const char *s, int err_line);
extern int yylineno;
extern FILE *yyin;

std::string cz_new_line = "\n";

ASTNode* root;

%}

%union {
	ASTNode *node;
	DataType datatype;
	Op oper;
	std::string *lxval;
}

%token<lxval> MAIN
%token INT
%token CHAR
%token RETURN
%token READ
%token WRITE
%token WRITELN
%token BREAK
%token IF
%token ELSE
%token WHILE
%token<lxval> ID
%token<lxval> LITINT
%token<lxval> LITCHAR
%token<lxval> LITSTRING

%left '='
%left OR
%left AND
%nonassoc EQUAL NEQUAL
%nonassoc '<' '>' LESSEQ GREATEREQ
%left '+' '-'
%left '*' '/'

%type<datatype> Type
%type<oper> UnaryOp BinOp AritmOp RelOp BooleanOp
%type<node> Program Decls VarDecl FuncDecl ParamDecList Block VarDeclList StmtList Stmt Expr BinaryExpr UnaryExpr PostFixExpr ArgumentList Constant

%start Program

%%

Program:
		Decls								{	root = $1; root->reverse();
												scope_lvl = 0; var_symbol_tab.clear(); func_symbol_tab.clear();
												while(!declared.empty())declared.pop(); declared.push("$");
												root->walk(0);
											}
;

Decls:
		Type VarDecl ';' Decls				{ $$ = $4; ((VarDecl*)$2)->setDataType( $1 ); $$->add( $2 ); $$->set_location( yylineno ); }
		| Type ID FuncDecl Decls 			{ $$ = $4; ((FuncDecl*)$3)->setFuncName( $2 ); ((FuncDecl*)$3)->setDataType( $1 ); $$->add( $3 ); $$->set_location( yylineno ); }
		| Type MAIN FuncDecl				{ $$ = new ASTNode(); ((FuncDecl*)$3)->setFuncName( $2 ); ((FuncDecl*)$3)->setDataType( $1 );	$$->add( $3 ); $$->set_location( yylineno ); }
;

VarDecl:
		ID									{ $$ = new VarDecl(); $$->add( new DeclIdentifier($1) ); $$->set_location( yylineno ); }
		| ID '[' Constant ']'				{ $$ = new VarDecl(); $$->add( new DeclIdentifier($1, (ConstExpr*)$3 ) ); $$->set_location( yylineno ); }
		| VarDecl ',' ID					{ $$ = $1; $$->add( new DeclIdentifier($3) ); $$->set_location( yylineno ); }
		| VarDecl ',' ID '[' Constant ']'	{ $$ = $1; $$->add( new DeclIdentifier($3, (ConstExpr*)$5 ) ); $$->set_location( yylineno ); }
;

FuncDecl:
		'(' ParamDecList ')' Block			{ $$ = new FuncDecl( (ParamList*)$2, (Block*)$4 ); $$->set_location( yylineno ); }
		| '(' ')' Block						{ $$ = new FuncDecl( NULL, (Block*)$3 ); $$->set_location( yylineno ); }
;

ParamDecList:
		Type ID								{ $$ = new ParamList(); $$->add( new Param( $1, $2 ) ); $$->set_location( yylineno ); }
		| Type ID '[' ']'					{ $$ = new ParamList(); $$->add( new Param( ($1==INT_T)?INT_ARRAY_T:CHAR_ARRAY_T, $2 ) ); $$->set_location( yylineno ); }
		| ParamDecList ',' Type ID			{ $$ = $1; $$->add( new Param( $3, $4 ) ); $$->set_location( yylineno );	}
		| ParamDecList ',' Type ID '[' ']'	{ $$ = $1; $$->add( new Param( ($3==INT_T)?INT_ARRAY_T:CHAR_ARRAY_T, $4 ) ); $$->set_location( yylineno ); }
;

Block:
		'{' VarDeclList StmtList '}'		{ $$ = new Block( (VarDeclList*)$2, (StatementList*)$3 ); $$->set_location( yylineno ); }
		| '{' VarDeclList '}'				{ $$ = new Block( (VarDeclList*)$2 ); $$->set_location( yylineno ); }
		| '{' StmtList '}'					{ $$ = new Block( (StatementList*)$2 ); $$->set_location( yylineno ); }
;

VarDeclList:
		Type VarDecl ';'					{ $$ = new VarDeclList(); ((VarDecl*)$2)->setDataType($1); $$->add($2); $$->set_location( yylineno ); }
		| VarDeclList Type VarDecl ';'		{ $$ = $1; ((VarDecl*)$3)->setDataType($2); $$->add( $3 ); $$->set_location( yylineno );  }
;

Type:
		INT							{ $$ = INT_T;  }
		| CHAR						{ $$ = CHAR_T; }
;

StmtList:
		Stmt						{ $$ = new StatementList(); $$->add( $1 ); $$->set_location( yylineno ); }
		| StmtList Stmt				{ $$ = $1; $$->add( $2 ); $$->set_location( yylineno ); }
;

Stmt:
		';'									{ $$ = new Statement(); $$->set_location( yylineno ); }
		| Expr ';'							{ $$ = $1; $$->set_location( yylineno ); }
		| RETURN Expr ';'					{ $$ = new Return( (Expression*)$2 ); $$->set_location( yylineno ); }
		| READ ID ';'						{ $$ = new Read( new Identifier( $2 ) ); $$->set_location( yylineno ); }
		| READ ID '[' Expr ']' ';'			{ $$ = new Read( new Identifier( $2, (Expression*)$4) ); $$->set_location( yylineno ); }
		| WRITE Expr ';'					{ $$ = new Write( (Expression*)$2 ); $$->set_location( yylineno ); }
		| WRITELN ';'						{ $$ = new Write( new ConstExpr( CHAR_ARRAY_T, &cz_new_line ) ); $$->set_location( yylineno ); }
		| BREAK ';'							{ $$ = new Break(); $$->set_location( yylineno ); }
		| IF '(' Expr ')' Stmt				{ $$ = new If( (Expression*)$3, (Statement*)$5 ); $$->set_location( yylineno ); }
		| IF '(' Expr ')' Stmt ELSE Stmt	{ $$ = new If( (Expression*)$3, (Statement*)$5, (Statement*)$7 ); $$->set_location( yylineno ); }
		| WHILE '(' Expr ')' Stmt			{ $$ = new While( (Expression*)$3, (Statement*)$5 ); $$->set_location( yylineno ); }
		| Block								{ $$ = $1; $$->set_location( yylineno ); }
;

Expr:
		UnaryExpr '=' Expr			{ 	
										Identifier* aux = dynamic_cast<Identifier*>($1);
										if( aux == NULL ) yyerror( "Lado esquerdo de uma atribuicao deve ser um identificador." );
										else $$ = new Assignment(aux, (Expression*)$3);
										$$->set_location( yylineno );
									}
		| BinaryExpr				{ $$ = $1; $$->set_location( yylineno ); }
;

BinaryExpr:
		BinaryExpr BinOp UnaryExpr	{ $$ = new BinaryExpr( $2, (BinaryExpr*)$1, (UnaryExpr*)$3 ); $$->set_location( yylineno ); }
		| UnaryExpr					{ $$ = $1; $$->set_location( yylineno ); }
;

UnaryExpr:
		UnaryOp UnaryExpr			{ $$ = new UnaryExpr( $1, (UnaryExpr*)$2 ); $$->set_location( yylineno ); }
		| PostFixExpr				{ $$ = $1; $$->set_location( yylineno );  }
;

PostFixExpr:
		ID '(' ArgumentList ')'		{ $$ = new FuncCall( $1, (ArgList*)$3 ); $$->set_location( yylineno ); }
		| ID '(' ')'				{ $$ = new FuncCall( $1 ); $$->set_location( yylineno ); }
		| ID						{ $$ = new Identifier( $1 ); $$->set_location( yylineno ); }
		| ID '[' Expr ']'			{ $$ = new Identifier( $1, (Expression*)$3 ); $$->set_location( yylineno ); }
		| Constant					{ $$ = $1; $$->set_location( yylineno ); }
		| '(' Expr ')'				{ $$ = $2; $$->set_location( yylineno ); }
;

Constant:
		LITINT						{ $$ = new ConstExpr( INT_T, $1 ); $$->set_location( yylineno ); }
		| LITCHAR					{ $$ = new ConstExpr( CHAR_T, $1 ); $$->set_location( yylineno ); }
		| LITSTRING					{ $$ = new ConstExpr( CHAR_ARRAY_T, $1 ); $$->set_location( yylineno ); }
;

ArgumentList:
		Expr						{ $$ = new ArgList(); $$->add( $1 ); $$->set_location( yylineno ); }
		| ArgumentList ',' Expr		{ $$ = $1; $$->add( $3 ); $$->set_location( yylineno ); }
;

UnaryOp:
		'-'							{ $$ = MINUS; }
		| '!'						{ $$ = NOT; }
;

BinOp:
		BooleanOp					{ $$ = $1; }
		| RelOp						{ $$ = $1; }
		| AritmOp					{ $$ = $1; }
;

AritmOp:
		'+'							{ $$ = PLUS; }
		| '-'						{ $$ = MINUS; }
		| '*'						{ $$ = TIMES; }
		| '/'						{ $$ = DIVIDES; }
;

RelOp:
		'<'							{ $$ = LESS; }
		| '>'						{ $$ = GREATER; }
		| EQUAL						{ $$ = EQUALS; }
		| NEQUAL					{ $$ = NOT_EQUAL; }
		| LESSEQ					{ $$ = LESS_EQUAL; }
		| GREATEREQ					{ $$ = GREATER_EQUAL; }
;

BooleanOp:
		OR							{ $$ = LOGICAL_OR; }
		| AND						{ $$ = LOGICAL_AND; }
;


%%

void yyerror(const char *s) {
	fprintf(stderr, "ERRO! Linha: %d - %s\n", yylineno, s );
	exit(1);
}

void yyerror(const char *s, int err_line ) {
	fprintf(stderr, "ERRO! Proximo a linha: %d - %s\n", err_line, s );
	exit(1);
}

int main( int argc, char** argv ){
	
	if( argc > 1 ) yyin = fopen( argv[1], "r" );
	else fprintf( stdout, "Nenhum arquivo de entrada especificado.. Fazendo analise da entrada padrao..\n");
	
	yyparse();
	fprintf(stdout, "ANALISE SINTATICA OK.\n");
	
	return 0;
}
