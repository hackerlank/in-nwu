/*
	$ main.c, 0.1.0115
*/

/* @ for: Commctrl.h */
#pragma comment (lib, "ComCtl32.lib")

/* @ ������: ��ϵͳ=Windows */
#pragma comment (linker, "/subsystem:\"windows\"")
/* @ ������: �嵥�ļ� */
#pragma comment (linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")


#include <Windows.h>
#include "base.h"


/* ��ʾ */
#define ALERT(message)		\
	MessageBox(hWin, TEXT(message), TEXT(""), MB_OK | MB_ICONWARNING)
#define NOTICE(message)		\
	MessageBox(hWin, TEXT(message), TEXT(""), MB_OK | MB_ICONINFORMATION)


/* �����ھ�� */
extern HWND hWin;
/* ����ʵ�� */
extern HINSTANCE instance;

/* ��ʼ���� */
static unsigned char number[9];
/* �ƶ��������Ŀ */
static size_t  count	= 0;
static size_t *result	= NULL;


/*
	$ threadCalc		�����߳�

	@ void *dummy
*/
void threadCalc(void *dummy) {
	char	*buffer;
	char	message[128];

	size_t	i, position;
	time_t	start, end;

	unsigned char data[9];

	/* �ͷ��ϴεļ����� */
	if(result) {
		free(result);
	}

	start	= clock();
	result	= calculate(number, &count);
	end		= clock();

	/* ���ɽ� */
	if(result == NULL) {
		if(count == 1) {
			NOTICE("���������ΪĿ�����У�������㡣");
		} else {
			NOTICE("������������в��ɽ⡣");
		}

		/* ��ʾ��ť������ */
		enable(1, 0);

		return;
	}

	sprintf(message, "������ϣ��� %ld ������ʱ %.3lf �롣", count, (double) (end - start) / 1000);
	NOTICE(message);

	/* ���ɽ�� */
	if((buffer = (char *) calloc(count + 1, sizeof(char))) == NULL) {
		exit(0);
	}

	/* ��ʼ��λ�� */
	position = number[0];

	/* �����ƶ����� */
	for(i = 0; i < count; i++) {
		unpack(data, result[i]);

		/* �ж��ƶ������λ�� */
		switch((int) data[0] - (int) position) {
			case -3: buffer[i] = 'U'; break;
			case -1: buffer[i] = 'L'; break;
			case  1: buffer[i] = 'R'; break;
			case  3: buffer[i] = 'D'; break;
		}

		position = data[0];
	}

	step(buffer);
	free(buffer);

	/* ��ʾ��ť���� */
	enable(1, 1);
}

/*
	$ threadPlay		��ʾ�߳�

	@ void *dummy
*/
void threadPlay(void *dummy) {
	size_t i;
	unsigned char data[9];

	/* �ָ���ʼ���� */
	show(-1, number);

	/* ���ý��� */
	focus(1);

	/* ��̬��ʾ */
	for(i = 0; i < count; i++) {
		Sleep(1000);

		unpack(data, result[i]);
		show((int) i, data);
	}

	/* �ͷŽ��� */
	Sleep(1000);
	focus(0);

	/* �ָ���ť�ͽ��� */
	enable(1, 1);
}


/*
	$ actCalc			�������

	@ char *buffer		��������������ַ���������
	@ size_t length		�ַ�������
*/
void actCalc(char *buffer, size_t length) {
	size_t i, j;
	unsigned char input[9] = {0};

	if(buffer == NULL) {
		ALERT("���������ĳ�ʼ�������У�"); return;
	}
	if(length < 9) {
		ALERT("�������������Ӧ���� 8 �����ֺͿո������������ 9 ���ַ���"); return;
	}

	for(i = 0, j = 1; i < 9; i++) {
		switch(buffer[i]) {
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				if(input[j]) {
					ALERT("���������������Ч�������ظ������֣�"); return;
				} else {
					input[j++] = buffer[i] - '1' + 1;
				}
				break;

			default:
				if(input[0]) {
					ALERT("���������������Ч�������ظ��Ŀո�"); return;
				} else {
					input[0] = i + 1;
				}
				break;
		}
	}

	/* �ж��Ƿ�����ո� */
	if(input[0] == 0) {
		ALERT("���������������Ч��û�а����ո�"); return;
	}

	/* �ж������Ƿ��������ظ� */
	for(i = 1; i < 9; i++) {
		if(input[i] == 0) {
			ALERT("���������������Ч�����ֲ�������"); return;
		}

		for(j = i + 1; j < 9; j++) {
			if(input[i] == input[j]) {
				ALERT("���������������Ч���������ظ���"); return;
			}
		}
	}

	memcpy(number, input, sizeof input);

	/* ���ð�ť */
	enable(0, 0);
	/* ��ʾ���� */
	show(-1, number);
	/* ��ս�� */
	step("");

	/* ���������߳� */
	_beginthread(threadCalc, 0, NULL);
}

/*
	$ actPlay			��ʾ����
*/
void actPlay() {
	if((count == 0) || (result == NULL)) {
		return;
	}

	/* ���ð�ť */
	enable(0, 0);

	/* ������ʾ�߳� */
	_beginthread(threadPlay, 0, NULL);
}


/*
	$ WinMain

	@ HINSTANCE hInstance
	@ HINSTANCE hPrevInstance
	@ LPSTR lpCmdLine
	@ int nCmdShow

	# int
*/
int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	instance = hInstance;

	gui();

	return 0;
}
