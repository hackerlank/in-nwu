/*
	$ ExprCalc   (C) 2014 MF
	$ header/parser.h   #1403
*/


#ifndef _EC_PARSER_H_
#define _EC_PARSER_H_

#ifdef __cplusplus
	extern "C" {
#endif



/* yylex() ���� */
#define YYLEX_PARAM				& yylval

/* �ڴ洦�� */
#define YYMALLOC				ec_mm_new
#define YYFREE					ec_mm_delete

/* ������ */
#define YY_(v)
#define yyerror(v)				ec_ex_syntax ("syntax error")




/* �쳣��ʼ */
#define E_TRY					__try

/* �쳣�������� */
#define E_CATCH					__except (_fpieee_flt (								\
									GetExceptionCode ( ),							\
									GetExceptionInformation ( ),					\
									ec_parser_exception_floating					\
								)) {												\
									RETURN_FAILURE;									\
								}




/* �﷨������������ */
#define F(group, name)			ec_parser_##group##_##name


/* �﷨������������ */
#define DI(name)				int F (internal, name) (YYSTYPE *op_1, YYSTYPE *op_2, YYSTYPE *op_3)
#define D1(name)				int F (handler,  name) (YYSTYPE *out,  YYSTYPE *in_1)
#define D2(name)				int F (handler,  name) (YYSTYPE *out,  YYSTYPE *in_1, YYSTYPE *in_2)

/* �﷨������������ */
#define DD1(name)				D1 (name) {		E_TRY
#define DD2(name)				D2 (name) {		E_TRY


/* �﷨������������ */
#define END							E_CATCH											\
									RETURN_SUCCEED;									\
								}
#define END_EX(var, msg)			E_CATCH											\
									if(_finite (var->v.number) == 0) {				\
										ec_ex_syntax (msg);							\
										RETURN_FAILURE;								\
									}												\
									RETURN_SUCCEED;									\
								}
#define END_OF(var)				END_EX (var, "floating overflow")


/* �﷨�������������� */
#define RETURN_SUCCEED			return 0
#define RETURN_FAILURE			return 1


/* �﷨������������ */
#define CALL(name, ...)			if(F (internal, name) (__VA_ARGS__) != 0) { YYERROR; }
#define EXEC(name, ...)			if(F (handler,  name) (__VA_ARGS__) != 0) { YYERROR; }




/* ��ֵ���� */
#define N(var)					var->v.number
#define NW(var)					var->is_set	= 0;	N(var)


/* ������ֵ */
#define SV(var, i)				(var->v.set.pointer [i])

/* ����ѭ�� */
#define SL(var, i)				for(i = 0; i < var->v.set.count; (i) ++)

/* ���ϼ�� */
#define SC(var)					if((var->is_set == 0) || (var->v.set.count == 0)) {		\
									ec_ex_syntax ("a set cannot be empty");				\
									RETURN_FAILURE;										\
								}




/* �﷨�������� */
D1 (pos);			D1 (neg);
D2 (add);			D2 (sub);			D2 (mul);			D2 (div);			D2 (mod);			D2 (pow);
D1 (sin);			D1 (cos);			D1 (tan);
D1 (arcsin);		D1 (arccos);		D1 (arctan);
D1 (sinh);			D1 (cosh);			D1 (tanh);
D2 (log);			D1 (log10);			D1 (ln);
D1 (exp);			D1 (fact);
D1 (sqrt);			D1 (cuberoot);		D2 (yroot);
D1 (avg);			D1 (sum);			D1 (var);			D1 (varp);			D1 (stdev);			D1 (stdevp);
D1 (max);			D1 (min);			D1 (floor);			D1 (ceil);			D1 (round);			D1 (rand);			D1 (a2r);			D1 (r2a);

/* �ڲ����� */
DI (end);			DI (setbuild);		DI (setwrite);		DI (setmerge);




#ifdef __cplusplus
	}
#endif

#endif   /* PARSER */