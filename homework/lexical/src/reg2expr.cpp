/**
	$ Lexical   (C) 2005-2013 MF
	$ reg2expr.cpp
*/


#include "class.h"



/* ����ʽת��Ϊ��׺���ʽ */
char * lexical::convert_regular_to_expression (void) {
	char	byte;

	// �Ѷ�ȡ�ַ�����
	size_t	count	= 0;
	// �ַ�ת��ģʽ
	bool	escape	= false;

	// ��һ��������ַ���Ϣ
	bool	prev_alphabet	= false;
	char	prev_byte		= 0;

	// ������ʾ
	cout << "������������ʽ��" << endl;

	while(1) {
		// ��ȡ�ַ�
		cin >> noskipws >> byte;

		// �Ѷ��ַ�����
		count ++;

		// ���������� NUL
		if(byte == 0) {
			return "��������Ч���ַ�";
		}
		// �������н���
		if((byte == '\r') || (byte == '\n')) {
			break;
		}

		// ����ת��ģʽ
		if(escape == false) {
			// �����ȡ�� \, ��һ���ַ�Ӧ����Ϊ��ĸ
			if(byte == '\\') {
				// ��ת��ģʽ
				escape = true;

				// ֱ�ӽ�������ѭ��, ������һ�ֶ�ȡ
				continue;
			}
		}

		// ��ǰ�ַ��Ƿ�������ĸ
		bool	alphabet	= false;
		// �������
		char *	error		= NULL;

		// ���ת��ģʽ�Ѿ���, ���۶���ʲô����Ϊ��ĸ
		if(escape == true) {
			// ������ĸ���
			alphabet	= true;
		} else {
			// ���������
			switch(byte) {
				// ������, ����ǰ���������һ������
				case '(':			this->regular_concat		(prev_alphabet, prev_byte);
							error =	this->regular_bracket_left	();

							break;

				// ������
				case ')':	error =	this->regular_bracket_right	();		break;
				// ��
				case '|':	error =	this->regular_or			();		break;
				// �հ�
				case '*':	error =	this->regular_closure		();		break;

				// �����ַ���Ϊ��ĸ
				default:
					// ������ĸ���
					alphabet	= true;
			}
		}

		// ������ĸ
		if(alphabet == true) {
			// ÿ����ĸǰ������һ������
			this->regular_concat(prev_alphabet, prev_byte);

			// ��ӵ�ǰ�ַ�����ĸ����
			error = this->regular_alphabet (byte);
		}

		// ���ִ���
		if(error != NULL) {
			// ������Ϣ����
			size_t	length	= strlen(error);
			// ����ռ�, Ԥ������ռ�
			char *	message	= new char [length + 64];

			// ���ɰ����ַ�λ�õĴ�����Ϣ
			sprintf(message, "%s ���ڵ� %d ���ַ�����", error, count);

			return message;
		}

		// �ر�ת��ģʽ
		escape	= false;

		// ���汾�δ�����ַ�
		prev_alphabet	= alphabet;
		prev_byte		= byte;
	}

	// ��������ʽ������ת��Ӧ�ùر�, �����������ʽ�� \ ����
	if(escape == true) {
		return "��Ч��ת��";
	}

	// ��ջ��ʣ�������������Żر��ʽ����
	while(this->stack_byte.empty() == false) {
		// ջ�������
		byte = this->stack_byte.top();

		// ��������� '(', ���Ų�ƥ��
		if(byte == '(') {
			return "���Ų�ƥ��";
		}

		// ������ǰ�����
		this->stack_byte.pop();
		// ���ɱ��ʽ����
		this->regular_byte(false, byte);
	}

	// �ж������ʽ�����ж�����Ŀ
	if(this->expression.size() == 0) {
		return "û�ж����κι���";
	}

	return NULL;
}




/* �����ַ� */
char * lexical::regular_byte (bool alphabet, char byte) {
	// �������ʽ����
	expression_t *	object	= new expression_t;

	object->alphabet	= alphabet;
	object->byte		= byte;

	// ��ӵ����ʽ��������
	this->expression.push_back(object);

	return NULL;
}



/* ������ĸ�ַ� */
char * lexical::regular_alphabet (char byte) {
	// �����ַ�
	lexical::regular_byte(true, byte);

	return NULL;
}



/* ���������� */
char * lexical::regular_bracket_left (void) {
	// ������ֱ��ѹջ
	this->stack_byte.push('(');

	return NULL;
}


/* ���������� */
char * lexical::regular_bracket_right (void) {
	// �������������
	while(this->stack_byte.empty() == false) {
		// ��ȡջ���ַ�
		char byte = this->stack_byte.top();

		// ����ջ��
		this->stack_byte.pop();

		// ����������, ����ѭ��
		if(byte == '(') {
			return NULL;
		}

		this->regular_byte (false, byte);
	}

	// δ���������žͽ���, ˵�����Ų�ƥ��
	return "���Ų�ƥ��";
}



/* �������� */
char * lexical::regular_concat (bool alphabet, char byte) {
	// ���������ǵ�һ���ַ�, ���������
	if(byte == 0) {
		return NULL;
	}

	// ֻ����һ���ַ�Ϊ��ĸ ')' ��հ����������
	if(alphabet == false) {
		if((byte != ')') && (byte != '*')) {
			return NULL;
		}
	}

	// �����հ�����������
	while(this->stack_byte.empty() == false) {
		char byte = this->stack_byte.top();

		// ֻ�ж��� '*' �� '&' �ŵ���
		if((byte == '*') || (byte == '&')) {
			this->stack_byte.pop();
			this->regular_byte(false, byte);

			continue;
		}

		// ������Ϊ���������Ż����ȼ����͵��ַ�
		break;
	}

	// ��ǰ�����ѹջ
	this->stack_byte.push('&');

	return NULL;
}


/* ����� */
char * lexical::regular_or (void) {
	while(this->stack_byte.empty() == false) {
		char byte = this->stack_byte.top();

		// ֻҪ�������� '(' ����������ӵ����Ա�
		if(byte != '(') {
			this->stack_byte.pop();
			this->regular_byte(false, byte);

			continue;
		}

		break;
	}

	this->stack_byte.push('|');

	return NULL;
}


/* ����հ� */
char * lexical::regular_closure (void) {
	while(this->stack_byte.empty() == false) {
		char byte = this->stack_byte.top();

		// ֻ�ж��� '*' �ŵ�������ӵ����Ա�
		if(byte == '*') {
			this->stack_byte.pop();
			this->regular_byte(false, byte);

			continue;
		}

		break;
	}

	this->stack_byte.push('*');

	return NULL;
}

