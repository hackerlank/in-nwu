/*
	$ Ananke PE Loader  (C) 2005-2012 mfboy
	$ process.h, 0.1.1028
*/

#ifndef _ANK_PROCESS_H_
#define _ANK_PROCESS_H_

#if defined(__cplusplus)
	extern "C" {
#endif


/* ״̬ */
typedef enum { LOAD, RUN, PAUSE, BREAK } status_e;

/* PE �ṹ */
typedef enum { DOS_HEADER, PE_HEADER, OPTIONAL_HEADER, SECTION_TABLE, IMPORT_TABLE } struct_e;

/* ������ */
typedef enum { SYSTEM, PE, DYNAMIC, USER } owner_e;


/* ������Ϣ */
typedef struct {
	char	*file;

	/* ״̬ */
	status_e status;

	/* ������Ϣ */
	DWORD	procId;
	HANDLE	procHandle;

	/* ��� */
	DWORD	entry;

	/* ����ַ��λ */
	DWORD	baseAddr;
	DWORD	baseRVAddr;
	long	baseOffset;
} process_h;

/* �ϵ� */
typedef struct bp_s {
	/* ��ʼ�ͽ�����ַ */
	DWORD	addrStart;
	DWORD	addrEnd;

	/* RVA */
	DWORD	rvaStart;
	DWORD	rvaEnd;

	/* ���� */
	char	*section;

	int index;
	struct bp_s *next;
} bp_s;

/* ���� */
typedef struct section_s {
	/* ��ʼλ�� */
	DWORD	address;
	DWORD	virtualaddr;
	DWORD	offset;

	/* ��С */
	DWORD	size;

	/* ���� */
	char	*name;

	/* ������ */
	owner_e owner;

	int index;
	struct section_s *next;
} section_s;

/* ����� */
typedef struct import_s {
	char	*dll;

	/* ����������� */
	char	*function;
	DWORD	ordinal;

	/* ��ڵ�ַ */
	DWORD	entryTable;
	DWORD	entryReal;

	/* IAT λ�� */
	DWORD	iatAddr;
	DWORD	iatOffset;
	/* ԭʼ IAT ָ�� */
	DWORD	iatPoint;

	int index;
	struct import_s *next;
} import_s;

/* ģ�� */
typedef struct module_s {
	char	*path;

	/* ��ַ�ʹ�С */
	DWORD	address;
	DWORD	size;

	int index;
	struct module_s *next;
} module_s;



/* ���н��� */
void threadProcess(char *arg);


#if defined(__cplusplus)
	}
#endif

#endif   /* _ANK_PROCESS_H_ */