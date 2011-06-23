/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     MAIN = 258,
     INT = 259,
     CHAR = 260,
     RETURN = 261,
     READ = 262,
     WRITE = 263,
     WRITELN = 264,
     BREAK = 265,
     IF = 266,
     ELSE = 267,
     WHILE = 268,
     EQUAL = 269,
     NEQUAL = 270,
     LESSEQ = 271,
     GREATEREQ = 272,
     OR = 273,
     AND = 274,
     ID = 275,
     LITINT = 276,
     LITCHAR = 277,
     LITSTRING = 278
   };
#endif
/* Tokens.  */
#define MAIN 258
#define INT 259
#define CHAR 260
#define RETURN 261
#define READ 262
#define WRITE 263
#define WRITELN 264
#define BREAK 265
#define IF 266
#define ELSE 267
#define WHILE 268
#define EQUAL 269
#define NEQUAL 270
#define LESSEQ 271
#define GREATEREQ 272
#define OR 273
#define AND 274
#define ID 275
#define LITINT 276
#define LITCHAR 277
#define LITSTRING 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 32 "cezinho-parser.y"

	ASTNode *node;
	DataType datatype;
	Op oper;
	std::string *lxval;



/* Line 1685 of yacc.c  */
#line 106 "parser.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


