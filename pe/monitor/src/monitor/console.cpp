/*
	$ Monitor Module   (C) 2005-2014 MF
	$ console.cpp
*/


#include "header/base.h"



/* ����̨��� */
void *	Console::handle		= NULL;




/* ��������̨���� */
bool Console::open (void) {
	// �������̨
	if(AllocConsole() == FALSE) {
		return false;
	}

	// ��ȡ����̨������
	Console::handle	= (void *) GetStdHandle(STD_OUTPUT_HANDLE);

	return true;
}



/* ������ɫ */
void Console::color (bool red, bool green, bool blue, bool intensity) {
	// ���û�о��, ������ֱ�ӷ���
	if(Console::handle == NULL) {
		return;
	}

	// ����ֵ
	unsigned int attribute	= 0;

	// �ϲ���ɫ
	attribute	|= (red			== true) ? FOREGROUND_RED		: 0;
	attribute	|= (green		== true) ? FOREGROUND_GREEN		: 0;
	attribute	|= (blue		== true) ? FOREGROUND_BLUE		: 0;
	attribute	|= (intensity	== true) ? FOREGROUND_INTENSITY	: 0;

	// ��������
	SetConsoleTextAttribute((HANDLE) Console::handle, (WORD) attribute);
}




/* ��ʽ����� */
void Console::printf (const char *format, ...) {
	// �������λ��
	uintptr_t argument = ((uintptr_t) & format) + sizeof(uintptr_t);

	// ִ�����
	Console::vprintf(format, (void *) argument);
}


/* ��̬������ʽ����� */
void Console::vprintf (const char *format, void *argument) {
	// ���û�о��, ֱ�ӷ���
	if(Console::handle == NULL) {
		return;
	}

	// �����С
	int		length	= _vsnprintf(NULL, 0, format, (va_list) argument);
	// ���������
	char *	buffer	= NULL;

	// �жϳ����Ƿ�Ϊ����
	if(length <= 0) {
		// ����̶���С������
		buffer	= new char [64];

		// �������
		sprintf(buffer, "{FAILED TO EXECUTE vsnprintf()}\n");

		// ���㳤��
		length	= strlen(buffer);
	} else {
		// ���仺����
		buffer = new char [length + 4];

		// �����ַ���
		vsnprintf(buffer, length, format, (va_list) argument);
	}

	// �������
	WriteConsoleA((HANDLE) Console::handle, buffer, (DWORD) length, (LPDWORD) & length, NULL);

	// �ͷ��ַ���
	delete [] buffer;
}




/* ִ����� */
void Console::output (void *p) {
	// ��ʽ���ı�
	const char *	format		= *((const char **) p);
	// ������
	va_list			argument	= (va_list) ((uintptr_t) p + sizeof(uintptr_t));

	// �����ʽ��һ���ַ�Ϊ #, ����ʽ���
	if(*format == '#') {
		// ���ҽ������ַ�
		const char * end	= strchr(format + 1, '#');

		// ��ȡʱ��
		time_t	timestamp	= time(NULL);
		// ת��Ϊ����ʱ��
		tm *	timeinfo	= localtime(& timestamp);

		// ʱ�仺����
		char	timebuffer	[128];

		// ����ʱ���ַ���
		strftime(timebuffer, sizeof timebuffer, "[%b %d %H:%M:%S]", timeinfo);

		if(end == NULL) {
			// ���û���ҵ���������, ֻ���ʱ��
			Console::printf("%s ", timebuffer);

			// ������һ���ַ� #
			format ++;
		} else {
			// ǰ׺����
			int		length	= end - format - 1;
			// ǰ׺�ַ���
			char *	prefix	= new char [length + 1];

			// ����ǰ׺
			memcpy(prefix, format + 1, (size_t) length);
			// �����ַ���
			memset(prefix + length, 0, sizeof(char));

			// �����Ϣ
			Console::printf("%s - %s ", prefix, timebuffer);

			// �ͷ�ǰ׺�ַ���
			delete [] prefix;

			// ����ǰ׺
			format = format + length + 2;
		}
	}

	Console::vprintf(format, argument);
}




/* �����ɫ��ʾ */
void Console::notice (const char *format, ...) {
	// ��ɫ
	Console::color	(true,	true,	true,	false);
	// �������
	Console::output	(& format);
}


/* �����ɫ���� */
void Console::warning (const char *format, ...) {
	// ��ɫ
	Console::color	(false,	true,	false,	false);
	// �������
	Console::output	(& format);
}


/* �����ɫ���� */
void Console::error (const char *format, ...) {
	// ��ɫ
	Console::color	(true,	false,	false,	true);
	// �������
	Console::output	(& format);
}

