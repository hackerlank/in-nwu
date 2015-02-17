/**
	$ Lexical   (C) 2005-2013 MF
	$ class.h
*/


#pragma warning (disable: 4996)



#ifndef _CLASS_H_
#define _CLASS_H_



// ������
#include <iostream>

// ������
#include <vector>
#include <stack>
#include <queue>



// for std
using namespace std;




/* ״̬�ڵ� */
struct state_t;
/* ���߽ڵ� */
struct line_t;


/* ״̬�ڵ����� */
typedef vector <struct state_t *>	array_state_t;
/* ���߽ڵ����� */
typedef vector <struct line_t *>	array_line_t;

/* �������� */
typedef vector <bool>	array_bool_t;



/* ״̬�ڵ� */
typedef struct state_t {
	array_line_t	in;				// �Ըýڵ���Ϊ�յ�����ߵ�����
	array_line_t	out;			// �Ըýڵ���Ϊ�������ߵ�����
} state_t;


/* ���߽ڵ� */
typedef struct line_t {
	char	byte;					// ת���ֽ�, ���Ϊ 0 ��ʾ �� ��

	state_t *	from;				// ���ڵ�
	state_t *	to;					// �յ�ڵ�
} line_t;



/* ���ʽ���� */
typedef struct {
	bool	alphabet;				// �Ƿ�Ϊ��ĸ, ��Ϊ�����
	char	byte;					// �����ֽ�
} expression_t;


/* �Զ����ṹ */
typedef struct {
	state_t *	start;				// ��ʼ�ڵ�
	state_t *	end;				// �����ڵ�
} automata_t;



/* ״̬��ת���ṹ */
typedef struct convert_t {
	int		number;					// ״̬���
	bool	accept;					// �Ƿ�Ϊ��̬

	array_state_t *		state;		// ��Ӧ״̬��

	struct convert_t **	to;			// ת����������
} convert_t;




/* �ʷ��������� */
class lexical {
	public:
		explicit	 lexical	(FILE *stream);
					~lexical	(void);


		// ��������ʽ������ת��, �����Ƿ�ɹ�
		bool	read		(void);
		// �������, ��ת���Ľ��
		void	output		(void);
		// ����ƥ��ģʽ, �ȴ��û������ַ�������֪ƥ����
		void	match		(void);



	private:
		FILE *	in;										// ������

		vector	<char>				alphabet;			// ��ĸ��
		vector	<convert_t *>		matrix;				// ״̬��ת������
		vector	<expression_t *>	expression;			// ���ʽ��������

		stack	<char>				stack_byte;			// ����ʽת��׺���ʽ�ַ�����ջ
		stack	<automata_t *>		stack_automata;		// �Զ�������ջ


		// ����ʽת��Ϊ��׺���ʽ
		char *	convert_regular_to_expression	(void);
		// ��׺���ʽת��Ϊ NFA
		char *	convert_expression_to_nfa		(void);
		// NFA ת��Ϊ DFA
		char *	convert_nfa_to_dfa				(void);
		// DFA ת��Ϊ��� DFA
		char *	convert_dfa_to_simpdfa			(void);


		// �����ַ�
		char *	regular_byte			(bool alphabet, char byte);
		char *	regular_alphabet		(char byte);
		// ��������
		char *	regular_bracket_left	(void);
		char *	regular_bracket_right	(void);
		// ���������
		char *	regular_concat			(bool alphabet, char byte);
		char *	regular_or				(void);
		char *	regular_closure			(void);


		// ������ʽ����
		char *	expression_alphabet		(char byte);
		char *	expression_concat		(void);
		char *	expression_or			(void);
		char *	expression_closure		(void);


		// ����״̬��
		void	nfa_clear				(void);
		// ��״̬�������Ƶ�
		void	nfa_derivation			(char byte, array_state_t *from, array_state_t *to);
		// ����״̬�� �� �հ�
		void	nfa_epsilon_closure		(state_t *from, array_state_t *result);
		// ״̬������
		void	nfa_state_sort			(array_state_t *set);
		// ��״̬�Ƿ������״̬��
		bool	nfa_state_exist			(array_state_t *set, state_t *state);
		// ��ȡ״̬����Ӧ��ת���ṹ
		convert_t *	nfa_convert			(array_state_t *set);


		// ���ɽڵ����
		array_state_t *	dfa_state_build		(array_bool_t **state_end);
		// ����ת���ṹ����
		char *	dfa_state_convert		(vector <array_state_t *> *list, array_bool_t *end);
		// ��ʼ��״̬��
		void	dfa_set_initialize		(array_state_t *list, array_bool_t *end, vector <array_state_t *> *set);
		// �ָ�״̬��
		void	dfa_set_split			(size_t byte, vector <array_state_t *> *list, array_bool_t *end);
};



#endif   /* CLASS */