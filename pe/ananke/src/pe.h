/*
	$ Ananke PE Loader  (C) 2005-2012 mfboy
	$ pe.h, 0.1.1027
*/

#ifndef _ANK_PE_H_
#define _ANK_PE_H_

#if defined(__cplusplus)
	extern "C" {
#endif


/* ���ļ��򻺳����ж�ȡ�ַ���*/
char * readFromFile(FILE *fp, BOOL incase);
char * readFromBuffer(char *buffer, size_t bufsize);


/* �ڴ��ַ => RVA */
DWORD addr2rva(DWORD addr);
/* RVA => ����ƫ���� */
DWORD rva2offset(DWORD rva);
/* ���ڴ��ַ������� */
char * addr2section(DWORD addr);


/* ����������� */
void peFunction(FILE *fp, IMAGE_IMPORT_DESCRIPTOR *imp);


/* ���α��ʼ�� */
void peSection();
/* ������� */
void peImport(BOOL update);


/* ������ڼ��� */
void peEntry();


/* ��Ӷϵ� */
void peBreakAddress(DWORD addr);
void peBreakRange(DWORD from, DWORD to);


/* �ϵ���ѭ�� */
void loop(DWORD eip);


#if defined(__cplusplus)
	}
#endif

#endif   /* _ANK_PE_H_ */