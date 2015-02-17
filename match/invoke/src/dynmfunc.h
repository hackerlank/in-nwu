/*
	$ Dynamic DLL Function Loader   (C) 2005-2012 mfboy
	$ dynmfunc.h, 0.1.1117
*/

#ifndef _DYNMFUNC_H_
#define _DYNMFUNC_H_

#if defined(__cplusplus)
	extern "C" {
#endif


/* �������� */
#define FUNCTION(name, ret, call, ...)  ret (call *name)(__VA_ARGS__)

/* ��������ģʽ */
#define FUNCTION_PASSIVE		0x01
/* ��������ģʽ�¼���ʧ��ʱֱ�ӷ��ض����׳��쳣 */
#define FUNCTION_NOT_FATAL		0x02


/* ���غ��� */
int Function(void *p, char *dll, char *function, int flag);


#if defined(__cplusplus)
	}
#endif

#endif   /* _DYNMFUNC_H_ */