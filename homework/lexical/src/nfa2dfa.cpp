/**
	$ Lexical   (C) 2005-2013 MF
	$ nfa2dfa.cpp
*/


#include "class.h"



/* NFA ת��Ϊ DFA */
char * lexical::convert_nfa_to_dfa (void) {
	// ��ĸ������
	size_t	count	= this->alphabet.size();

	// ��ȡ���ɵ� DFA �Զ����ṹ
	automata_t *	object	= this->stack_automata.top();

	// ��ʼ״̬��ת���ṹ
	convert_t *		start_convert	= new convert_t;
	// ��ʼ״̬�� �� �հ�
	array_state_t *	start_state		= new array_state_t;

	// ����ʼ״̬���� �� �հ�
	this->nfa_epsilon_closure (object->start, start_state);
	// ����ʼ״̬���� �� �հ���������
	this->nfa_state_sort(start_state);

	// ������ʼ״̬��ת���ṹ
	start_convert->number	= 0;
	start_convert->state	= start_state;

	// ����ʼ״̬��ת���ṹ��ӵ�ת������
	this->matrix.push_back(start_convert);

	// ����ת������
	for(size_t i = 0; i < this->matrix.size(); i ++) {
		// ��ǰת���ṹ
		convert_t *		convert	= this->matrix[i];
		// ��ǰת���ṹ������״̬���հ�
		array_state_t *	closure	= convert->state;

		// ��ѯ NFA �Զ�������״̬�Ƿ���״̬��, �����Ƿ�Ϊ��̬
		convert->accept	= this->nfa_state_exist(closure, object->end);
		// ����ת����������
		convert->to		= new convert_t * [count];

		// ������ĸ��
		for(size_t j = 0; j < count; j ++) {
			char byte	= this->alphabet[j];

			// ������ĸ��ת�Ƶ�����״̬��
			array_state_t * to	= new array_state_t;

			// ��״̬�������Ƶ�, ����ӵ�ǰ״̬���� �� �հ������ַ� byte ��ת�Ƶ�����״̬��
			this->nfa_derivation (byte, closure, to);

			// �����״̬��Ϊ��, ��ʾ�����յ�ǰ�ַ�
			if(to->empty() == true) {
				// �����ڴ�
				delete to;

				// ���ø��ַ���Ӧ��ת��·��Ϊ NULL
				convert->to[j] = NULL;

				continue;
			}

			// ������״̬���е�ÿһ��״̬
			for(size_t k = 0; k < to->size(); k ++) {
				// ����״̬�� �� �հ�
				this->nfa_epsilon_closure (to->at(k), to);
			}

			// ����״̬���� �� �հ���������
			this->nfa_state_sort(to);

			// ��ȡ��״̬���� �� �հ���Ӧ��ת���ṹ
			convert_t * next = this->nfa_convert (to);

			// ���ת���ṹ������
			if(next == NULL) {
				// �����µ�ת���ṹ
				next	= new convert_t;

				// �������
				next->number	= this->matrix.size();
				// ����հ���Ϣ
				next->state		= to;

				// ���浽ת������
				this->matrix.push_back(next);
			} else {
				// ��������
				delete to;
			}

			// ���ý����� byte �ַ���ת�����µ�ת���ṹ
			convert->to[j] = next;
		}
	}

	// ����ת������
	this->nfa_clear();

	return NULL;
}




/* ����״̬��, ��ת���ṹ�в��ٱ��� NFA ״̬�� */
void lexical::nfa_clear (void) {
	// ����ת������
	for(size_t i = 0; i < this->matrix.size(); i ++) {
		delete this->matrix[i]->state;

		// ����ָ��
		this->matrix[i]->state = NULL;
	}
}



/* ��״̬�������Ƶ�, ������һ��״̬����ȡ����һ���ַ������ɵ���״̬�� */
void lexical::nfa_derivation (char byte, array_state_t *from, array_state_t *to) {
	// ������ǰ״̬��
	for(size_t i = 0; i < from->size(); i ++) {
		state_t * state = from->at(i);

		// �����Ե�ǰ״̬Ϊ������������
		for(size_t j = 0; j < from->at(i)->out.size(); j ++) {
			line_t * line = from->at(i)->out[j];

			// �жϵ�ǰ���ߵ������Ƿ�Ϊ����ָ���ַ�
			if(line->byte != byte) {
				continue;
			}

			// �����ߵ��յ���ӵ���״̬����
			if(find(to->begin(), to->end(), line->to) == to->end()) {
				to->push_back(line->to);
			}
		}
	}
}



/* ����״̬�� �� �հ� */
void lexical::nfa_epsilon_closure (state_t *from, array_state_t *result) {
	// ��ѯ����
	queue<state_t *> query;

	// ����һ��״̬�����
	query.push(from);

	// ��������ֱ��û����Ҫ�����״̬
	while(query.empty() == false) {
		state_t * state = query.front();

		// ɾ������Ԫ��
		query.pop();

		// ����ǰ״̬���뵽���״̬��
		if(find(result->begin(), result->end(), state) == result->end()) {
			result->push_back(state);
		}

		// ���������Ե�ǰ״̬Ϊ��������
		for(size_t i = 0; i < state->out.size(); i ++) {
			line_t * line = state->out[i];

			// ֻ���� �� ��
			if(line->byte != 0) {
				continue;
			}
			// �ж������յ��Ƿ�Ϊ������״̬
			if(line->to == state) {
				continue;
			}

			// �ж������յ��Ƿ��Ѿ������ڽ��״̬��
			if(find(result->begin(), result->end(), line->to) == result->end()) {
				// �����ڵĻ��������
				query.push(line->to);
			}
		}
	}
}




/* ״̬������ */
void lexical::nfa_state_sort (array_state_t *set) {
	sort(set->begin(), set->end());
}


/* ��״̬�Ƿ������״̬�� */
bool lexical::nfa_state_exist (array_state_t *set, state_t *state) {
	if(find(set->begin(), set->end(), state) == set->end()) {
		return false;
	}

	return true;
}



/* ��ȡ״̬����Ӧ��ת���ṹ */
convert_t * lexical::nfa_convert (array_state_t *set) {
	// ����ת������
	for(size_t i = 0; i < this->matrix.size(); i ++) {
		// ��ǰת���ṹ��״̬��
		array_state_t * closure = this->matrix[i]->state;

		// �Ƚ�״̬����С
		if(closure->size() != set->size()) {
			continue;
		}

		// �����Ƚ�״̬������
		for(size_t j = 0; j < set->size(); j ++) {
			if(set->at(j) != closure->at(j)) {
				goto next;
			}
		}

		// �Ƚϳɹ�
		return this->matrix[i];

next:
		// �Ƚ�ʧ��
		continue;
	}

	return NULL;
}

