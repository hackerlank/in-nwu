/*
	$ ExprCalc   (C) 2014 MF
	$ parser.c   #1403
*/


#include "header/base.h"
#include "header/bridge.h"
#include "header/parser.h"



/* �Ƕ� => ���� */
#define A2R(v)			((v) * M_PI / 180)
/* ���� => �Ƕ� */
#define R2A(v)			((v) * 180 / M_PI)




/* �����쳣���� */
static int __cdecl ec_parser_exception_floating (_FPIEEE_RECORD *record) {
	ec_ex_syntax ("floating error");

	return EXCEPTION_EXECUTE_HANDLER;
}




/* ͳ�ƺ����������� */
static ec_value_t ec_parser_statistic (YYSTYPE *in, ec_value_t *count) {
	unsigned int	i;

	ec_value_t		avg	= 0;
	ec_value_t		sum	= 0;

	/* ������� */
	SL(in, i) {
		avg	+= SV(in, i);
	}

	/* ��ƽ��ֵ */
	avg	= avg / (ec_value_t) i;

	/* ƽ����� */
	SL(in, i) {
		ec_value_t	diff	= SV(in, i) - avg;

		sum	+= diff * diff;
	}

	* count	= (ec_value_t) i;

	return sum;
}




/* ���� */
DD1 (pos)	{	NW(out)	=   N(in_1);						} END_OF (out)
DD1 (neg)	{	NW(out)	= - N(in_1);						} END_OF (out)

/* �Ӽ��˳� */
DD2 (add)	{	NW(out)	= N(in_1) + N(in_2);				} END_OF (out)
DD2 (sub)	{	NW(out)	= N(in_1) - N(in_2);				} END_OF (out)
DD2 (mul)	{	NW(out)	= N(in_1) * N(in_2);				} END_OF (out)
DD2 (div)	{	NW(out)	= N(in_1) / N(in_2);				} END_EX (out, "division by zero")

/* ��ģ�˷� */
DD2 (mod)	{	NW(out)	= fmod	(N(in_1), N(in_2));			} END_EX (out, "modulo by zero")
DD2 (pow)	{	NW(out)	= pow	(N(in_1), N(in_2));			} END_OF (out)

/* ���Ǻ��� */
DD1 (sin)	{	NW(out)	= sin (A2R(N(in_1)));				} END_OF (out)
DD1 (cos)	{	NW(out)	= cos (A2R(N(in_1)));				} END_OF (out)
DD1 (tan)	{	NW(out)	= tan (A2R(N(in_1)));				} END_OF (out)

/* �����Ǻ��� */
DD1 (arcsin)	{	NW(out)	= R2A(asin (N(in_1)));			} END_OF (out)
DD1 (arccos)	{	NW(out)	= R2A(acos (N(in_1)));			} END_OF (out)
DD1 (arctan)	{	NW(out)	= R2A(atan (N(in_1)));			} END_OF (out)

/* ˫������ */
DD1 (sinh)	{	NW(out)	= sinh (N(in_1));					} END_OF (out)
DD1 (cosh)	{	NW(out)	= cosh (N(in_1));					} END_OF (out)
DD1 (tanh)	{	NW(out)	= tanh (N(in_1));					} END_OF (out)

/* ���� */
DD2 (log)	{	NW(out)	= log (N(in_1)) / log (N(in_2));	} END_OF (out)
DD1 (log10)	{	NW(out)	= log10 (N(in_1));					} END_OF (out)
DD1 (ln)	{	NW(out)	= log   (N(in_1));					} END_OF (out)

/* ָ�� */
DD1 (exp)	{	NW(out)	= exp (N(in_1));					} END_OF (out)

/* �׳� */
DD1 (fact)	{
	int	i;
	int	from	= (int) N(in_1);

	if(from < 0) {
		ec_ex_syntax ("factorials for negative integers");

		RETURN_FAILURE;
	}

	NW(out)	= 1;

	for(i = 1; i <= from; i ++) {
		NW(out)	*= (ec_value_t) i;
	}
} END_OF (out)

/* ���� */
DD1 (sqrt) {
	NW(out)	= sqrt (N(in_1));
} END_OF (out)

/* �����η� */
DD1 (cuberoot) {
	if(N(in_1) < 0) {
		NW(out)	= - pow (- N(in_1), 1. / 3.);
	} else {
		NW(out)	= pow (N(in_1), 1. / 3.);
	}
} END_OF (out)

/* ������η� */
DD2 (yroot) {
	if(N(in_1) < 0) {
		ec_ex_syntax ("unable to root a negative number");

		RETURN_FAILURE;
	}

	if(N(in_2) == 0) {
		ec_ex_syntax ("extract a root by zero");

		RETURN_FAILURE;
	}

	NW(out)	= pow (N(in_1), 1. / N(in_2));
} END_OF (out)


/* ���ϵ�����ƽ��ֵ */
DD1 (avg) {
	unsigned int	i;
	ec_value_t		sum	= 0;

	SC(in_1);
	SL(in_1, i) {	sum += SV(in_1, i);		}

	NW(out)	= sum / (double) i;
} END_OF (out)


/* ���ϵ�ͳ�� */
DD1 (sum) {
	unsigned int	i;

	NW(out)	= 0;

	SC(in_1);
	SL(in_1, i) {	N(out) += SV(in_1, i);	}
} END_OF (out)


/* ���ϵ��������� */
DD1 (var) {
	ec_value_t	sum;
	ec_value_t	count;

	SC(in_1);

	sum	= ec_parser_statistic (in_1, & count);

	if(count < 2) {
		ec_ex_syntax ("sample variance division by zero");

		RETURN_FAILURE;
	}

	NW(out)	= sum / (count - 1);
} END_OF (out)


/* ���ϵ��������巽�� */
DD1 (varp) {
	ec_value_t	sum;
	ec_value_t count;

	SC(in_1);

	sum	= ec_parser_statistic (in_1, & count);

	NW(out)	= sum / (ec_value_t) count;
} END_OF (out)


DD1 (stdev) {
	ec_value_t	sum;
	ec_value_t	count;

	SC(in_1);

	sum	= ec_parser_statistic (in_1, & count);

	if(count < 2) {
		ec_ex_syntax ("standard deviation division by zero");

		RETURN_FAILURE;
	}

	NW(out)	= sqrt (sum / (count - 1));
} END_OF (out)


DD1 (stdevp) {
	ec_value_t	sum;
	ec_value_t count;

	SC(in_1);

	sum	= ec_parser_statistic (in_1, & count);

	NW(out)	= sqrt (sum / (ec_value_t) count);
} END_OF (out)


/* ���ϵ����ֵ */
DD1 (max) {
	unsigned int	i;

	NW(out)	= SV(in_1, 0);

	SC(in_1);

	SL(in_1, i) {
		if(N(out) < SV(in_1, i)) {
			N(out)	= SV(in_1, i);
		}
	}
} END_OF (out)


/* ���ϵ���Сֵ */
DD1 (min) {
	unsigned int	i;

	NW(out)	= SV(in_1, 0);

	SC(in_1);

	SL(in_1, i) {
		if(N(out) > SV(in_1, i)) {
			N(out)	= SV(in_1, i);
		}
	}
} END_OF (out)


/* ����ȡ�� */
DD1 (floor)	{	NW(out)	= floor (N(in_1));		} END_OF (out)
/* ����ȡ�� */
DD1 (ceil)	{	NW(out)	= ceil  (N(in_1));		} END_OF (out)

/* �������� */
DD1 (round)	{
	if(N(in_1) >= 0) {
		NW(out)	= (ec_value_t) ((int) (N(in_1) + 0.5));
	} else {
		NW(out)	= (ec_value_t) ((int) (N(in_1) - 0.5));
	}
} END_OF (out)

/* ��������� */
DD1 (rand)	{
	NW(out)	= ((ec_value_t) rand ( )) / ((ec_value_t) RAND_MAX);
} END

/* �Ƕ�ת���� */
DD1 (a2r) {		NW(out)	= A2R(N(in_1));			} END_OF (out)
/* ����ת�Ƕ� */
DD1 (r2a) {		NW(out)	= R2A(N(in_1));			} END_OF (out)




/* ������� */
DI (end) {
	sprintf (buffer_output, "%lf", N(op_1));
	
	RETURN_SUCCEED;
}



/* �������� */
DI (setbuild) {
	/* ���伯�Ͽռ� */
	ec_value_t *	set	= (ec_value_t *) ec_mg_new (sizeof (ec_value_t) * EC_SET_BLOCK);

	/* �����һ��Ԫ�� */
	set [0]	= op_2->v.number;

	op_1->is_set		= 1;
	op_1->v.set.count	= 1;
	op_1->v.set.pointer	= set;
	op_1->v.set.unused	= EC_SET_BLOCK - 1;

	RETURN_SUCCEED;
}


/* д�뼯�� */
DI (setwrite) {
	/* �ж����� */
	if(op_2->v.set.unused == 0) {
		/* �ط��伯�� */
		op_2->v.set.pointer	= (ec_value_t *) ec_mg_renew (op_2->v.set.pointer, sizeof (ec_value_t) * (op_2->v.set.count + EC_SET_BLOCK));

		/* ���¿��ÿռ� */
		op_2->v.set.unused	= EC_SET_BLOCK;
	}

	/* ����Ԫ�� */
	op_2->v.set.pointer [op_2->v.set.count]	= op_3->v.number;

	op_1->is_set		= 1;
	op_1->v.set.count	= op_2->v.set.count  + 1;
	op_1->v.set.unused	= op_2->v.set.unused - 1;
	op_1->v.set.pointer	= op_2->v.set.pointer;

	RETURN_SUCCEED;
}


/* ���Ϻϲ� */
DI (setmerge) {
	SC(op_2);

	if(op_3 != NULL) {
		unsigned int	i;

		SC(op_3);

		/* �ж����� */
		if(op_2->v.set.unused < op_3->v.set.count) {
			/* ����Ԫ�ظ��� */
			unsigned int	required	= op_2->v.set.unused + op_3->v.set.count;

			/* �ط��伯�� */
			op_2->v.set.pointer	= (ec_value_t *) ec_mg_renew (op_2->v.set.pointer, sizeof (ec_value_t) * (op_2->v.set.count + required));

			/* ���¿��ÿռ� */
			op_2->v.set.unused	= required;
		}

		/* �ϲ����� */
		for(i = 0; i < op_3->v.set.count; i ++) {
			op_2->v.set.pointer [op_2->v.set.count + i]	= op_3->v.set.pointer [i];
		}
		
		op_2->v.set.count	+= i;
		op_2->v.set.unused	-= i;
	}

	op_1->is_set		= 1;
	op_1->v.set.count	= op_2->v.set.count;
	op_1->v.set.unused	= op_2->v.set.unused;
	op_1->v.set.pointer	= op_2->v.set.pointer;

	RETURN_SUCCEED;
}

