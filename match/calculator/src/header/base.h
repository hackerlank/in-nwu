/*
	$ ExprCalc   (C) 2014 MF
	$ header/base.h   #1403
*/


#ifndef _MSC_VER
	#error This project DOES NOT support your compiler
#endif


#ifndef _EC_BASE_H_
#define _EC_BASE_H_


#pragma warning (disable: 4003)
#pragma warning (disable: 4996)


#define _USE_MATH_DEFINES


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <fpieee.h>
#include <Windows.h>


#ifdef __cplusplus
	extern "C" {
#endif



/* ��ֵ���� */
typedef		double		ec_value_t;


/* �������� */
typedef struct {
	ec_value_t *		pointer;				/* ��������ָ�� */

	unsigned int		count;					/* ����Ԫ�ظ��� */
	unsigned int		unused;					/* ����ʣ����� */
} ec_set_t;


/* ���������� */
typedef struct {
	int		is_set;								/* ���ϱ�� */

	union {
		ec_value_t	number;						/* ��ֵ */
		ec_set_t	set;						/* ���� */
	} v;
} ec_operand_t;




/* �ڴ���� */
void *	ec_mm_new		(unsigned int size);
void *	ec_mm_renew		(void *p, unsigned int size);
void *	ec_mm_delete	(void *p);

/* ���йܵ��ڴ���� */
void	ec_mg_open		(void);
void	ec_mg_close		(void);
void *	ec_mg_new		(unsigned int size);
void *	ec_mg_renew		(void *p, unsigned int size);

/* ������� */
void	ec_ex_runtime	(const char *message);
int		ec_ex_syntax	(const char *message);

/* ���������� */
void	calc_open		(const char *input, char *output, char *error);
int		calc_execute	(void);
void	calc_close		(void);




/* ������ */
extern	const char *	buffer_input;			/* @ base.c */
extern	char *			buffer_output;			/* @ base.c */
extern	char *			buffer_error;			/* @ base.c */

/* ����ո�� */
extern	uint8_t *		read_space;				/* @ base.c */
/* ����ƫ���� */
extern	unsigned int	read_offset;			/* @ base.c */




/* �й��ڴ��������� */
#define EC_MG_BLOCK				16

/* ����������� */
#define EC_SET_BLOCK			32




#ifdef __cplusplus
	}
#endif

#endif   /* BASE */