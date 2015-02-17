/**
	$ Calculator   (C) 2005-2014
	$ lexical.cpp
*/


#include "base.h"



/* ���뻺���� */
char *	lexical::buffer		= NULL;

/* ��ǰ����ָ�� */
char *	lexical::pointer	= NULL;
/* �����߽�ָ�� */
char *	lexical::boundary	= NULL;





/* ���˿ո� */
void lexical::trim (void) {
	char *	source	= lexical::buffer;
	char *	target	= lexical::buffer;

	// �����ַ���
	while(* source != 0) {
		// �ж��Ƿ�Ϊ�ո�
		if(isspace (* source) == 0) {
			// �ƶ��ַ�
			if(source != target) {
				* target	= * source;
			}

			// Ŀ��ָ����һ���ַ�
			target ++;
		}

		// Դָ����һ���ַ�
		source ++;
	}

	* target	= 0;
}




/* ��ȡ���ʽ */
void lexical::read (FILE *stream) {
	// ���仺����
	if(lexical::buffer == NULL) {
		lexical::buffer	= new char [EXPRESSION_MAX];
	}

	// ��ȡ���ʽ
	fgets (lexical::buffer, EXPRESSION_MAX, stream);

	// �������пո�
	lexical::trim ( );

	// ����ָ��
	lexical::pointer	= lexical::buffer;
	lexical::boundary	= lexical::buffer + strnlen (lexical::buffer, EXPRESSION_MAX);
}



/* ������һ������ */
bool lexical::next (char *symbol, double *value, int *error) {
	// �ж��Ƿ񵽴��ַ�����β
	if(lexical::pointer == lexical::boundary) {
		return false;
	}

	// ��ʼ������ֵ
	* symbol	= 0;
	* value		= 0.;
	* error		= -1;

	// ���������
	switch(* lexical::pointer) {
		// ��ͨ�����
		case '+':
		case '*':
		case '/':
		case '(':
		case ')':
			goto found_symbol;

		// ����/����
		case '-':
			// �ж���һ���ַ��Ƿ�Ϊ '-'
			if(* (lexical::pointer + 1) == '-') {
				goto found_symbol;
			} else {
				// �ж�ǰһ���ַ��Ƿ�Ϊ�����
				if(lexical::pointer == lexical::buffer) {
					goto found_number;
				} else {
					switch(* (lexical::pointer - 1)) {
						case '+':
						case '-':
						case '*':
						case '/':
						case '(':
							goto found_number;
					}

					goto found_symbol;
				}
			}
	}


found_number:
	// ��ֹƫ����
	int	offset_from	= 0;
	int	offset_to	= 0;

	// ��ȡ
	if(sscanf (lexical::pointer, "%n%lf%n", & offset_from, value, & offset_to) < 1) {
		goto error;
	}

	// �ƶ����ַ���
	lexical::pointer += offset_to - offset_from;

	return true;


found_symbol:
	// ���ظ���
	* symbol	= * lexical::pointer;

	// ���ų���Ϊ 1
	lexical::pointer ++;

	return true;


error:
	// ����������ƫ����
	* error	= lexical::pointer - lexical::buffer;

	// ���ض�ȡʧ��
	return false;
}

