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

typedef std::map<std::string, std::stack< std::pair<DataType,int> > > VarSymTab;
typedef std::map<std::string, FuncDecl*> FuncSymTab;

static VarSymTab var_symbol_tab;
static FuncSymTab func_symbol_tab;
static std::stack< std::string > declared;
static int scope_lvl = 0;

inline std::string getTypeName(DataType t)
{
	switch (t) {
		case INT_T: return "int";
		case CHAR_T: return "char";
		case INT_ARRAY_T: return "int[]";
		case CHAR_ARRAY_T: return "char[]";
	}
}

inline std::string getOperText(Op oper)
{
	switch (oper) {
		case PLUS: return "+";
		case MINUS: return "-";
		case TIMES: return "*";
		case DIVIDES: return "/";
		case GREATER: return ">";
		case LESS: return "<";
		case EQUALS: return "==";
		case NOT_EQUAL: return "!=";
		case LESS_EQUAL: return "<=";
		case GREATER_EQUAL: return ">=";
		case LOGICAL_OR: return "||";
		case LOGICAL_AND: return "&&";
	}
}

#define REP(i, N) for( int i = 0; i < N; i++ )
#define INDENT(depth) REP(i, depth) std::cout<< "\t";

class ASTNode {
	protected:
		std::vector< ASTNode* > child;
		int node_location;
	public:
		ASTNode() { child.clear(); }
		
		virtual void add(ASTNode* node) { child.push_back(node); }
		
		void set_location(int lineno) { node_location = lineno; }
		
		void reverse() { std::reverse(child.begin(), child.end()); }
		
		virtual void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT( depth )
				std::cout << " em um ASTNode qualquer.." << std::endl;
			#endif
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
		}
};

class Expression : public ASTNode {
	protected:
		DataType expr_type;
	public:
		Expression(DataType dt) : expr_type(dt) {}
		Expression() : expr_type(INT_T) {}
		
		DataType getType() { return expr_type; }
		
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT( depth )
				std::cout << "generic expression tipo " << getTypeName(expr_type) << std::endl; // acho que nao deve imprimir isso
			#endif
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 ); // nem entrar aqui..
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
		
		DataType getVarType() { return var_type; }
		std::string* getVarName() { return this->var_name; }
		
		void walk(int depth) {
			DataType type;
			
			if ((var_symbol_tab.find(*var_name) != var_symbol_tab.end()) && 
				!var_symbol_tab[*var_name].empty()) {
				type = var_symbol_tab[*var_name].top().first;
			}
			else {
				std::string error = "O identificador ``" + *var_name + "'' não foi declarado nesse escopo.";
				yyerror(error.c_str(), node_location );
			}
			
			#ifdef DBG_SYM_TAB
				std::cout << *var_name << " @ symbol_tab as " << getTypeName( type ) << std::endl;
			#endif 
			
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "variavel " << getTypeName( type ) << " " << *(var_name) << std::endl;
			#endif
			
			if ((type == INT_ARRAY_T || type == CHAR_ARRAY_T)) {
				if (child[0] == NULL ) {
					std::string error = "Deve ser passada a posição do array ``" + *var_name
						+ "'' a ser acessada.";
					yyerror(error.c_str(), node_location );
				}
				else {
					Expression* pos = static_cast<Expression*>(child[0]);
					
					#ifdef DBG_PRINT_TREE
						INDENT(depth)
						std::cout << " na posicao : " << std::endl;
					#endif
					
					pos->walk( depth+1 );
					
					#ifdef DBG_PRINT_TREE
						std::cout << std::endl;
					#endif
					
					if (pos->getType() != INT_T) {
						std::string error = "Erro ao acessar o array ``" + *var_name
							+ "'': a posição deve ser uma expressão do tipo int.";
						yyerror(error.c_str(), node_location );	
					}
				}
				// Se o identificador esta sendo acessado corretamente o tipo do identificador nao eh mais array
				if( type == INT_ARRAY_T ) type = INT_T;
				if( type == CHAR_ARRAY_T ) type = CHAR_T;
			} 
			
			this->var_type = type;			
		}
};

class StatementList : public ASTNode {
	public:
	
	const std::vector<ASTNode*>& getChildren()
	{
		return this->child;
	}
	
	void walk(int depth) {
		#ifdef DBG_PRINT_TREE
			INDENT(depth) std::cout << "executando as instrucoes:" << std::endl;
		#endif
		for (size_t i = 0; i < child.size(); i++) child[i]->walk(depth + 1);
	}
};

class VarDeclList : public ASTNode {	
	public:
<<<<<<< HEAD
		
=======
	
>>>>>>> d5fef380f45de38b676bbeaf2301a65afb78db8c
	void walk(int depth) {
		#ifdef DBG_PRINT_TREE
			INDENT(depth) std::cout << "declaracoes de variaveis:" << std::endl;
		#endif
		for (size_t i = 0; i < child.size(); i++) child[i]->walk(depth + 1);
	}
};

class Statement : public ASTNode {
	public:
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT(depth) std::cout << "fake statement" << std::endl;
			#endif
		}
};

class HasBlock
{
	protected:
		bool has_return;
		DataType return_type;
		bool return_error;
		bool has_break;
	
	public:	
		HasBlock() : has_return(false), return_error(false), has_break(false) {}
		
		bool hasReturn() { return has_return; }
		DataType getReturnType() { return return_type; }
		bool hasError() { return return_error; }
		bool hasBreak() { return has_break; }	
};

class Break : public Statement {
	public:		
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT(depth) std::cout << "break" << std::endl;
			#endif
		}	
};

class Return : public Statement {
	protected:
		Expression* return_value;
		DataType return_type;
	public:
		Return(Expression* expr) : return_value(expr) {}
		
		DataType getReturnType() { return this->return_type; }
		
		void walk(int depth) {	
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << " return " << std::endl;
			#endif
			
			return_value->walk(depth + 1);
			this->return_type = return_value->getType();
		}
};

class Block : public Statement, public HasBlock {
	
	public:
		Block(VarDeclList* var_decl, StatementList* statements = NULL) {
			child.resize(2);
			child[0] = var_decl;
			child[1] = statements;
		}
		
		Block(StatementList* statements) {
			child.resize(2);
			child[0] = NULL;
			child[1] = statements;
		}
		
		void walk(int depth) {
			#ifdef  DBG_PRINT_TREE
				INDENT(depth) std::cout << "iniciando bloco.." << std::endl;
			#endif
			
			scope_lvl++; declared.push( "$" );
			
			this->has_return = false;
				
			if (child[0] != NULL) child[0]->walk(depth + 1);
			
			if (child[1] != NULL) {
				child[1]->walk(depth + 1);
				
				StatementList* stmt = static_cast<StatementList*>(child[1]);
				const std::vector<ASTNode*> children = stmt->getChildren();
				
				Return* ret;
				HasBlock* block;
				for (size_t i = 0; i < children.size(); ++i) {
					ret = dynamic_cast<Return*>(children[i]);
					
					if (ret != NULL) {
						if (!has_return) {
							this->has_return = true;
							this->return_type = ret->getReturnType();
						}
						else if (return_type != ret->getReturnType()) {
							this->return_error = true;
						}
					}
					else {
						block = dynamic_cast<HasBlock*>(children[i]);
						if (block != NULL) {
							if (block->hasReturn()) {
								if (this->has_return) {
									if (this->return_type != block->getReturnType()) {
										this->return_error = true;
									}
								}
								else {
									this->return_error |= block->hasError();
									this->return_type = block->getReturnType();
								}
							
								this->has_return = true;
							}
							
							if (dynamic_cast<Block*>(children[i]) != NULL && block->hasBreak()) {
								this->has_break = true;
							}
						}
						else if (dynamic_cast<Break*>(children[i]) != NULL) {
							this->has_break = true;
						}
					}
				}
			}
			
			#ifdef DBG_PRINT_TREE
				INDENT(depth) std::cout << "finalizando o bloco.. aqui atualiza a tabela de simbolos" << std::endl;
			#endif
			scope_lvl--;
			while( declared.top() != "$" ){
				std::string var_ident = declared.top(); declared.pop();
				#ifdef DBG_SYM_TAB
					std::cout << "removendo " << getTypeName( var_symbol_tab[var_ident].top().first ) << " " << var_ident << std::endl;
				#endif
				var_symbol_tab[ var_ident ].pop();
				if( var_symbol_tab[ var_ident ].empty() ) var_symbol_tab.erase( var_ident );
			}
			declared.pop();
		}
};

class If : public Statement, public HasBlock {
	public:
		If(Expression* expr, Statement* stmt, Statement* elsestmt = NULL) {
			child.resize(3);
			child[0] = expr; 
			child[1] = stmt;
			child[2] = elsestmt;
		}
		
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				if( child[2] == NULL ) std::cout << "open If" << std::endl;
				else std::cout << "elsed If" << std::endl;
			#endif
			
			child[0]->walk(depth + 1);
			child[1]->walk(depth + 1);
<<<<<<< HEAD
			
			Block* block = dynamic_cast<Block*>(child[1]);
=======
			//TODO -- nao precisa necessariamente ser um Block
			Block* block = static_cast<Block*>(child[1]);
>>>>>>> d5fef380f45de38b676bbeaf2301a65afb78db8c
			
			if (block != NULL) {
				if (block->hasBreak()) {
					yyerror("O comando ``break'' só pode ser utilizado dentro de um laço de repetição.", 
						node_location);
				}
			
				this->has_return   = block->hasReturn();
				this->return_error = block->hasError();
				this->return_type  = block->getReturnType();
			}
			else if (dynamic_cast<Return*>(child[1]) != NULL) {
				Return* ret = static_cast<Return*>(child[1]);
				
				this->has_return = true;
				this->return_type = ret->getReturnType();
			}
			else if (dynamic_cast<Break*>(child[1]) != NULL) {
				yyerror("O comando ``break'' só pode ser utilizado dentro de um laço de repetição.", 
					node_location);
			}
			
			if (child[2] != NULL) {
<<<<<<< HEAD
				child[2]->walk(depth + 1);
=======
				//TODO -- nao precisa necessariamente ser um Block
				block = static_cast<Block*>(child[2]);
				block->walk(depth+1);
>>>>>>> d5fef380f45de38b676bbeaf2301a65afb78db8c
				
				block = dynamic_cast<Block*>(child[2]);
			
				if (block != NULL) {
					
					std::cout << "BLOX\n";
					
					if (block->hasBreak()) {
						yyerror("O comando ``break'' só pode ser utilizado dentro de um laço de repetição.", 
							node_location);
					}
			
					if (this->has_return && block->getReturnType() != this->return_type) {
						this->return_error = true;
					}
			
					this->has_return   |= block->hasReturn();
					this->return_error |= block->hasError();
					this->return_type = block->getReturnType();
				}
				else if (dynamic_cast<Return*>(child[2]) != NULL) {
					Return* ret = static_cast<Return*>(child[2]);
					
					if (this->has_return && ret->getReturnType() != this->return_type) {
						this->return_error = true;
					} 

					this->has_return  = true;
					this->return_type = ret->getReturnType();
				}
				else if (dynamic_cast<Break*>(child[2]) != NULL) {
					yyerror("O comando ``break'' só pode ser utilizado dentro de um laço de repetição.", 
						node_location);
				}
			}	
		}
};

class While : public Statement, public HasBlock {
	public:
		While(Expression* expr, Statement* stmt) {
			child.resize(2);
			child[0] = expr; child[1] = stmt;
		}
		
		void walk( int depth ){	
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "while " << std::endl;
			#endif
			
			child[0]->walk(depth + 1);
			child[1]->walk(depth + 1);
			// TODO -- Stmt não precisa ser necessariamente um Block..
			Block* block = static_cast<Block*>(child[1]);
			this->has_return   = block->hasReturn();
			this->return_error = block->hasError();
			this->return_type  = block->getReturnType();
		}
};

class Read : public Statement {
	protected:
		Identifier* var_id;
	public:
		Read( Identifier* identifier ) : var_id( identifier ) {}
		
		void walk( int depth ){
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "read " << std::endl;
			#endif
			var_id->walk( depth+1 );
		}
};

class Write : public Statement {
	public:
		Write(){}
		Write( Expression* expr ){ child.resize( 1 ); child[0] = expr; }
		
		void walk( int depth ){
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "write" << std::endl;
			#endif
			child[0]->walk( depth+1 );
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
			#ifdef DBG_PRINT_TREE
				INDENT(depth) 
				std::cout << "parametro " << *(param_name) << ", tipo " << getTypeName(param_type) << std::endl;
			#endif
		}
		
		DataType getType() { return this->param_type; }
};

class ParamList : public ASTNode {
	public:	
		void walk( int depth ){
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "lista de parametros:" << std::endl;
			#endif
			
			Param* param;
			for (size_t i = 0; i < child.size(); i++) {
				child[i]->walk(depth + 1);
			
				param = static_cast<Param*>(child[i]);
				if (var_symbol_tab.find(*param->getParamName()) != var_symbol_tab.end()) {
					std::string error = "Parâmetro " + *param->getParamName() + " redeclarado.";
					yyerror(error.c_str(), node_location );
				}
				
				declared.push( *param->getParamName() );
				#ifdef DBG_SYM_TAB
					std::cout << "adicionando " << *param->getParamName() << " " << getTypeName( param->getParamType() ) << std::endl;
				#endif
				var_symbol_tab[ *param->getParamName() ].push( std::make_pair( param->getParamType(), scope_lvl ) );
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
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "lista de argumentos:" << std::endl;
			#endif
			for( size_t i = 0; i < child.size(); i++ ) child[i]->walk( depth+1 );
		}
	
		std::vector<ASTNode*>& getChild() {
			return this->child;
		}
};

class FuncDecl : public ASTNode {
	protected:
		DataType func_type;
		std::string* func_name;
	public:
		FuncDecl(ParamList* params, Block* block) {
			child.resize(2);
			child[0] = params;
			child[1] = block;
		}
		
		void setFuncName(std::string* func_ident) { 
			func_name = func_ident;
		}
		
		void setDataType(DataType dt) { func_type = dt; }
		
		DataType getType() { return this->func_type; }
		ParamList* getParamList() { return static_cast<ParamList*>(this->child[0]); }
		
		void walk(int depth){
			
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "declarando funcao " << *(func_name) << ", tipo " << getTypeName(func_type) << std::endl;
			#endif
			
			if (var_symbol_tab.find(*func_name) != var_symbol_tab.end()) {
				std::string error = "A função " + *func_name + " nao pode ter o mesmo nome que uma variavel.";
				yyerror( error.c_str(), node_location  );
			}
			
			if (func_symbol_tab.find(*func_name) != func_symbol_tab.end()) {
				std::string error = "Redeclaração da função " + *func_name + ".";
				yyerror(error.c_str(), node_location );
			}
			else {
				func_symbol_tab[*func_name] = this;
			}
			
			scope_lvl++; declared.push( "$" );
			if (child[0] != NULL) {
				child[0]->walk(depth + 1);
			}
			scope_lvl--;
			
			Block* block = static_cast<Block*>(child[1]);
			block->walk(depth + 1);
			
			if (block->hasReturn()) {
				if (block->hasError()) {
					std::string error = "Na função ``" + *func_name + "'': "
						+ " retornando um tipo diferente do tipo de retorno da função.";
						
					yyerror(error.c_str(), node_location);
				}
				else if (block->getReturnType() != this->func_type) {
					std::string error = "Na função ``" + *func_name + "'': "
						+ " retornando um tipo diferente do tipo de retorno da função.";
					yyerror(error.c_str(), node_location);
				}
			}
			else {
				std::string error = "Na função ``" + *func_name + "'': "
					+ "a função deve retornar o tipo " + getTypeName(this->func_type) + ", retornando nada.";
					
				yyerror(error.c_str(), node_location);
			}
			
			if (block->hasBreak()) {
				yyerror("O comando ``break'' só pode ser utilizado dentro de um laço de repetição.", 
					node_location);
			}
			
			while( declared.top() != "$" ){
				std::string var_ident = declared.top(); declared.pop();
				#ifdef DBG_SYM_TAB
					std::cout << "removendo " << getTypeName( var_symbol_tab[var_ident].top().first ) << std::endl;
				#endif
				var_symbol_tab[ var_ident ].pop();
				if( var_symbol_tab[ var_ident ].empty() ) var_symbol_tab.erase( var_ident );
			}
			declared.pop();
			
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
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "chamando funcao " << *(func_identifier) << std::endl;
			#endif
			
			if (func_symbol_tab.find(*func_identifier) != func_symbol_tab.end()) {
				FuncDecl* func = func_symbol_tab[*func_identifier];
				ArgList* args = static_cast<ArgList*>(child[0]);
		
				if (func->getParamList() == NULL && args != NULL) {
					std::string error = "A função ``" + *func_identifier + 
						"'' não recebe parâmetros.";
					
					yyerror(error.c_str(), node_location );
				}
				else if (func->getParamList() != NULL && args == NULL) {
					std::string error = "A função ``" + *func_identifier + 
						"'' recebe parâmetros.";
					
					yyerror(error.c_str(), node_location );
				}
				else if ((func->getParamList() != NULL && args != NULL) && 
					(func->getParamList()->getChild().size() != args->getChild().size())) {
					std::string error = "A função ``" + *func_identifier + 
						"'' não recebe esse número de parâmetros.";
					
					yyerror(error.c_str(), node_location );
				}
				else if (func->getParamList() != NULL && args != NULL) {
					
					child[0]->walk(depth + 1);
					
					const std::vector<ASTNode*>& paramChildren = func->getParamList()->getChild();
					const std::vector<ASTNode*>& argChildren = args->getChild();
					
					Expression* arg;
					Param* param;
					for (int i = 0; i < paramChildren.size(); ++i) {
						param = static_cast<Param*>(paramChildren[i]);
						arg   = static_cast<Expression*>(argChildren[i]);
						
						if (param->getParamType() != arg->getType()) {
							std::string error = "Na chamada de função ``" + *func_identifier + 
								"'': tipos de parâmetros incompatíveis.";
								
							yyerror(error.c_str(), node_location );
						}
					}
				}
				
				this->expr_type = func->getType();
			}
			else {
				std::string error = "A função ``" + *func_identifier + "'' não existe.";
				yyerror(error.c_str(), node_location );
			}
			
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
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "expressao unaria ";
				switch( oper ){
					case MINUS: std::cout << "-" << std::endl; break;
					case NOT: std::cout << "!" << std::endl; break;
					default: std::cout << " isso nao devia ser impresso" << std::endl;
				}
			#endif	
			
			child[0]->walk( depth+1 );
			this->expr_type = ((Expression*)child[0])->getType();
			if( oper == MINUS && this->expr_type == CHAR_T ) yyerror( "Operador unário '-' não definido sobre variáveis do tipo char.", node_location );
		}
};

class BinaryExpr : public Expression {
	protected:
		Op oper;
	public:
		BinaryExpr(Op op, UnaryExpr* lhs, BinaryExpr* rhs) : Expression( lhs->getType() ), oper( op ) {
			child.resize(2);
			child[0] = lhs;
			child[1] = rhs;
		}
		
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "expressao binaria " << getOperText( oper ) << std::endl;
			#endif
			
			child[0]->walk( depth+1 );
			child[1]->walk( depth+1 );
			
			if( oper != LOGICAL_OR && oper != LOGICAL_AND && ((UnaryExpr*)child[0])->getType() != ((BinaryExpr*)child[1])->getType() ){
				std::string error = "Em " + getOperText( oper ) + " tipos incompativeis. " + 
									getTypeName( ((BinaryExpr*)child[0])->getType() ) + " e " + getTypeName( ((UnaryExpr*)child[1])->getType() );
				yyerror( error.c_str(), node_location  );
			}
			else this->expr_type = ((UnaryExpr*)child[0])->getType();
			
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << getTypeName( this->getType() ) << " " << getOperText( oper ) << std::endl;
			#endif
		}
};

class Assignment : public Expression {
	public:
		Assignment(Identifier* lhs, Expression* rhs) : Expression(lhs->getVarType()) {
			child.resize(2);
			child[0] = lhs;
			child[1] = rhs;
		}
	
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << "fazendo atribuicao " << std::endl;
			#endif 
			
			child[0]->walk(depth + 1);
			child[1]->walk(depth + 1);
			
			Identifier* lhs = static_cast<Identifier*>(child[0]);
			Expression* rhs = static_cast<Expression*>(child[1]);
			
			if ( lhs->getVarType() != rhs->getType() ) {
				std::string error = "Impossível converter " + getTypeName(rhs->getType()) +
					" para " + getTypeName(lhs->getVarType()) + " na atribuição.";
					
				yyerror(error.c_str(), node_location );
			}
			else this->expr_type = lhs->getVarType();
		}
};

class ConstExpr : public Expression {
	protected:
		std::string* value;
	public:
		ConstExpr(DataType dt, std::string* lvalue) : Expression( dt ), value( lvalue ) {}
		
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
			if( value->at(0) == '\\' ){
				switch( value->at(1) ){
					case '0': return '\0';
					case 'n': return '\n';
					case 't': return '\t';
					case 'a': return '\a';
					case 'r': return '\r';
					case 'b': return '\b'; 
					case 'f': return '\f';
					case '\\': return '\\';
				}
			}
			return value->at(0);
		}
		
		void walk( int depth ){
			#ifdef DBG_PRINT_TREE
				INDENT(depth) std::cout << *value << std::endl;
			#endif
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
		
		void walk(int depth, DataType var_type ){
			
			if( func_symbol_tab.find( *var_name ) != func_symbol_tab.end() ){
				std::string error = "Redeclaração de " + *var_name + ".";
				yyerror( error.c_str(), node_location  );
			}
			if( var_symbol_tab.find( *var_name ) != var_symbol_tab.end() && var_symbol_tab[*var_name].top().second == scope_lvl ){
				std::string error = "Redeclaracao da variavel " + *var_name + "."; 
				yyerror( error.c_str(), node_location  );
			}
			
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << *(var_name) << " ";
			#endif
			
			if( var_size != NULL ){
				if (var_size->getType() != INT_T) {
					std::string error = "Na declaração da variável ``" + *var_name
						+ "'': O tipo do tamanho de um array deve ser um int.";
					
					yyerror(error.c_str(), node_location );
				}
				if( var_type == INT_T ) var_type = INT_ARRAY_T;
				if( var_type == CHAR_T ) var_type = CHAR_ARRAY_T;
				
				#ifdef DBG_PRINT_TREE
					std::cout << " com tamanho " << var_size->getIntValue();
				#endif
			}
			
			#ifdef DBG_SYM_TAB
				std::cout << "adicionando " << *var_name << " " << getTypeName( var_type ) << std::endl;
			#endif

			var_symbol_tab[*var_name].push( std::make_pair( var_type, scope_lvl ) );
			declared.push( *var_name );
			
			#ifdef DBG_PRINT_TREE
				std::cout << std::endl;
			#endif
		}
};

class VarDecl : public ASTNode {
	protected:
		DataType var_type;
	public:
		void setDataType(DataType dt) { var_type = dt; }
		
		void walk(int depth) {
			#ifdef DBG_PRINT_TREE
				INDENT(depth)
				std::cout << " declarando variaveis tipo " << getTypeName(var_type) << std::endl;
			#endif
			
			for (size_t i = 0; i < child.size(); i++) ((DeclIdentifier*)child[i])->walk(depth+1, var_type);	
		}
};

#endif