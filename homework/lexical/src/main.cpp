/**
	$ Lexical   (C) 2005-2013 MF
	$ main.cpp
*/


#include "class.h"



int main (int argc, char *argv[]) {
	// ʹ�ñ�׼��������������
	lexical lex (stdin);

	// ���벢ת��
	if(lex.read() == false) {
		// ת��ʧ��, �������
		cout << endl << endl << endl;

		// ��ͣ�������û�����������������
		system("pause");

		// ֹͣ����
		return 0;
	}

	// �����ʾ
	cout << endl << endl << "״̬ת������" << endl;

	// �������
	lex.output();
	// ����ƥ��ģʽ
	lex.match();

	return 0;
}

