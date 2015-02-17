/*
	$ Robot for LinLink   (C) 2005-2013 MF
	$ memory.c
*/

#include "base.h"


int memoryRead(DWORD address, size_t size, void *buffer) {
	size_t read;

	if(ReadProcessMemory(process, (LPCVOID) address, buffer, size, (SIZE_T *) &read)) {
		if(read == size) {
			return read;
		}
	}
	
	return 0;
}


int memoryWrite(DWORD address, size_t size, void *buffer) {
	DWORD	old, temp;
	size_t	written;

	/* ��ȡ�ڴ� */
	if(WriteProcessMemory(process, (LPVOID) address, buffer, size, (SIZE_T *) &written)) {
		return written;
	}

	/* ���Ի�ȡд��Ȩ�� */
	if(VirtualProtectEx(process, (LPVOID) address, size, PAGE_EXECUTE_READWRITE, &old) == FALSE) {
		return 0;
	}

	/* ��Ȩ���ٴ�д�� */
	WriteProcessMemory(process, (LPVOID) address, buffer, size, (SIZE_T *) &written);
	/* �ָ�Ȩ�� */
	VirtualProtectEx(process, (LPVOID) address, size, old, &temp);

	return written;
}

int memoryWriteV(DWORD address, size_t size, ...) {
	char	*buffer;
	int		result;

	size_t	i;
	va_list	arg;

	va_start(arg, size);
	NEW(buffer, char, size);
	
	/* ���ɴ�д�����ݻ����� */
	for(i = 0; i < size; i++) {
		buffer[i] = va_arg(arg, char);
	}

	result = memoryWrite(address, size, buffer);

	DEL(buffer);
	va_end(arg);

	return result;
}


int memorySet(DWORD address, size_t size, int data) {
	char	*buffer;
	int		result;

	NEW(buffer, char, size);
	memset(buffer, data, size);

	result = memoryWrite(address, size, buffer);

	DEL(buffer);

	return result;
}


int memoryEqual(DWORD address, size_t size, void *buffer) {
	char	*data;
	int		result;

	NEW(data, char, size);

	/* ��ȡ���ݲ����бȽ� */
	if(memoryRead(address, size, data)) {
		if((result = memcmp(buffer, data, size)) == 0) {
			result = 1;
		} else {
			result = 0;
		}
	} else {
		result = 0;
	}

	DEL(data);

	return result;
}

int memoryEqualV(DWORD address, size_t size, ...) {
	int		result;
	char	*source, *target;

	size_t	i;
	va_list	arg;

	va_start(arg, size);

	NEW(source, char, size);
	NEW(target, char, size);
	
	/* ���ɴ��Ƚ����ݻ����� */
	for(i = 0; i < size; i++) {
		source[i] = va_arg(arg, char);
	}

	if(memoryRead(address, size, target)) {
		if((result = memcmp(source, target, size)) == 0) {
			result = 1;
		} else {
			result = 0;
		}
	} else {
		result = 0;
	}

	DEL(source);
	DEL(target);

	va_end(arg);

	return result;
}
