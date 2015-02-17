/**
	$ Calculator   (C) 2005-2014
	$ base.h
*/


#ifndef _BASE_H_
#define _BASE_H_


#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <vector>





/* �����ʽ���� */
#define EXPRESSION_MAX		1024




/* �ڵ� */
typedef struct {
	char	symbol;						// �����
	double	value;						// ��ֵ
} node_t;





/* �ʷ������� */
class lexical {
	private:
		static	char *	buffer;

		static	char *	pointer;
		static	char *	boundary;


		static	void	trim	(void);


	public:
		static	void	read	(FILE *stream);
		static	bool	next	(char *symbol, double *value, int *error);
};




/* �﷨������ */
class parser {
	private:
		static	std::vector <node_t *> *	stack;


		static	void	shift		(char symbol, double value);
		static	bool	reduce		(void);

		static	double	execute		(int *error);

		static	char	top			(void);
		static	char	priority	(char top, char read);

		static	void	flush		(void);


	public:
		static	void	loop	(void);
};



#endif   /* BASE */