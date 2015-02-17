/*
	$ calc.c, 0.1.0115
*/

#include "base.h"


/* Ŀ������	[(9), 1, 2, 3, 4, 5, 6, 7, 8] */
#define RESULT		0x09053977


/*
	$ soluble				�ж�����ɽ�

	# unsigned char *number

	@ size_t				����ɽ�, ���ط�װ�õ�����, ���򷵻� 0
*/
static size_t soluble(unsigned char *number) {
	int i, j;
	int inverse = 0;

	/* ���������� */
	for(i = 1; i < 9; i++) {
		for(j = i + 1; j < 9; j++) {
			if(number[i] > number[j]) {
				inverse++;
			}
		}
	}

	/* �����в��ɽ� */
	if(inverse % 2) {
		return 0;
	}

	/* ��װ���� */
	return pack(number);
}

/*
	$ extend				����չ��

	@ size_t data
*/
static void extend(size_t data) {
	size_t next;

	/* ���� */
	if(next = moveUp(data)) {
		if(flagSet(next, 1) == 0) {
			queueAdd(next);
		}
	}

	/* ���� */
	if(next = moveRight(data)) {
		if(flagSet(next, 2) == 0) {
			queueAdd(next);
		}
	}

	/* ���� */
	if(next = moveDown(data)) {
		if(flagSet(next, 3) == 0) {
			queueAdd(next);
		}
	}

	/* ���� */
	if(next = moveLeft(data)) {
		if(flagSet(next, 4) == 0) {
			queueAdd(next);
		}
	}
}

/*
	$ steps					���ɲ���

	@ size_t input
	@ size_t *count

	# size_t *
*/
static size_t * steps(size_t input, size_t *count) {
	size_t i, j;
	size_t data, direction;

	/* ��� */
	size_t  size	= 0;
	size_t *result	= NULL;

	/* β�ڵ� */
	data = RESULT;

	/* ������ */
	*count = 0;

	/* ѭ������ */
	while(direction = flagGet(data)) {
		if(direction == 7) {
			break;
		}

		if(*count == size) {
			size += 32;

			if((result = (size_t *) realloc(result, size * sizeof(size_t))) == NULL) {
				exit(0);
			}
		}

		/* ������ */
		result[*count] = data;

		/* ���Ӳ����� */
		(*count)++;

		/* ���㸸�ڵ� */
		switch(direction) {
			case 1:		data = moveDown(data);	break;
			case 2:		data = moveLeft(data);	break;
			case 3:		data = moveUp(data);	break;
			case 4:		data = moveRight(data);	break;
			default:	data = 0; break;
		}
	}

	/* �жϻ��ݽ���Բ��� */
	if(data != input) {
		free(result);

		return NULL;
	}

	/* ת������ */
	for(i = 0, size = *count / 2; i < size; i++) {
		j = *count - i - 1;

		data = result[j];

		result[j] = result[i];
		result[i] = data;
	}

	return result;
}


/*
	$ calcuate				���м���

	@ unsigned char *number
	@ size_t *step			�ƶ�������

	# size_t *				�ɽⷵ���ƶ�����, ���򷵻� NULL
*/
size_t * calculate(unsigned char *number, size_t *step) {
	/* ��װ������ */
	size_t input, data;

	/* ������ */
	*step = 0;

	/* ���ⲻ�ɽ� */
	if((input = soluble(number)) == 0) {
		return NULL;
	}

	/* ������� */
	if(input == RESULT) {
		*step = 1;
		return NULL;
	}

	/* ��ʼ�����ÿռ� */
	flagInit();
	queueInit();

	/* ����ʼ״̬ѹ����� */
	flagSet(input, 7);
	queueAdd(input);

	/* ����ѭ�� */
	while(data = queueFetch()) {
		/* �ж��Ƿ�Ϊ��� */
		if(data == RESULT) {
			break;
		}

		/* �����н���չ�� */
		extend(data);
	}

	/* ������� */
	queueClear();

	/* ѭ������ */
	if(data == 0) {
		return NULL;
	}

	return steps(input, step);
}
