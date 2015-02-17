/**
	$ Lexical   (C) 2005-2013 MF
	$ call.cpp
*/


#include "class.h"



/* ���� */
lexical:: lexical	(FILE *stream) : in (stream)	{ }
/* ���� */
lexical::~lexical	(void)	{ }



/* ��������ʽ������ת�� */
bool lexical::read (void) {
	// ת�������б�
	char * (lexical:: * handler [])(void) = {
		& lexical::convert_regular_to_expression,
		& lexical::convert_expression_to_nfa,
		& lexical::convert_nfa_to_dfa,
		& lexical::convert_dfa_to_simpdfa
	};

	// ���ε���ת������
	for(int i = 0; i < (sizeof(handler) / sizeof(void *)); i ++) {
		// ִ�д���
		char *	error	 = (this->* handler[i])();

		// ������ִ���, ������󲢽���
		if(error != NULL) {
			cout << endl << "ת�����ִ���" << endl << error;

			return false;
		}
	}

	return true;
}



/* ������� */
void lexical::output (void) {
	// ��ĸ��Ԫ������
	size_t count	= this->alphabet.size();

	// �����һ�еĿո�
	cout << "\t";

	// ��һ��Ϊ�����������ĸ��
	for(size_t i = 0; i < count; i ++) {
		cout << "\t" << this->alphabet[i];
	}

	// ����״̬��ת������
	for(size_t i = 0; i < this->matrix.size(); i ++) {
		// �������
		cout << endl;

		// ��ǰת���ṹ
		convert_t *	convert = this->matrix[i];

		// ���״̬���
		if(convert->accept == true) {
			// ���Ϊ��̬, �����ǰ���������
			cout << "\t(" << convert->number << ")";
		} else {
			// ���Ϊ����̬, ֱ��������
			cout << "\t"  << convert->number;
		}

		// ������ǰת����������
		for(size_t j = 0; j < count; j ++) {
			// �жϵ�ǰת���ṹ�Ƿ���յ�ǰ�ַ�
			if(convert->to[j] == NULL) {
				// ���������, ��� -
				cout << "\t-";
			} else {
				// �������, �����״̬���
				cout << "\t"  << convert->to[j]->number;
			}
		}
	}

	// ���һ�к��������
	cout << endl;
}



/* ����ƥ��ģʽ */
void lexical::match (void) {
	while(1) {
		// �������������
		fflush(stdin);

		// �������
		cout << endl << endl;
		// �����ʾ
		cout << "������Ҫƥ���ַ�����";

		// ��������ֽ�
		char	byte	= 0;
		// ��ǰ���ڵ�ת���ṹ
		convert_t *	convert	= this->matrix[0];

		while(convert != NULL) {
			// �����ַ�
			cin >> noskipws >> byte;

			// ��������, ����ѭ��
			if((byte == '\n') || (byte == '\r')) {
				break;
			}

			// ���Ҷ�����ַ�����ĸ���е�λ��
			vector<char>::iterator iterator = find(this->alphabet.begin(), this->alphabet.end(), byte);

			// �ж��Ƿ�����˲�����ĸ���е��ַ�
			if(iterator == this->alphabet.end()) {
				// �����޷�ת��
				convert = NULL;
			} else {
				// ��������ַ�����ĸ����ʵ������
				size_t index = distance(this->alphabet.begin(), iterator);

				// ����ת���ṹ
				convert = convert->to[index];
			}
		}

		// �ж���ֹʱ��ת��״̬
		if((convert == NULL) || (convert->accept == false)) {
			cout << "������ַ�����ƥ�䡣";
		} else {
			cout << "������ַ����ɹ�ƥ�䡣";
		}
	}
}

