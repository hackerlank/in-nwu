/*
	$ dispatch.c, 0.2.1221
*/

#include "base.h"


/* ������ֹͣ���̶�̬�߳� */
#define ANIMATE_START(item, c)								\
	ResetEvent(ANI_STOP);									\
	ResetEvent(ANI_EXIT);									\
	THRD_RUN(threadAnimate, index | (c << 31));				\
	num->start##item = clock()
#define ANIMATE_STOP(item)									\
	num->end##item = clock();								\
	SetEvent(ANI_STOP);										\
	WaitForSingleObject(ANI_EXIT, INFINITE)


/* �Ƿ����������� */
static int		RUN		= 0;
/* ʹ�� SSE */
static int		SSE		= 0;

/* �����߳� */
static HANDLE	THRD	= NULL;

/* ��̬�����ź� */
static HANDLE	ANI_STOP	= NULL;
static HANDLE	ANI_EXIT	= NULL;



/* ��̬�����߳� */
static THRD_NEW(threadAnimate) {
	/* �ȴ���Ϣ */
	DWORD	signal;

	/* ��� */
	size_t	i = 0;
	/* ������Ϣ */
	char	*process[] = {".", "..", "..."};

	while(1) {
		listSet (
			/* �� */		arg & 0x7FFFFFFF,
			/* �� */		(arg >> 31) + 5,
			"%s",
			process[i]
		);

		/* ѡ����һ����̬ */
		i = (i + 1) % (sizeof process / sizeof(char *));

		/* �ȴ��˳� */
		signal = WaitForSingleObject(ANI_STOP, 1000);

		if(signal == WAIT_OBJECT_0) {
			break;
		} else if(signal != WAIT_TIMEOUT) {
			DIE("�����ͬ���ź���Ϣ");
		}
	}

	SetEvent(ANI_EXIT);
}


/* ��������߳� */
static THRD_NEW(threadDispatch) {
	number_s *num = NUM->next;

	/* ���㺯�� */
	size_t (*func)(bigint_t, int_t, int_t) = NULL;

	double	notation;
	size_t	temp, index = 0, status = 0;

	/* ϵ����������������λ */
	int_t	ratio, exp, high;

	/* ѡ����㺯�� */
	if(SSE) {
		func = powWithSSE;
	} else {
		func = powWithALU;
	}

	/* �ж�״̬ */
	while(num) {
		if(num->doneCalc) {
			status |= 1;
		} else {
			status |= 2;
		}

		num = num->next;
	}

	/* ����״̬��ʾ��Ϣ */
	switch(status) {
		/* û������ */
		case 0: goto done;

		/* ������������ɼ��� */
		case 1:
			if(!CONFIRM("�Ƿ����¿�ʼ���㣿")) {
				goto done;
			}

			status = 1;
			break;

		/* ����������δ��ɼ��� */
		case 2:
			status = 1; break;

		/* ������������� */
		case 3:
			if(CONFIRM("�в��������Ѿ�������ɣ��Ƿ�ֻ����ʣ������ݣ����ѡ��������¼����������ݡ�")) {
				status = 0;
			} else {
				status = 1;
			}
			break;
	}

	num = NUM->next;

	/* ��ʼ������ */
	while(num) {
		if((num->doneCalc == 0) || (status == 1)) {
			DELA(num->data);

			num->doneCalc	= 0;
			num->doneOutput	= 0;

			/* �����ʾ */
			listSet(index, 3, "?");
			listSet(index, 4, "?");
			listSet(index, 5, "-");
			listSet(index, 6, "-");
			listSet(index, 7, "-");
		}

		index++;
		num = num->next;
	}

	index	= 0;
	num		= NUM->next;

	while(num) {
		if(num->doneCalc) {
			index++;
			num = num->next;

			continue;
		}

		/* �����ڴ� */
		BIGINT(num->data, num->exponent) {
			listSet(index, 2, "");
			listSet(index, 3, "");
			listSet(index, 4, "�ڴ治��");
			listSet(index, 5, "");
			listSet(index, 6, "");
			listSet(index, 7, "");

			num->doneCalc	= 1;
			num->doneOutput	= 1;

			index++;
			num = num->next;

			continue;
		}

		/* ϵ�� */
		if(num->remainder) {
			ratio = num->remainder;
		} else {
			ratio = 1;
		}

		/* ָ�� */
		exp = num->exponent;

		/* ת��Ϊ 3 ^ 16 Ϊ�׵ĳ˷� */
		if(temp = exp % 16) {
			while(temp--) {
				ratio *= 3;
			}
		}

		/* 3 ^ 16n = (3 ^ 16) ^ n */
		exp >>= 4;

		/* ����˻� */
		ANIMATE_START(Mul, 0);
		num->countHex = func(num->data, ratio, exp);
		ANIMATE_STOP(Mul);

		/* ���ʱ�� */
		listSet(index, 5, "%.3lfs", (double) (num->endMul - num->startMul) / 1000.);

		/* ת��Ϊ 10 ^ 10 ���� */
		ANIMATE_START(Conv, 1);
		num->countDec = convert(num->data, num->countHex);
		ANIMATE_STOP(Conv);

		/* ���� */
		num->lenAll		= 9 * num->countDec;
		num->lenHigh	= 9;

		/* ���λ */
		high = *(num->data + (num->countDec - 1) * OFFSET + 1);
		/* ��ʼ�������� */
		temp = NOTATION / 10;

		/* ����ʮ����λ�� */
		while(high < temp) {
			temp = temp / 10;

			num->lenAll--;
			num->lenHigh--;
		}

		/* ����˻� */
		if(num->countHex == 1) {
			listSet(index, 2, "%lu\n\n", high);
		} else {
			/* ���λת�� */
			temp		= high;
			notation	= (double) NOTATION;

			/* �������ϵ�� */
			while(notation < high) {
				notation	/= 10.;
			}

			/* �����λ�չ� 10^10 */
			while(temp < NOTATION) {
				notation	/= 10.;
				temp		*= 10;
			}

			temp += (int_t) ((double) *(num->data + (num->countDec - 2) * OFFSET + 1) / notation);

			listSet(index, 2, "%1.4lf * 10 ^ %lu\n\n", (double) temp / NOTATION, num->lenAll - 1);
		}

		/* ���������Ϣ */
		listSet(index, 3, "%lu", num->lenAll);
		listSet(index, 4, "%lu Byte", sizeof(int_t) * 2 * num->countDec);
		listSet(index, 6, "%.3lfs", (double) (num->endConv - num->startConv) / 1000.);
		listSet(index, 7, "%.3lfs", (double) ((num->endMul - num->startMul) + (num->endConv - num->startConv)) / 1000.);

		num->doneCalc = 1;

		index++;
		num = num->next;
	}


done:		/* ��ɼ��� */
	num		= NUM->next;

	RUN		= 0;
	THRD	= NULL;

	/* ��ť״̬ */
	openEnable(1);
	saveEnable(0);
	sseEnable(1);
	calcText("��ʼ����");

	/* ����Ƿ��м������ */
	while(num) {
		if(num->doneCalc) {
			saveEnable(1); break;
		}

		num = num->next;
	}
}


/* ��ʼ���� */
static void start() {
	if(NUM->next == NULL) {
		return;
	}

	/* �ж� SSE ���� */
	if((SSE = sseChecked()) == 0) {
		if(cpuSSE()) {
			if(!CONFIRM("ʹ�� SSE2 ָ��ɶԼ�������Ż�������ӿ������ٶȡ��ر� SSE2 �Ż��󣬼�����ܻ�ķѽϳ�ʱ�䣬�Ƿ������")) {
				return;
			}
		}
	}

	openEnable(0);
	saveEnable(0);
	sseEnable(0);
	calcText("ֹͣ����");

	RUN = 1;

	/* �����ź��� */
	if(ANI_STOP == NULL) {
		ANI_STOP = CreateEvent(NULL, TRUE, FALSE, NULL);
		ANI_EXIT = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	/* ���������߳� */
	THRD = (HANDLE) THRD_RUN(threadDispatch, 0);
}

/* ֹͣ���� */
static void stop() {
	int			done = 0, index = 0;
	number_s	*num = NUM->next;

	if(!CONFIRM("���Ҫֹͣ���㣿")) {
		return;
	}
	if(RUN == 0) {
		return;
	}

	calcEnable(0);

	/* ��ֹ�����߳� */
	TerminateThread(THRD, 0);

	/* ������ֹ�ź� */
	SetEvent(ANI_STOP);
	WaitForSingleObject(ANI_EXIT, 1200);

	/* ����Ƿ����Ѿ���ɵ���� */
	while(num) {
		if(num->doneCalc) {
			done = 1;
		} else {
			/* �����ʾ */
			listSet(index, 2, "?");
			listSet(index, 3, "?");
			listSet(index, 4, "?");
			listSet(index, 5, "-");
			listSet(index, 6, "-");
			listSet(index, 7, "-");
		}

		index++;
		num = num->next;
	}

	openEnable(1);
	calcEnable(1);
	sseEnable(1);
	saveEnable(done);
	calcText("��ʼ����");

	RUN		= 0;
	THRD	= NULL;
}


/* ���ݴ���, �ַ����Ӻ��� */
void dispatch() {
	if(RUN) {
		stop();
	} else {
		start();
	}
}

/* �رմ���ʱ��Ᵽ��״̬ */
int close() {
	char		*msg = NULL;
	number_s	*num = NUM->next;

	if(RUN) {
		msg = "�������ڽ������ݼ��㣬�Ƿ�ȷ��Ҫ�˳���";
	} else {
		while(num) {
			if((num->doneCalc == 0) || (num->doneOutput == 0)) {
				msg = "��������û�м������û�б��棬�Ƿ�ȷ��Ҫ�˳���"; break;
			}

			num = num->next;
		}
	}

	if(msg) {
		return CONFIRM(msg);
	} else {
		return 1;
	}
}
