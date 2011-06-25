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
#include "parser.hpp"

void yyerror(const char *);
char invalidchar[30] = "CARACTER INVALIDO: ";
std::string buf;

#define TOKEN(tk) tk

%}
%x STRING COMMENT RCHAR ESCP ENDCHAR

DELIM	[ \t\r]
WS		{DELIM}+
DIGIT	[0-9]
LETTER	[A-Za-z_]

%%

{WS}			{;}
\n				{yylineno++;}
"main"			{ yylval.lxval = new std::string(yytext); return MAIN; }
"int"			{ return INT; }
"char"			{ return CHAR; }
"return"		{ return RETURN; }
"read"			{ return READ; }
"write"			{ return WRITE; }
"writeln"		{ return WRITELN; }
"break"			{ return BREAK; }
"if"			{ return IF; }
"else"			{ return ELSE; }
"while"			{ return WHILE; }

[-!(){}<>+*/=;,] { return *yytext; }

"["				{ return *yytext; }
"]"				{ return *yytext; }

"=="			{ return EQUAL; }
"!="			{ return NEQUAL; }
"<="			{ return LESSEQ; }
">="			{ return GREATEREQ; }
"||"			{ return OR; }
"&&"			{ return AND; }

{LETTER}({LETTER}|{DIGIT})*		{ yylval.lxval = new std::string(yytext); return ID; }
{DIGIT}+						{ yylval.lxval = new std::string(yytext); return LITINT; }

'				{ BEGIN RCHAR; }
<RCHAR>\\		{ BEGIN ESCP; }
<RCHAR>.		{ buf = ""; buf += *yytext; BEGIN ENDCHAR; }
<ESCP>{
	[0ntarbf\\']	{ buf = "\\"; buf += *yytext; BEGIN ENDCHAR; }
}
<ESCP>.			{ yyerror( "FALTANDO DELIMITADOR ' EM CONSTANTE TIPO char." ); }

<ENDCHAR>'		{ BEGIN INITIAL; yylval.lxval = new std::string( buf ); return LITCHAR; }
<ENDCHAR>.		{ yyerror( "FALTANDO DELIMITADOR ' EM CONSTANTE TIPO char." ); };


\" 				{ BEGIN STRING; buf = ""; }
<STRING>\"		{ BEGIN INITIAL; yylval.lxval = new std::string( buf ); return LITSTRING; }
<STRING>\n		{ yylineno++; yyerror("CADEIA DE CARACTERES OCUPA MAIS DE UMA LINHA"); }
<STRING>{
	"\\n"		{ buf += '\n'; }
	"\\t"		{ buf += '\t'; }
	"\\a"		{ buf += '\a'; }
	"\\r"		{ buf += '\r'; }
	"\\b"		{ buf += '\b'; }
	"\\f"		{ buf += '\f'; }
	"\\\""		{ buf += '\\'; }
}
<STRING><<EOF>> { yyerror("STRING NAO TERMINA."); }
<STRING>.		{ buf += *yytext; }

"/*"			 { BEGIN COMMENT; }
<COMMENT>"*"+"/" { BEGIN INITIAL; }

<COMMENT>"*"+[^*/\n] {;}
<COMMENT>\n		 { yylineno++; }
<COMMENT>[^*\n]	 {;}
<COMMENT><<EOF>> { yyerror("COMENTARIO NAO TERMINA."); }

.				{ yyerror( strcat( invalidchar, yytext ) ); }

%%
int yywrap(){ return 1; }

