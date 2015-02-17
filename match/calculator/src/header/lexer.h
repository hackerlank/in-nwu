/*
	$ ExprCalc   (C) 2014 MF
	$ header/lexer.h   #1403
*/


#ifndef _EC_LEXER_H_
#define _EC_LEXER_H_

#ifdef __cplusplus
	extern "C" {
#endif



/* �ʷ������������� */
#define F(name)				ec_lexer_##name


/* �ʷ������������� */
#define DECL(name, ...)		int F (name) (YYSTYPE *out, const char *text, int length, __VA_ARGS__)

/* ���ôʷ��������� */
#define CALL(name, ...)		ec_lexer_return	= F (name) (yylval, yytext, yyleng, __VA_ARGS__);


/* ͳ�ƴ��� */
#define COUNT()				read_offset		= read_offset + (unsigned int) yyleng;

/* ����ƥ���� */
#define RETURN()			return ec_lexer_return;




/* �ʷ��������� */
DECL (eof);
DECL (integer,	int base);			DECL (float);
DECL (delimiter);					DECL (operator);				DECL (identifier);
DECL (undefined);




/* �ʷ������������ؽ�� */
int		ec_lexer_return;					/* @ lexer.c */




#ifdef __cplusplus
	}
#endif

#endif   /* LEXER */