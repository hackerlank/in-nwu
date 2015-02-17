/*
	$ ExprCalc   (C) 2014 MF
	$ lexer.c   #1403
*/


#include "header/base.h"
#include "header/bridge.h"
#include "header/lexer.h"



/* ��������ӳ������ */
typedef struct {
	const char *	name;				/* ������ */
	ec_value_t		value;				/* ����ֵ */
} c_constant_t;


/* ��������ӳ������ */
typedef struct {
	const char *	name;				/* ������ */
	int				token;				/* token ֵ */
} c_function_t;



/* ��������ӳ��� */
static c_constant_t table_constant [ ] = {
	{	"e",		M_E		},
	{	"pi",		M_PI	},
};


/* ��������ӳ��� */
static c_function_t table_function [ ] = {
	{	"sin",		T_FUNCTION_SIN		},
	{	"cos",		T_FUNCTION_COS		},
	{	"tan",		T_FUNCTION_TAN		},
	{	"arcsin",	T_FUNCTION_ARCSIN	},
	{	"arccos",	T_FUNCTION_ARCCOS	},
	{	"arctan",	T_FUNCTION_ARCTAN	},
	{	"sinh",		T_FUNCTION_SINH		},
	{	"cosh",		T_FUNCTION_COSH		},
	{	"tanh",		T_FUNCTION_TANH		},
	{	"log",		T_FUNCTION_LOG		},
	{	"log10",	T_FUNCTION_LOG10	},
	{	"ln",		T_FUNCTION_LN		},
	{	"pow",		T_FUNCTION_POW		},
	{	"exp",		T_FUNCTION_EXP		},
	{	"fact",		T_FUNCTION_FACT		},
	{	"mod",		T_FUNCTION_MOD		},
	{	"sqrt",		T_FUNCTION_SQRT		},
	{	"cuberoot",	T_FUNCTION_CUBEROOT	},
	{	"yroot",	T_FUNCTION_YROOT	},
	{	"avg",		T_FUNCTION_AVG		},
	{	"sum",		T_FUNCTION_SUM		},
	{	"var",		T_FUNCTION_VAR		},
	{	"varp",		T_FUNCTION_VARP		},
	{	"stdev",	T_FUNCTION_STDEV	},
	{	"stdevp",	T_FUNCTION_STDEVP	},
	{	"max",		T_FUNCTION_MAX		},
	{	"min",		T_FUNCTION_MIN		},
	{	"floor",	T_FUNCTION_FLOOR	},
	{	"ceil",		T_FUNCTION_CEIL		},
	{	"round",	T_FUNCTION_ROUND	},
	{	"rand",		T_FUNCTION_RAND		},
	{	"a2r",		T_FUNCTION_A2R		},
	{	"r2a",		T_FUNCTION_R2A		},
};




/* �ʷ������������ؽ�� */
int		ec_lexer_return	= 0;			/* @ lexer.h */




/* ��ȡ��� */
DECL (eof) {
	return T_END;
}



/* ʶ������ */
DECL (integer, int base) {
	unsigned long	number	= 0;

	switch(base) {
		case  0:	number	= 0;								break;
		case  2:	number	= strtoul (text + 2, NULL,  2);		break;
		case  8:	number	= strtoul (text + 1, NULL,  8);		break;
		case 10:	number	= strtoul (text    , NULL, 10);		break;
		case 16:	number	= strtoul (text + 2, NULL, 16);		break;

		default:	return ec_ex_syntax ("undefined number base");
	}

	out->is_set		= 0;
	out->v.number	= (ec_value_t) number;

	return T_NUMBER;
}


/* ʶ�𸡵��� */
DECL (float) {
	out->is_set		= 0;
	out->v.number	= (ec_value_t) atof (text);

	return T_NUMBER;
}



/* ʶ��ֽ�� */
DECL (delimiter) {
	return (int) * text;
}


/* ʶ������� */
DECL (operator) {
	return (int) * text;
}


/* ʶ���ʶ�� */
DECL (identifier) {
	int	i;

	int	count_1	= sizeof (table_constant) / sizeof (table_constant [0]);
	int	count_2	= sizeof (table_function) / sizeof (table_function [0]);

	for(i = 0; i < count_1; i ++) {
		if(_strnicmp (table_constant [i].name, text, length) == 0) {
			out->is_set		= 0;
			out->v.number	= table_constant [i].value;

			return T_NUMBER;
		}
	}

	for(i = 0; i < count_2; i ++) {
		if(_strnicmp (table_function [i].name, text, length) == 0) {
			return table_function [i].token;
		}
	}

	return ec_ex_syntax ("undefined symbol");
}



/* δ�����ַ� */
DECL (undefined) {
	return ec_ex_syntax ("undefined character");
}


