/*
	$ ExprCalc   (C) 2014 MF
	$ main.c   #1403
*/


#include "header/base.h"
#include "header/bridge.h"



/* ���ʽ����ӿ� */
__declspec(dllexport) int __cdecl expression_calculate (const char *input, char *output, char *error) {
	/* ִ��״̬ */
	int	status	= 0;

	/* �� */
	calc_open (input, output, error);

	/* ִ�� */
	status	= calc_execute ( );

	/* �ر� */
	calc_close ( );

	return status;
}




/* DLL ��� */
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	switch(fdwReason) {
		case DLL_PROCESS_ATTACH:
			/* ���� DLL ʱ������������� */
			srand ((unsigned int) time (NULL));

			break;
	}

	return TRUE;
}

