/*
 *
 *  Created by   Murilo Adriano Vasconcelos	  || Matrícula : 094816 
 *				& Paulo Cezar Pereira Costa   || Matrícula : 080153
 *
 *  Copyright 2011 Universidade Federal de Goias. All rights reserved.
 *
 */

#ifndef CEZINHO_AST_H_
#define CEZINHO_AST_H_

#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <stack>
#include <set>
#include <sstream>

void yyerror(const char *);
void yyerror(const char *, int);

class Identifier;
class FuncDecl;
struct Variable;
class ConstExpr;

enum DataType 
{
	INT_T = 0x0,
	CHAR_T = 0x1,
	INT_ARRAY_T = 0x2,
	CHAR_ARRAY_T = 0x3,
};

enum Op
{
        PLUS,
        MINUS,
        TIMES,
        DIVIDES,
        GREATER,
        LESS,
	EQUALS,
        NOT_EQUAL,
	LESS_EQUAL,
	GREATER_EQUAL,
        LOGICAL_OR,
        LOGICAL_AND,
	NOT
};

typedef std::map<std::string, std::stack<DataType> > VarSymTab;
typedef std::map<std::string, FuncDecl*> FuncSymTab;

static VarSymTab var_symbol_tab;
static FuncSymTab func_symbol_tab;


inline std::string getTypeName(DataType t)
{
	switch (t) {
		case INT_T: return "int";
		case CHAR_T: return "char";
		case INT_ARRAY_T: return "int[]";
		case CHAR_ARRAY_T: return "char[]";
	}
}

#define REP( i, N ) for( int i = 0; i < N; i++ )
#define IDENT(depth) REP( i, depth) std::cout<< "\t";

class ASTNode {
	protected:
		std::vector< ASTNode* > child;
		int node_location;
	public:
		ASTNode() { child.clear(); }
		
		virtual void add(ASTNode* node) { child.push_back(node); }
		
		void set_location(int lineno) { node_location = lineno; }
		
		void reverse() { 
			std::reverse(child.begin(), child.end()); 
		}
		
		virtual void walk( int depth ){
			IDENT( depth )
			std::cout << " em um ASTNode qualquer.." << std::endl;
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
		}
};

class Statement : public ASTNode {
	public:
		
		void walk( int depth ){
			IDENT( depth ); std::cout << "fake statement" << std::endl;
		}
};

class Expression : public ASTNode {
	protected:
		DataType expr_type;
	public:
		Expression( DataType dt ) : expr_type( dt ) {}
		Expression() : expr_type( INT_T ) {}
		
		DataType getType() { return expr_type; }
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "generic expression tipo ";
			switch( expr_type ){
				case INT_T: std::cout << "int\n"; break;
				case CHAR_T: std::cout << "char\n"; break;
				case INT_ARRAY_T: std::cout << "int[]\n"; break;
				case CHAR_ARRAY_T: std::cout << "char[]\n"; break;
			}
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 ); // nao deve entrar aqui..
		}
};

class Identifier : public ASTNode {
	protected:
		DataType var_type;
		std::string* var_name;
	public:
		Identifier(std::string* identifier, Expression* array_pos = NULL) : var_name(identifier) {
			child.resize(1);
			child[0] = array_pos;
		}
		
		DataType getVarType(){ return var_type; }
		std::string* getVarName() { return this->var_name; }
		
		void walk(int depth) {
			DataType type;
			if ((var_symbol_tab.find(*var_name) != var_symbol_tab.end()) && 
				!var_symbol_tab[*var_name].empty()) {
				type = var_symbol_tab[*var_name].top();
			}
			else {
				std::string error = "O identificador ``" + *var_name + "'' não foi declarado nesse escopo.";
				yyerror(error.c_str());
			}
			
			if ((type == INT_ARRAY_T || type == CHAR_ARRAY_T)) {
				if (child[0] == NULL) {
					std::string error = "Deve ser passada a posição do array ``" + *var_name
						+ "'' a ser acessada.";
					yyerror(error.c_str());
				}
				else {
					Expression* pos = static_cast<Expression*>(child[0]);
					
					if (pos->getType() != INT_T) {
						std::string error = "Erro ao acessar o array ``" + *var_name
							+ "'': a posição deve ser uma expressão do tipo int.";
						yyerror(error.c_str());	
					}
				}
			}
			
			this->var_type = type;
			
			// DEBUG
			IDENT( depth );
			
			std::cout << "variavel ";
			switch( var_type ){
				case INT_T: std::cout << "int "; break;
				case CHAR_T: std::cout << "char "; break;
				case INT_ARRAY_T: std::cout << "int[] "; break;
				case CHAR_ARRAY_T: std::cout << "char[] "; break;
			}
			std::cout << *(var_name);
			
			if (child[0] != NULL) {
				std::cout << " na posicao :" << std::endl;
				child[0]->walk( depth+1 );
			}
			std::cout << std::endl;
			// DEBUG
		}
};

class StatementList : public ASTNode {
	public:
		
	void walk( int depth ){
		IDENT( depth ); std::cout << "executando as instrucoes:" << std::endl;
		for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
	}
};

class VarDeclList : public ASTNode {	
	public:
		
	void walk( int depth ){
		IDENT( depth ); std::cout << "declaracoes de variaveis:" << std::endl;
		for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
	}
};

class Param : public ASTNode{	
	protected:
		DataType param_type;
		std::string* param_name;
	public:
		Param( DataType dt, std::string* param_ident ) : param_type( dt ), param_name( param_ident ){}
		
		const std::string* getParamName() { return param_name; }
		
		DataType getParamType() { return param_type; }
		
		void walk( int depth ){
			IDENT( depth ); 
			std::cout << "parametro " << *(param_name) << ", tipo ";
			switch( param_type ){
				case INT_T: std::cout << "int "; break;
				case CHAR_T: std::cout << "char "; break;
				case INT_ARRAY_T: std::cout << "int[] "; break;
				case CHAR_ARRAY_T: std::cout << "char[] "; break;
			}
			std::cout << std::endl;
		}
		
		DataType getType() { return this->param_type; }
};

class ParamList : public ASTNode {
	public:	
		void walk( int depth ){
			IDENT( depth );
			std::cout << "lista de parametros:" << std::endl;
		
			std::set<std::string> params;
		
			Param* param;
			for (size_t i = 0; i < child.size(); i++) {
				child[i]->walk(depth + 1);
			
				param = static_cast<Param*>(child[i]);
				if (params.find(*param->getParamName()) != params.end()) {
					std::string error = "Parâmetro " + *param->getParamName() + " redeclarado.";
					yyerror(error.c_str());
				}
				
				params.insert(*param->getParamName());
			}
		}
	
		const std::vector<ASTNode*>& getChild()
		{
			return this->child;
		}
};

class ArgList : public ASTNode {
	public:
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "lista de argumentos:" << std::endl;
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
		}
	
		std::vector<ASTNode*>& getChild() {
			return this->child;
		}
};

class Block : public Statement {
	public:
		Block( VarDeclList* var_decl, StatementList* statements ){
			child.resize(2);
			child[0] = var_decl;
			child[1] = statements;
		}
		Block( VarDeclList* var_decl ){
			child.resize(1);
			child[0] = var_decl;
		}
		Block( StatementList* statements ){
			child.resize(1);
			child[0] = statements;
		}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "iniciando num novo bloco de instrucoes" << std::endl;
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
			IDENT( depth ); std::cout << "finalizando o bloco.. aqui atualiza a tabela de simbolos" << std::endl;
		}
};

class FuncDecl : public ASTNode {
	protected:
		DataType func_type;
		std::string* func_name;
	public:
		FuncDecl( ParamList* params, Block* block ){
			child.resize(2);
			child[0] = params;
			child[1] = block;
		}
		
		void setFuncName( std::string* func_ident ) { 
			func_name = func_ident;
		}
		
		void setDataType( DataType dt ) { func_type = dt; }
		
		DataType getType() { return this->func_type; }
		ParamList* getParamList() { return static_cast<ParamList*>(this->child[0]); }
		
		void walk( int depth ){
			IDENT( depth );
			
			if (func_symbol_tab.find(*func_name) != func_symbol_tab.end()) {
				std::string error = "Redeclaração da função " + *func_name + ".";
				yyerror(error.c_str());
			}
			else {
				func_symbol_tab[*func_name] = this;
			}
			
			std::cout << "declarando funcao " << *(func_name) << ", tipo ";
			switch( func_type ){
				case INT_T: std::cout << "int "; break;
				case CHAR_T: std::cout << "char "; break;
				case INT_ARRAY_T: std::cout << "int[] "; break;
				case CHAR_ARRAY_T: std::cout << "char[] "; break;
			}
			std::cout << std::endl;
			
			if (child[0] != NULL) {
				child[0]->walk(depth+1);
			
			}
			child[1]->walk( depth+1 );
		}
};

class FuncCall : public Expression {
	protected:
		std::string* func_identifier;
	public:
		
		FuncCall( std::string* func_name, ArgList* args = NULL) 
		{ 	
			child.resize(1); 
			child[0] = args;
			
			this->func_identifier = func_name;
		}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "chamando funcao " << *(func_identifier) << std::endl;
			if (func_symbol_tab.find(*func_identifier) != func_symbol_tab.end()) {
				FuncDecl* func = func_symbol_tab[*func_identifier];
				ArgList* args = static_cast<ArgList*>(child[0]);
		
				if (func->getParamList() == NULL && args != NULL) {
					std::string error = "A função ``" + *func_identifier + 
						"'' não recebe parâmetros.";
					
					yyerror(error.c_str());
				}
				else if (func->getParamList() != NULL && args == NULL) {
					std::string error = "A função ``" + *func_identifier + 
						"'' recebe parâmetros.";
					
					yyerror(error.c_str());
				}
				else if ((func->getParamList() != NULL && args != NULL) && 
					(func->getParamList()->getChild().size() != args->getChild().size())) {
					std::string error = "A função ``" + *func_identifier + 
						"'' não recebe esse número de parâmetros.";
					
					yyerror(error.c_str());
				}
				else if (func->getParamList() != NULL && args != NULL) {
					const std::vector<ASTNode*>& paramChildren = func->getParamList()->getChild();
					const std::vector<ASTNode*>& argChildren = args->getChild();
					
					Expression* arg;
					Param* param;
					for (int i = 0; i < paramChildren.size(); ++i) {
						param = static_cast<Param*>(paramChildren[i]);
						arg   = static_cast<Expression*>(argChildren[i]);
						
						var_symbol_tab[*param->getParamName()].push(param->getParamType());
						
						if (param->getType() != arg->getType()) {
							std::string error = "Na chamada de função ``" + *func_identifier + 
								"'': tipos de parâmetros incompatíveis.";
								
							yyerror(error.c_str());
						}
					}
				}
				
				this->expr_type = func->getType();
			}
			else {
				std::string error = "A função ``" + *func_identifier + "'' não existe.";
				yyerror(error.c_str());
			}
			
			if (child[0] != NULL) child[0]->walk(depth + 1);
		}
};

class UnaryExpr : public Expression {
	protected:
		Op oper;
	public:
		UnaryExpr( Op op, UnaryExpr* expr ) : Expression( expr->getType() ), oper( op ) {
			child.resize(1);
			child[0] = expr;
		}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "expressao unaria ";
			switch( oper ){
				case MINUS: std::cout << "-" << std::endl; break;
				case NOT: std::cout << "!" << std::endl; break;
				default: std::cout << " isso nao devia ser impresso" << std::endl;
			}
			child[0]->walk( depth+1 );
		}
};

class BinaryExpr : public Expression {
	protected:
		Op oper;
	public:
		BinaryExpr( Op op, BinaryExpr* lhs, UnaryExpr* rhs ) : Expression( lhs->getType() ), oper( op ){
			child.resize(2);
			child[0] = lhs;
			child[1] = rhs;
		}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "expressao binaria ";
			switch( oper ){
				case PLUS: std::cout << "+" << std::endl; break;
				case MINUS: std::cout << "-" << std::endl; break;
	        	case TIMES: std::cout << "*" << std::endl; break;
				case DIVIDES: std::cout << "/" << std::endl; break;
				case GREATER: std::cout << ">" << std::endl; break;
				case LESS: std::cout << "<" << std::endl; break;
				case EQUALS: std::cout << "==" << std::endl; break;
				case NOT_EQUAL: std::cout << "!=" << std::endl; break;
				case LESS_EQUAL: std::cout << "<=" << std::endl; break;
				case GREATER_EQUAL: std::cout << ">=" << std::endl; break;
				case LOGICAL_OR: std::cout << "||" << std::endl; break;
				case LOGICAL_AND: std::cout << "&&" << std::endl; break;
				default: std::cout << "nao devia imprimir isso" << std::endl;
			}
			child[0]->walk( depth+1 );
			child[1]->walk( depth+1 );
		}
};

class Assignment : public Expression {
	public:
		Assignment(Identifier* lhs, Expression* rhs) : Expression(lhs->getVarType()) {
			child.resize(2);
			child[0] = lhs;
			child[1] = rhs;
		}
		
		void walk( int depth ){
			IDENT( depth );
			
			std::cout << "fazendo atribuicao " << std::endl;
			child[0]->walk(depth + 1);
			child[1]->walk(depth + 1);
			
			Identifier* lhs = static_cast<Identifier*>(child[0]);
			Expression* rhs = static_cast<Expression*>(child[1]);
			
			if (lhs->getVarType() != rhs->getType()) {
				std::string error = "Impossível converter " + getTypeName(rhs->getType()) +
					" para " + getTypeName(lhs->getVarType()) + " na atribuição.";
					
				yyerror(error.c_str());
			}
		}
};

class ConstExpr : public Expression {
	protected:
		std::string* value;
	public:
		ConstExpr( DataType dt, std::string* lvalue ) : Expression( dt ), value( lvalue ) {}
		
		int getIntValue() {
			std::istringstream iss(*value);
			int v;
			iss >> v;
			
			return v;
		}
		
		std::string getStringValue() {
			return *value;
		}
		
		char getCharValue() {
			return value->at(0);
		}
		
		void walk( int depth ){
			IDENT( depth ); std::cout << *value << std::endl;
		}
};

class DeclIdentifier : public ASTNode {
	protected:
		ConstExpr* var_size;
		std::string* var_name;
	public:
		DeclIdentifier(std::string* identifier) : var_name(identifier) { var_size = NULL; }
		DeclIdentifier(std::string* identifier, ConstExpr* array_size) : var_name(identifier), var_size(array_size) {}
		
		std::string* getVarName() { return this->var_name; }
		int getVarSize() { return this->var_size->getIntValue(); }
		bool isArray() { return (var_size != NULL); }
		
		void walk(int depth) {
			IDENT(depth);
			std::cout << *(var_name);
			if (var_size != NULL) {
				if (var_size->getType() != INT_T) {
					std::string error = "Na declaração da variável ``" + *var_name
						+ "'': O tipo do tamanho de um array deve ser um int.";
					
					yyerror(error.c_str());
				}
				std::cout << " com tamanho " << var_size->getIntValue();
			}
			std::cout << std::endl;
		}
};

class VarDecl : public ASTNode {
	protected:
		DataType var_type;
	public:
		void setDataType(DataType dt) { var_type = dt; }
		
		void walk(int depth) {
			IDENT( depth );
			std::cout << " declarando variaveis tipo ";
			switch( var_type ){
				case INT_T: std::cout << "int\n"; break;
				case CHAR_T: std::cout << "char\n"; break;
				case INT_ARRAY_T: std::cout << "int[]\n"; break;
				case CHAR_ARRAY_T: std::cout << "char[]\n"; break;
			}
			for (size_t i = 0; i < child.size(); i++) child[i]->walk(depth+1);
			
			std::set<std::string> declared;
			DeclIdentifier* id;
			
			for (size_t i = 0; i < child.size(); i++) {
				id = static_cast<DeclIdentifier*>(child[i]);
				
				if (declared.find(*(id->getVarName())) != declared.end()) {
					std::string error = "Redeclaração do identificador " + *id->getVarName() + ".";
					
					yyerror(error.c_str());
				}
				
				declared.insert(*(id->getVarName()));
				
				DataType type;
				if (id->isArray()) {
					if (this->var_type == INT_T) {
						type = INT_ARRAY_T;
					}
					else {
						type = CHAR_ARRAY_T;
					}
				}
				else {
					type = this->var_type;
				}
				
				var_symbol_tab[*(id->getVarName())].push(type);
			}
		}
};

class While : public Statement {
	public:
		While( Expression* expr, Statement* stmt ){
			child.resize(2);
			child[0] = expr; child[1] = stmt;
		}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "while " << std::endl;
			child[0]->walk( depth+1 );
			child[1]->walk( depth+1 );
		}
};

class If : public Statement {
	public:
		If( Expression* expr, Statement* stmt ){
			child.resize(2);
			child[0] = expr; child[1] = stmt;
		};
		If( Expression* expr, Statement* stmt, Statement* elsestmt ){
			child.resize(3);
			child[0] = expr; child[1] = stmt; child[2] = elsestmt;
		}
		
		void walk( int depth ){
			IDENT( depth );
			if( child.size() == 2 ) std::cout << "open If" << std::endl;
			else std::cout << "elsed If" << std::endl;
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
		}
};

class Return : public Statement {
	protected:
		Expression* return_value;
	public:
		Return( Expression* expr ) : return_value( expr ) {}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << " return " << std::endl;
			return_value->walk( depth+1 );
		}
};

class Read : public Statement {
	protected:
		Identifier* var_id;
	public:
		Read( Identifier* identifier ) : var_id( identifier ) {}
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "read " << std::endl;
			var_id->walk( depth+1 );
		}
};

class Write : public Statement {
	public:
		Write(){}
		Write( Expression* expr ){ child.resize( 1 ); child[0] = expr; }
		
		void walk( int depth ){
			IDENT( depth );
			std::cout << "write" << std::endl;
			child[0]->walk( depth+1 );
		}
};

class Break : public Statement {
	public:
		
		void walk( int depth ){
			IDENT( depth ); std::cout << "break" << std::endl;
		}	
};

#endif