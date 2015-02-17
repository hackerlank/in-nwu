/**
	$ Lexical   (C) 2005-2013 MF
	$ expr2nfa.cpp
*/


#include "class.h"



/* ��׺���ʽת��Ϊ NFA */
char * lexical::convert_expression_to_nfa (void) {
	// �������ʽ����
	for(int i = 0, j = this->expression.size(); i < j; i++) {
		// �������
		char *			error	= NULL;
		// ��ǰ��
		expression_t *	object	= this->expression[i];

		if(object->alphabet == true) {
			// ������ĸ
			error = this->expression_alphabet (object->byte);
		} else {
			// ���������
			switch(object->byte) {
				// ����
				case '&':	error = this->expression_concat		();		break;
				// ��
				case '|':	error = this->expression_or			();		break;
				// �հ�
				case '*':	error = this->expression_closure	();		break;
			}
		}

		// ������ִ���
		if(error) {
			return error;
		}
	}

	// ������ջ��Ӧ��ֻ��һ��Ԫ��
	if(this->stack_automata.size() != 1) {
		return "�ṹ����ȷ";
	}

	// ����ĸ���������
	sort(this->alphabet.begin(), this->alphabet.end());

	return NULL;
}




/* ������ĸ */
char * lexical::expression_alphabet (char byte) {
	// �����Զ����ṹ
	automata_t *	object	= new automata_t;

	// ��ʼ�ڵ�ͽ����ڵ�
	state_t *	start	= new state_t;
	state_t *	end		= new state_t;

	// �����Զ�����ʼ�ͽ����ڵ������
	line_t *	line	= new line_t;

	// ��ʼ�ڵ���� byte ��ת�Ƶ������ڵ�
	line->byte		= byte;
	line->from		= start;
	line->to		= end;

	// ��������
	start->out.push_back	(line);
	end  ->in .push_back	(line);

	// �Զ����� start ��ʼ, end ����
	object->start	= start;
	object->end		= end;

	// ���Զ����ṹ��ջ
	this->stack_automata.push(object);

	// ������ĸ�����Ƿ���ڵ�ǰ������ַ�
	if(find(this->alphabet.begin(), this->alphabet.end(), byte) == this->alphabet.end()) {
		// ����ַ�����ĸ����
		this->alphabet.push_back(byte);
	}

	return NULL;
}



/* �������� */
char * lexical::expression_concat (void) {
	// ����������Ҫ����Ԫ��
	if(this->stack_automata.size() < 2) {
		return "���������������Ҫ�� 2 ��Ԫ��";
	}

	// ������һ���Զ����ṹ
	automata_t *	object_2	= this->stack_automata.top();	this->stack_automata.pop();
	// ����ǰһ���Զ����ṹ
	automata_t *	object_1	= this->stack_automata.top();	this->stack_automata.pop();

	// ��������
	line_t * line	= new line_t;

	// �� ������ǰһ���ṹ�Ľ����ڵ�ͺ�һ���ṹ�Ŀ�ʼ�ڵ�
	line->byte		= 0;
	line->from		= object_1->end;
	line->to		= object_2->start;

	// ��������
	object_1->end  ->out.push_back	(line);
	object_2->start->in .push_back	(line);

	// �ϲ������ṹ, ����ǰһ���ṹ�Ľ����ڵ�Ϊ��һ���ṹ�Ľ����ڵ�
	object_1->end	= object_2->end;

	// ɾ����һ���ṹ
	delete object_2;

	// ���ϲ�����Զ����ṹ��ջ
	this->stack_automata.push(object_1);

	return NULL;
}


/* ����� */
char * lexical::expression_or (void) {
	// ��������Ҫ����Ԫ��
	if(this->stack_automata.size() < 2) {
		return "�������������Ҫ�� 2 ��Ԫ��";
	}

	// ��һ���ṹ
	automata_t *	object_1	= this->stack_automata.top();	this->stack_automata.pop();
	// �ڶ����ṹ
	automata_t *	object_2	= this->stack_automata.top();	this->stack_automata.pop();

	// �����µ���ʼ�ڵ�ͽ����ڵ�
	state_t *	state_start	= new state_t;
	state_t *	state_end	= new state_t;

	// �µ���ʼ�ڵ�����
	line_t *	line_start_1	= new line_t;
	line_t *	line_start_2	= new line_t;
	// �µ���ֹ�ڵ�����
	line_t *	line_end_1		= new line_t;
	line_t *	line_end_2		= new line_t;

	// ��һ�� �� �������µ���ʼ�ڵ㵽��һ���ṹ����ʼ�ڵ�
	line_start_1->byte		= 0;
	line_start_1->from		= state_start;
	line_start_1->to		= object_1->start;

	// �ڶ��� �� �����ӵ�һ���ṹ�Ľ����ڵ���µĽ����ڵ�
	line_end_1->byte		= 0;
	line_end_1->from		= object_1->end;
	line_end_1->to			= state_end;

	// ������ �� �������µ���ʼ�ڵ㵽�ڶ����ṹ����ʼ�ڵ�
	line_start_2->byte		= 0;
	line_start_2->from		= state_start;
	line_start_2->to		= object_2->start;

	// ������ �� �����ӵڶ����ṹ�Ľ����ڵ���µĽ����ڵ�
	line_end_2->byte		= 0;
	line_end_2->from		= object_2->end;
	line_end_2->to			= state_end;

	// �����һ����
	state_start->out.push_back		(line_start_1);
	object_1->start->in.push_back	(line_start_1);

	// ����ڶ�����
	object_1->end->out.push_back	(line_end_1);
	state_end->in.push_back			(line_end_1);

	// �����������
	state_start->out.push_back		(line_start_2);
	object_2->start->in.push_back	(line_start_2);

	// �����������
	object_2->end->out.push_back	(line_end_2);
	state_end->in.push_back			(line_end_2);

	// �ϲ��µĽڵ�������ṹ
	object_1->start		= state_start;
	object_1->end		= state_end;

	// ɾ���ڶ����ṹ
	delete object_2;

	// ���ϲ����Ԫ����ջ
	this->stack_automata.push(object_1);

	return NULL;
}


/* ����հ� */
char * lexical::expression_closure (void) {
	// �հ�������Ҫһ��Ԫ��
	if(this->stack_automata.size() < 1) {
		return "�հ������������Ҫ�� 1 ��Ԫ��";
	}

	// ��ȡջ���Զ����ṹ
	automata_t *	object	= this->stack_automata.top();

	// �����µ���ʼ�ڵ�ͽ����ڵ�
	state_t *	state_start	= new state_t;
	state_t *	state_end	= new state_t;

	line_t *	line_1	= new line_t;
	line_t *	line_2	= new line_t;
	line_t *	line_3	= new line_t;
	line_t *	line_4	= new line_t;

	// ��һ�� �� �������µ���ʼ�ڵ�ͽṹ��ʼ�ڵ�
	line_1->byte	= 0;
	line_1->from	= state_start;
	line_1->to		= object->start;

	// �ڶ��� �� �������µ���ʼ�ڵ���µĽ����ڵ�
	line_2->byte	= 0;
	line_2->from	= state_start;
	line_2->to		= state_end;

	// ������ �� �����ӽṹ�����ڵ���µĽ����ڵ�
	line_3->byte	= 0;
	line_3->from	= object->end;
	line_3->to		= state_end;

	// ������ �� �������µĽ����ڵ���µ���ʼ�ڵ�
	line_4->byte	= 0;
	line_4->from	= state_end;
	line_4->to		= state_start;

	// �����һ����
	state_start->out.push_back		(line_1);
	object->start->in.push_back		(line_1);

	// ����ڶ�����
	state_start->out.push_back		(line_2);
	state_end->in.push_back			(line_2);

	// �����������
	object->end->out.push_back		(line_3);
	state_end->in.push_back			(line_3);

	// �����������
	state_end->out.push_back		(line_4);
	state_start->in.push_back		(line_4);

	// ����Ԫ�ؿ�ʼ�ͽ����ڵ�
	object->start	= state_start;
	object->end		= state_end;

	return NULL;
}

