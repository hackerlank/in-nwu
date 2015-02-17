/*
	$ P2MP3  (C) 2005-2012 mfboy
	$ win32.h, 0.1.1005
*/

#ifndef _WIN32_H_
#define _WIN32_H_

/*
	$ explorer				����Դ������, ��ָ��ĳ���ļ�

	@ char *filename
*/
void explorer(char *filename);

/*
	$ fileext				��ȡ�ļ���չ��

	@ char *filename

	# char *				���û����չ��, ���� NULL
*/
char *fileext(char *filename);

/*
	$ select_file_open		ѡ��һ���ļ��Թ���

	@ const char *filter	�ļ�ɸѡ��, ����Ϊ NULL ʹ��Ĭ��ɸѡ��
	@ const char *title		���ڱ���

	# char *				�����ļ������ļ���, ȡ������ NULL
*/
char *select_file_open(const char *filter, const char *title);

/*
	$ select_file_save		ѡ��һ��·���Թ�����

	@ char *filename		Ĭ���ļ���
	@ const char *title		���ڱ���

	# char *				ѡ����·������·��, ȡ������ NULL
*/
char *select_file_save(char *filename, const char *title);


/*
	$ inputbox				�������봰��

	@ const char *title
	@ const char *message
	@ const char *deftext	�������Ĭ�ϵ��ı�

	# char *				����ȷ������������ı�, ȡ������ NULL
*/
char *inputbox(const char *title, char *message, char *deftext);


#endif
