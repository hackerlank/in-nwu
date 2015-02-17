/*
	$ ExprCalc   (C) 2014 MF
	$ base.c   #1403
*/


#include "header/base.h"
#include "header/bridge.h"



/* ���뻺�� */
const char *	buffer_input	= NULL;				/* @ base.h */
/* ������� */
char *			buffer_output	= NULL;				/* @ base.h */
/* ���󻺳� */
char *			buffer_error	= NULL;				/* @ base.h */

/* ����ո�� */
uint8_t *		read_space		= NULL;				/* @ base.h */
/* ����ƫ���� */
unsigned int	read_offset		= 0;				/* @ base.h */



/* �ڴ��йܱ�ʹ����� */
static	unsigned int	memory_count	= 0;
static	unsigned int	memory_unused	= 0;

/* �ڴ��йܱ� */
static	void **			memory_table	= NULL;




/* �����ڴ� */
void * ec_mm_new (unsigned int size) {
	void *	p	= calloc (1, (size_t) size);

	if(p == NULL) {
		ec_ex_runtime ("failed to allocate memory");
	}

	return p;
}


/* �ط����ڴ� */
void * ec_mm_renew (void *p, unsigned int size) {
	p	= realloc (p, (size_t) size);

	if(p == NULL) {
		ec_ex_runtime ("failed to reallocate memory");
	}

	return p;
}


/*  �ͷ��ڴ� */
void * ec_mm_delete (void *p) {
	if(p != NULL) {
		free (p);
	}

	return NULL;
}




/* ���ڴ��й� */
void ec_mg_open (void) {
	memory_count	= 0;
	memory_unused	= EC_MG_BLOCK;

	memory_table	= (void **) ec_mm_new (sizeof (void *) * EC_MG_BLOCK);
}


/* �ر��ڴ��й� */
void ec_mg_close (void) {
	unsigned int i;

	for(i = 0; i < memory_count; i ++) {
		ec_mm_delete (memory_table [i]);
	}

	ec_mm_delete (memory_table);
}


/* �����й��ڴ� */
void * ec_mg_new (unsigned int size) {
	void *	p	= ec_mm_new (size);

	if(memory_unused == 0) {
		memory_table	= (void **) ec_mm_renew (memory_table, sizeof (void *) * (memory_count + EC_MG_BLOCK));

		memory_unused	= EC_MG_BLOCK;
	}

	memory_table [memory_count]	= p;

	memory_count  ++;
	memory_unused --;

	return p;
}


/* �ط����ڴ��й� */
void * ec_mg_renew (void *p, unsigned int size) {
	unsigned int i;

	for(i = 0; i < memory_count; i ++) {
		if(memory_table [i] == p) {
			break;
		}
	}

	if(memory_count == i) {
		ec_ex_runtime ("managed memory function called with unmanaged memory address");
	}

	p	= ec_mm_renew (p, size);

	memory_table [i]	= p;

	return p;
}




/* ����ʱ�쳣 */
void ec_ex_runtime (const char *message) {
	/* ���׼���������������Ϣ */
	fprintf (stderr, "%s", message);

	/* ��ֹ���� */
	exit (-1);
}


/* �﷨���� */
int ec_ex_syntax (const char *message) {
	unsigned int	i		= 0;
	unsigned int	offset	= 0;

	if(* buffer_error != '\0') {
		return 0;
	}

	/* ����ʵ��ƫ���� */
	while(i < read_offset) {
		/* ��ǰ�ֽ� */
		uint8_t	byte	= read_space [offset / 8];

		/* �жϵ�ǰλ��ʾ���ֽ��Ƿ�Ϊ�ո� */
		if(((byte >> (offset % 8)) & 0x01) == 0) {
			i ++;
		}

		offset ++;
	}

	sprintf (buffer_error, "offset %u : %s", offset, message);

	return 0;
}




/* �� */
void calc_open (const char *input, char *output, char *error) {
	unsigned int	i;
	unsigned int	j;

	/* �����ַ������� */
	unsigned int	length	= strlen (input);

	/* ���ڴ��й� */
	ec_mg_open ( );

	/* �������븱�� */
	buffer_input	= (const char *) ec_mg_new (length + 1);

	/* �����ո�� */
	read_space		= (uint8_t *) ec_mg_new ((length / 8) + 1);
	/* ���ô���ƫ���� */
	read_offset		= 0;

	/* ����ÿ�������ֽ� */
	for(i = j = 0; i < length; i ++) {
		/* �ж��Ƿ�Ϊ�ո� */
		if(isspace (input [i]) != 0) {
			/* �ո���ֽ� */
			uint8_t	byte	= 0x01 << (i % 8);

			/* ���浽�ո�� */
			read_space [i / 8]	|=	byte;
		} else {
			/* ���浽���뱸�� */
			((char *) buffer_input) [j ++]	= input [i];
		}
	}

	/* ���û����� */
	buffer_output	= output;
	buffer_error	= error;

	/* �������������ʹ��󻺳��� */
	* buffer_output	= '\0';
	* buffer_error	= '\0';

	/* ���ôʷ����������ַ������� */
	yy_scan_string (buffer_input);
}


/* ִ�� */
int calc_execute (void) {
	/* ִ�н��� */
	yyparse ( );

	/* �жϼ����Ƿ�ɹ� */
	if(* buffer_error == '\0') {
		return 1;
	} else {
		return 0;
	}
}


/* �ر� */
void calc_close (void) {
	/* �رմʷ������� */
	yylex_destroy ( );

	/* �ر��ڴ��й� */
	ec_mg_close ( );
}

