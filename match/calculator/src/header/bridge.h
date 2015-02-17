/*
	$ ExprCalc   (C) 2014 MF
	$ header/bridge.h   #1403
*/


#ifndef _EC_BRIDGE_H_
#define _EC_BRIDGE_H_

#ifdef __cplusplus
	extern "C" {
#endif



/* �ڲ��������� */
#define YYSTYPE			ec_operand_t



/* �ʷ������������� */
struct yy_buffer_state;




/* �ʷ������� */
int		yylex			(YYSTYPE *);
int		yylex_destroy	(void);

struct yy_buffer_state *	yy_scan_string	(const char *);



/* �﷨������ */
int		yyparse	(void);




/* �� bison ���� token ��ŵ�ͷ�ļ� */
#include "../syntax/parser.yy.h"




#ifdef __cplusplus
	}
#endif

#endif   /* BRIDGE */