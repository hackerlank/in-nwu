/**
	$ Lexical   (C) 2005-2013 MF
	$ dfa2simpdfa.cpp
*/


#include "class.h"



/* DFA ת��Ϊ��� DFA */
char * lexical::convert_dfa_to_simpdfa (void) {
	// �������
	char * error;

	// ״̬����
	size_t	count_matrix	= this->matrix.size();
	// ��ĸ����
	size_t	count_alphabet	= this->alphabet.size();

	// ��ֹ״̬����
	array_bool_t *	state_end		= NULL;
	// ״̬ת������
	array_state_t *	state_matrix	= this->dfa_state_build (& state_end);

	// ״̬��
	vector <array_state_t *>	state_set;

	// ��ʼ��״̬��
	this->dfa_set_initialize (state_matrix, state_end, & state_set);

	// ������ĸ��
	for(size_t i = 0; i < count_alphabet; i ++) {
		// ���ݲ�ͬ���ַ���״̬�������и�
		this->dfa_set_split(i, & state_set, state_end);
	}

	// ���ݺϲ����״̬�������µ�ת���ṹ����
	if(error = this->dfa_state_convert(& state_set, state_end)) {
		return error;
	}

	return NULL;
}




/* ���ɽڵ����, ����ת���ṹ����ת��Ϊ�ڵ���� */
array_state_t * lexical::dfa_state_build (array_bool_t **state_end) {
	// ����
	size_t	count_matrix	= this->matrix.size();
	size_t	count_alphabet	= this->alphabet.size();

	// ��ֹ״̬����
	*state_end	= new array_bool_t;

	// ״̬����
	array_state_t *	states	= new array_state_t;

	// ��ʼ������
	for(size_t i = 0; i < count_matrix; i ++) {
		// ����һ����״̬
		states->push_back(new state_t);

		// �ж��Ƿ�Ϊ��ֹ״̬
		(*state_end)->push_back(this->matrix[i]->accept);
	}

	// ��ʼ��������ÿһ��״̬
	for(size_t i = 0; i < count_matrix; i ++) {
		// ������ĸ��
		for(size_t j = 0; j < count_alphabet; j ++) {
			// ת���ṹ��ָ���ַ���ת������
			convert_t * convert = this->matrix[i]->to[j];

			if(convert == NULL) {
				// �����յ�ǰ�ַ�
				states->at(i)->out.push_back(NULL);
			} else {
				// �����µ�����
				line_t * line	= new line_t;

				// ָ��Ŀ��ת���ṹ��Ӧ��״̬�ڵ�
				line->byte	= this->alphabet[j];
				line->from	= states->at(i);
				line->to	= states->at(convert->number);

				// ����Ŀ�Ľڵ�
				states->at(i)->out.push_back(line);
				// ����Դ�ڵ�
				states->at(convert->number)->in.push_back(line);
			}
		}
	}

	return states;
}



/* ���ýڵ����, ���µ�״̬���ϲ����� */
char * lexical::dfa_state_convert (vector <array_state_t *> *list, array_bool_t *end) {
	// ��յ�ǰ����
	this->matrix.clear();

	size_t	count_list		= list->size();
	size_t	count_alphabet	= this->alphabet.size();

	// ��ʼ��״̬������
	for(size_t i = 0; i < count_list; i ++) {
		// ��ǰ״̬��
		array_state_t * set = list->at(i);

		// ����һ��ת���ṹ
		convert_t * convert = new convert_t;

		convert->accept		= end->at(i);
		convert->number		= this->matrix.size();
		convert->state		= NULL;
		convert->to			= new convert_t * [count_alphabet];

		// ��ӵ�������
		this->matrix.push_back(convert);
	}

	// ת��״̬������Ϊ����
	for(size_t i = 0; i < count_list; i ++) {
		// �ӵ�ǰ״̬��������ȡһ��״̬
		state_t *		state	= list->at(i)->at(0);
		// ת��Ŀ��״̬����
		array_line_t *	lines	= & state->out;

		// ����״̬��ת��Ŀ��״̬����
		for(size_t j = 0; j < count_alphabet; j ++) {
			// ���û��Ŀ��״̬, ��������������ַ�
			if(lines->at(j) == NULL) {
				this->matrix[i]->to[j] = NULL;

				continue;
			}

			// ���״̬�����
			size_t index = 0;

			// �ٴα���״̬��, ����Ŀ��״̬���ڵ�״̬�����
			for(; index < count_list; index ++) {
				if(find(list->at(index)->begin(), list->at(index)->end(), lines->at(j)->to) != list->at(index)->end()) {
					goto found;
				}
			}

			// û���ҵ�
			return "û���ҵ�Ŀ��״̬��";

found:
			// ����Ŀ��״̬
			this->matrix[i]->to[j] = this->matrix[index];
		}
	}

	return NULL;
}




/* ��ʼ��״̬��, ����״̬����ת��Ϊ״̬������ */
void lexical::dfa_set_initialize (array_state_t *list, array_bool_t *end, vector <array_state_t *> *set) {
	// �ж��Ƿ��з���̬
	bool	exist	= false;

	// ��̬״̬������
	size_t	index_end	= 0;
	// ����̬״̬������
	size_t	index_unend	= 0;

	// ����Ƿ��з���̬��״̬
	if(find(end->begin(), end->end(), false) != end->end()) {
		exist = true;
	}

	// ����״̬������
	if(end->at(0) == true) {
		// ����̬Ϊ�� 2 ��
		index_unend	= 1;
	} else {
		// ��̬Ϊ�� 2 ��
		index_end	= 1;
	}

	// Ϊ״̬���������һ��״̬��
	set->push_back(new array_state_t);

	// ������ڷ���̬, �����һ��״̬��, ��ʱ�� 0 ��Ϊ����̬״̬��, �� 1 ��Ϊ��̬״̬��
	if(exist == true) {
		set->push_back(new array_state_t);
	}

	// ��������״̬
	for(size_t i = 0; i < list->size(); i ++) {
		// �ж��Ƿ�Ϊ��̬
		if(end->at(i) == true) {
			// ���״̬����ֹ״̬, ֱ�Ӽ�����̬״̬��
			set->at(index_end)->push_back(list->at(i));
		} else {
			// �������̬״̬��
			set->at(index_unend)->push_back(list->at(i));
		}
	}

	// ���������ֹ״̬��Ϣ
	end->clear();

	if(exist == true) {
		// ������ڷ���̬
		if(index_end == 1) {
			end->push_back(false);
			end->push_back(true);
		} else {
			end->push_back(true);
			end->push_back(false);
		}
	} else {
		// ��������ڷ���̬, ֻ��һ����Ϊ����̬״̬��
		end->push_back(true);
	}
}



/* �ָ�״̬��, �����ݶ�����ַ���״̬�����з��� */
void lexical::dfa_set_split (size_t byte, vector <array_state_t *> *list, array_bool_t *end) {
	// ���ѿ�ʼǰ״̬������
	size_t	count_set		= list->size();
	// ״̬��Ŀ
	size_t	count_matrix	= this->matrix.size();

	// ����״̬��
	for(size_t i = 0; i < count_set; i ++) {
		// ��ǰ״̬��
		array_state_t *	set	= list->at(i);

		// ��Ҫ�ָ��ȥ��״̬��Ŀ��״̬��
		array_state_t *	split_set	= new array_state_t [count_matrix + 1];

		// �Ƿ��Ѿ�������һ��״̬��Ŀ��״̬���
		bool	split_read		= false;
		// �Ƿ���Ҫ�ָ�״̬��
		bool	split_require	= false;
		// �����е�Ŀ��״̬���
		size_t	split_target	= 0;

		// �����µ�״̬��, �ɵ�״̬���Ѿ�ͨ�� set ���б���
		list->at(i) = new array_state_t;

		// ����״̬���е�״̬
		for(size_t j = 0; j < set->size(); j ++) {
			// ��ǰ״̬
			state_t *	state	= set->at(j);

			// ��ǰ״̬������ַ���ת������Ŀ��״̬����
			line_t *	line	= state->out[byte];

			// Ŀ��״̬����״̬�����, 0 ��ʾ�������ַ�
			size_t		target	= 0;

			// �жϵ�ǰ״̬�Ƿ���յ�ǰ�ַ�
			if(line != NULL) {
				// Ŀ��״̬
				state_t * to = line->to;

				// ����״̬������
				for(size_t k = 0; k < list->size(); k ++) {
					// ��ѯĿ��״̬�Ƿ��ڵ�ǰ״̬����
					if(find(list->at(k)->begin(), list->at(k)->end(), to) == list->at(k)->end()) {
						continue;
					}

					// ����״̬�����
					target = k + 1;

					break;
				}

				// ����Ҳ���, �������ڵ�ǰ״̬��
				if(target == 0) {
					target = i + 1;
				}
			}

			// ������ǵ�ǰ״̬���е�һ��״̬
			if(split_read == false) {
				// ���ñ���״̬
				split_read		= true;
				// ֱ�ӱ���Ŀ��״̬�����
				split_target	= target;

				// ����ǰ״̬���뵽�µ�״̬��
				list->at(i)->push_back(state);

				// ��������ѭ��, ����������һ��״̬
				continue;
			}

			// ��������״̬��Ӧ�õ�Ŀ��״̬����ͬ, ֱ�ӱ��浽��״̬��
			if(split_target == target) {
				list->at(i)->push_back(state);

				continue;
			}

			// ���浽��Ҫ�ָ��ȥ��Ŀ��״̬������
			split_set[target].push_back(state);

			// ������Ҫ�и�
			split_require	= true;
		}

		// ���û����Ҫ�ָ��ȥ��״̬, ѭ������
		if(split_require == false) {
			goto done;
		}

		// �����ָ��ȥ��״̬
		for(size_t k = 0; k <= count_matrix; k ++) {
			// ���û���κ�״̬�������״̬��, ����
			if(split_set[k].size() == 0) {
				continue;
			}

			// ����ÿһ�����������һ���µ�״̬��
			array_state_t * state_set = new array_state_t (split_set[k]);

			// ����̬���鸴�Ƶ�ǰ״̬��״̬
			end->push_back(end->at(i));
			// ��״̬����������µ�״̬��
			list->push_back(state_set);
		}

done:
		// �ͷžɵ�״̬��
		delete		set;
		// �ͷŷָ�״̬������
		delete []	split_set;
	}
}

