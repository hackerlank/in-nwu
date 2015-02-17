/*
	$ ExprCalc Tester   (C) 2014 MF
*/


#include <stdio.h>
#include <Windows.h>



/* exprcalc.dll */
#pragma comment (lib, "exprcalc.lib")


/* ����ӿ� */
__declspec(dllimport)	int	__cdecl	expression_calculate	(const char *input, char *output, char *error);




int main (int argc, char *argv[]) {
	puts ("Expression Calculator Tester");
	puts ("-----------------------------------------\n");
	puts ("> Enter your expression (less than 1 KB) and press <ENTER>");
	puts ("  Press CTRL+C to exit\n\n");

	while(1) {
		unsigned int	i;

		char	input	[1025];				/* ������ʽ������	*/
		char	output	[ 256];				/* ���ʽ������������	*/
		char	error	[ 256];				/* ������󻺳���		*/

		fflush (stdin);

		printf ("> ");

		if(fgets (input, sizeof input, stdin) == NULL) {
			puts ("# invalid input\n");

			continue;
		}

		for(i = 0; i < sizeof input; i ++) {
			if((input [i] == '\r') || (input [i] == '\n')) {
				input [i] = '\0';

				break;
			}
		}

		if(input [i] != 0) {
			puts ("# too long input\n");

			continue;
		}

		if(expression_calculate (input, output, error) == 0) {
			if(strncmp (error, "offset", 6) == 0) {
				unsigned long	i;
				unsigned long	offset	= strtoul (error + 7, NULL, 10) + 2;		/* ����������Ϣ�е�ƫ���� */

				/* ����ո� */
				for(i = 0; i < offset; i ++) {
					putchar (' ');
				}

				/* ����ָ��λ�ú���� '^' */
				puts ("^");
			}

			printf ("# %s", error);
		} else {
			printf ("= %s", output);
		}

		puts ("\n");
	}

	return 0;
}

