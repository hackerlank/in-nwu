/**
	$ Calculator   (C) 2005-2014
	$ parser.cpp
*/


#include "base.h"



/* ����ջ */
std::vector <node_t *> *	parser::stack	= NULL;





/* �ƽ� */
void parser::shift (char symbol, double value) {
	// �����µĽڵ�
	node_t *	node	= new node_t;

	// ����ڵ�����
	node->symbol	= symbol;
	node->value		= value;

	// �ڵ��ջ
	parser::stack->push_back (node);
}



/* ��Լ */
bool parser::reduce (void) {
	// �����в����� 3 ���ڵ���й�Լ
	if(parser::stack->size ( ) < 3) {
		return false;
	}

	// ���λ�ȡ 3 ��Ԫ��
	node_t *	node_1	= parser::stack->back ( );		parser::stack->pop_back ( );
	node_t *	node_2	= parser::stack->back ( );		parser::stack->pop_back ( );
	node_t *	node_3	= parser::stack->back ( );		parser::stack->pop_back ( );

	// S -> (S)
	if((node_3->symbol == '(') && (node_2->symbol == 0) && (node_1->symbol == ')')) {
		parser::shift (0, node_2->value);

		return true;
	}

	// S -> S op S
	if((node_3->symbol == 0) && (node_1->symbol == 0)) {
		switch(node_2->symbol) {
			case '+':	parser::shift (0, node_3->value + node_1->value);	break;
			case '-':	parser::shift (0, node_3->value - node_1->value);	break;
			case '*':	parser::shift (0, node_3->value * node_1->value);	break;

			case '/':
				if(node_1->value == 0) {
					return false;
				} else {
					parser::shift (0, node_3->value / node_1->value);
				}
				break;

			default:	return false;
		}

		return true;
	}

	return false;
}




/* ִ�м��� */
double parser::execute (int *error) {
	// ��ʼ������
	* error	= -1;

	// ѭ����ȡ����
	while(true) {
		// �����
		char	symbol	= 0;
		// ��ֵ
		double	value	= 0.;

		// ��ȡ����
		if(lexical::next (& symbol, & value, error) == false) {
			// �ж��Ƿ��ɴ����µĽ���
			if(* error != -1) {
				// �ʷ�����
				(* error) ++;

				// ����ʧ��
				return 0.;
			}

			goto found_symbol;
		}

		// �жϵ�ǰ�Ƿ���������
		if(symbol != 0) {
			goto found_symbol;
		}

		// ��ֱֵ�ӽ�ջ, �ƽ�
		parser::shift (symbol, value);

		continue;


found_symbol:
		while(true) {
			// ջ������
			char	symbol_top	= parser::top ( );

			// ���ջ���������, ����Ҫ��Լ
			if(symbol_top == 0) {
				break;
			}

			// ���ݷ������ȼ��ж��Ƿ���Ҫ��Լ
			if(parser::priority (symbol_top, symbol) != '>') {
				break;
			}

			// �����Ҫ��Լ, ִ�й�Լ
			if(parser::reduce ( ) == false) {
				goto failed;
			}
		}

		if(symbol == 0) {
			// �����ȡ���, ����ѭ��
			break;
		} else {
			// ���û�ж�ȡ���, �ƽ������
			parser::shift (symbol, value);
		}
	}

	// ջ�д�ʱӦ��ֻʣ��һ��Ԫ��
	if(parser::stack->size ( ) == 1) {
		// ֱ�ӷ��ؽ��
		return parser::stack->back ( )->value;
	}


failed:
	// �﷨����
	* error	= 0;

	// ����ʧ��
	return 0.;
}




/* ջ������� */
char parser::top (void) {
	// �������
	int	i	= parser::stack->size ( ) - 1;

	// ����ջ
	while(i >= 0) {
		// �����ڵ�
		node_t *	node	= parser::stack->at (i);

		// �����Ƿ�Ϊ����
		if(node->symbol > 0) {
			return node->symbol;
		}

		i --;
	}

	return 0;
}



/* �Ƚ����ȼ� */
char parser::priority (char top, char read) {
	switch(top) {
		case '+':
		case '-':
			switch(read) {
				case  0 :
				case '+':
				case '-':
					return '>';
				case '*':
				case '/':
				case '(':
					return '<';
				case ')':
					return '>';
			}
			break;

		case '*':
		case '/':
			switch(read) {
				case  0 :
				case '+':
				case '-':
				case '*':
				case '/':
					return '>';
				case '(':
					return '<';
				case ')':
					return '>';
			}
			break;

		case '(':
			switch(read) {
				case '+':
				case '-':
				case '*':
				case '/':
				case '(':
					return '<';
			}
			break;

		case ')':
			switch(read) {
				case  0 :
				case '+':
				case '-':
				case '*':
				case '/':
				case ')':
					return '>';
			}
			break;
	}

	return 0;
}




/* ����ջ */
void parser::flush (void) {
	// ����ջֱ��û�нڵ�
	while(true) {
		// �ж�ջ��
		if(parser::stack->size ( ) <= 0) {
			break;
		}

		// ջ��
		node_t *	node	= parser::stack->back ( );

		// �ͷŽڵ�
		delete node;

		// ����ջ��
		parser::stack->pop_back ( );
	}
}





/* ������ѭ�� */
void parser::loop (void) {
	// ��ʼ��ջ
	parser::stack	= new std::vector <node_t *>;

	// ��ѭ��
	while(true) {
		// ������ʾ
		fprintf	(stdout, "�������������ʽ��");
		// ������뻺����
		fflush	(stdin);

		// ���ôʷ�������׼��
		lexical::read (stdin);

		// ����λ��
		int		error	= 0;
		// ������
		double	result	= parser::execute (& error);

		// �ж��Ƿ��������
		if(error == -1) {
			// ���������
			fprintf (stdout, "��������%lf", result);
		} else {
			// ���������Ϣ
			if(error == 0) {
				fprintf (stdout, "�����������ı��ʽ��ʽ����ȷ");
			} else {
				fprintf (stdout, "������󣺵� %d ���ַ�������Ԥ��֮����ַ�", error);
			}
		}

		// �������
		fprintf (stdout, "\n\n\n");

		// ����ջ
		parser::flush ( );
	}
}

