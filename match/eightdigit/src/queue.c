/*
	$ queue.c, 0.1.0115
*/

#include "base.h"


/* ��ʼ������ */
#define SIZE_INIT			1048576			/* 1 MB */
/* ÿ������������ */
#define SIZE_EXTEND			262144			/* 256 KB */


/* ���в��� */
static int		action	= 0;
/* �������� */
static size_t	size	= 0;

/* ���пռ� */
static size_t	*queue	= NULL;
static size_t	*edge	= NULL;

/* ͷβָ�� */
static size_t	*head	= NULL;
static size_t	*tail	= NULL;


/*
	$ queueInit			��ʼ������
*/
void queueInit() {
	if(queue) {
		queueClear();
	}

	/* ���в��� */
	action	= 0;
	/* �������� */
	size	= SIZE_INIT;

	/* ������� */
	queue	= (size_t *) calloc(size, sizeof(size_t));
	/* ���б߽� */
	edge	= queue + size - 1;

	/* ����ʧ�� */
	if(queue == NULL) {
		exit(0);
	}

	/* ���ö�ͷ�Ͷ�β */
	head	= queue;
	tail	= head;
}

/*
	$ queueClear		�ͷŶ���
*/
void queueClear() {
	if(queue == NULL) {
		return;
	}

	action	= 0;
	size	= 0;

	edge	= NULL;
	head	= NULL;
	tail	= NULL;

	free(queue);

	queue	= NULL;
}

/*
	$ queueAdd			��������������
*/
void queueAdd(size_t data) {
	/* �¿ռ� */
	size_t *p;
	/* �¾ɿռ������� */
	int offset = 0, left = 0;

	/* д������ */
	*tail = data;

	/* ������б߽� */
	if(tail == edge) {
		tail = queue;
	} else {
		tail++;
	}

	/* ��ͷ�Ͷ�β�ص� */
	if(tail == head) {
		/* ����ϴβ������������, ˵������ */
		if(action == 1) {
			offset = 1;
		}
	}

	/* �������, ����ռ� */
	if(offset) {
		size += SIZE_EXTEND;

		if((p = (size_t *) realloc(queue, size * sizeof(size_t))) == NULL) {
			exit(0);
		}

		/* ����ƫ���� */
		offset = (int) p - (int) queue;

		/* ����ָ�� */
		head = (size_t *) ((char *) head + offset);
		edge = (size_t *) ((char *) edge + offset) + 1;

		/* ����ռ� */
		left	= (int) tail - (int) queue;
		offset	= (int) (p + size) - (int) edge;

		/* �ƶ����� */
		if(left) {
			if(left <= offset) {
				memmove(edge, p, left);

				/* �µ�βָ�� */
				tail = (size_t *) ((char *) edge + left);
			} else {
				memmove(edge, p, offset);
				memmove(p, (char *) p + offset, left - offset);

				/* �µ�βָ����ǰ�ƶ� */
				tail = (size_t *) ((char *) p + (left - offset));
			}
		}

		/* ���¶���ָ�� */
		queue	= p;
		edge	= queue + size - 1;

		/* ����βָ�� */
		if(tail > edge) {
			tail = queue;
		}
	}

	action = 1;
}

/*
	$ queueFetch		�Ӷ�����ȡ������

	# size_t
*/
size_t queueFetch() {
	/* ȡ������ */
	size_t data = *head;

	/* ����Ϊ�� */
	if(head == tail) {
		return 0;
	}

	/* ������б߽� */
	if(head == edge) {
		head = queue;
	} else {
		head++;
	}

	/* ���ò��� */
	action = 0;

	return data;
}
