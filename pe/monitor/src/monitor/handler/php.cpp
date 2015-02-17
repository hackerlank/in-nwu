/*
	$ Monitor Module   (C) 2005-2014 MF
	$ handler/php.cpp
*/


#include "../header/base.h"
#include "../header/handler.h"



/* ��ȡ�ܵ� */
char * handler_php::read (void *stream, unsigned int *size, unsigned int timeout) {
	// ��ʼʱ��
	uint32_t	start	= (uint32_t) GetTickCount();
	// ������
	char *		buffer	= NULL;

	// �Ѷ�ȡ��С
	*size	= 0;

	while(true) {
		// �˳�����
		unsigned int code;

		// �жϽ���״̬
		if(GetExitCodeProcess((HANDLE) this->process, (LPDWORD) & code) == FALSE) {
			break;
		}

		// ����������˳�, ����ѭ��
		if(code != STILL_ACTIVE) {
			break;
		}

		// �ܵ������ݴ�С
		unsigned int length;

		// ����Ƿ������ݿɶ�
		if(PeekNamedPipe((HANDLE) stream, NULL, 0, NULL, (LPDWORD) & length, NULL) == FALSE) {
			break;
		}

		// ����������ݿɶ�
		if(length > 0) {
			// ���仺������С
			if((buffer = (char *) realloc(buffer, *size + length)) == NULL) {
				break;
			}

			// ��ȡ����
			ReadFile((HANDLE) stream, buffer + *size, length, (LPDWORD) & length, NULL);

			// �����Ѷ�ȡ��С
			*size = *size + length;

			// �ж����һ���ֽ��Ƿ�Ϊ������־
			if(*(buffer + *size - 1) == '\xFF') {
				break;
			}
		} else {
			// ��ǰʱ��
			uint32_t	current		= (uint32_t) GetTickCount();

			// ���ʱ�����, ���ÿ�ʼʱ��
			if(current < start) {
				start = current;
			}

			// �����Ƿ�ʱ
			if((current - start) > timeout) {
				break;
			}
		}
	}

	return buffer;
}


/* д��ܵ� */
bool handler_php::write (const char *format, ...) {
	// ��������ѹر�, �޷�д��
	if(this->process == NULL) {
		return false;
	}

	// �����б��ַ
	va_list	argument	= (va_list) (((uintptr_t) & format) + sizeof(uintptr_t));

	// �������ɳ��� (�� \0)
	int		length		= _vscprintf(format, argument) + 1;

	// �жϳ����쳣
	if(length <= 1) {
		return false;
	}

	// ������
	char *	buffer		= new char	[length];

	// ת��Ϊ ansi
	vsprintf(buffer, format, argument);

	// ���ɽ��
	bool			result	= false;
	// д��ĳ���
	unsigned int	written	= 0;

	// д�뵽��׼��
	if(WriteFile(this->stream_in, (LPCVOID) buffer, (DWORD) length, (LPDWORD) & written, NULL) != FALSE) {
		// �ж�д������
		if(written == (unsigned int) length) {
			result = true;
		}
	}

	delete [] buffer;

	return result;
}



/* ˢ�½���״̬ */
void handler_php::refresh (void) {
	// �жϵ�ǰ״̬
	if(this->process == NULL) {
		return;
	}

	// �˳�����
	unsigned int code;

	// ��ȡ�ӽ����˳�����
	if(GetExitCodeProcess((HANDLE) this->process, (LPDWORD) & code)) {
		// ������������
		if(code == STILL_ACTIVE) {
			return;
		}
	}

	// ���ý����˳�״̬
	this->process	= NULL;

	// �����ʾ��Ϣ
	Console::error("#HANDLER#Child process has been terminated (File: php.exe, Code: 0x%02X)\n", code);
}



/* ��ȡ�������� */
char * handler_php::command (void) {
	// ��ǰģ������·��
	char	directory	[MAX_PATH + 1];

	// ���� 3 ��·�����Ȼ�����
	char *	buffer		= new char [MAX_PATH * 3];

	// ��ջ�����
	memset(directory,	0, MAX_PATH + 1);
	memset(buffer,		0, MAX_PATH * 3);

	// ��ȡ·��
	if(GetModuleFileNameA((HMODULE) Handler::dll, directory, MAX_PATH) == 0) {
		goto failed;
	}

	// ����·������β
	char * end	= strrchr(directory, '\\');

	// ·������ʧ��
	if(end == NULL) {
		goto failed;
	}

	// �������� \\ Ϊ \0, �����ַ���
	*end = '\0';

	// ������������
	sprintf(buffer, "\"%s\\php\\php.exe\" -f \"%s\\php\\monitor.php\"", directory, directory);

	return buffer;

failed:
	// �ͷŻ�����
	delete [] buffer;

	return false;
}




/* ���� */
bool handler_php::start (const char *path) {
	// ��������
	char *	command		= this->command();

	// ��ȡ��������ʧ��
	if(command == NULL) {
		return false;
	}

	// �ܵ�ͨ��
	HANDLE	pipe_in_read,  pipe_in_write;					// stdin
	HANDLE	pipe_out_read, pipe_out_write;					// stdout
	HANDLE	pipe_err_read, pipe_err_write;					// stderr

	// ��ȫ
	SECURITY_ATTRIBUTES		security;

	ZeroMemory(& security, sizeof(security));

	security.nLength				= sizeof security;
	security.lpSecurityDescriptor	= NULL;
	security.bInheritHandle			= TRUE;					// �ӽ��̿��Լ̳�

	// stdin
	if(CreatePipe(& pipe_in_read, & pipe_in_write, & security, 0) == FALSE) {
		return false;
	}

	// stdout
	if(CreatePipe(& pipe_out_read, & pipe_out_write, & security, 0) == FALSE) {
		return false;
	}

	// stderr
	if(CreatePipe(& pipe_err_read, & pipe_err_write, & security, 0) == FALSE) {
		return false;
	}

	// ������Ϣ
	STARTUPINFO			startup;
	PROCESS_INFORMATION	process;
	
	ZeroMemory(& startup,	sizeof startup);
	ZeroMemory(& process,	sizeof process);

	// ��������
	startup.cb			= sizeof startup;
	startup.hStdInput	= pipe_in_read;
	startup.hStdOutput	= pipe_out_write;
	startup.hStdError	= pipe_err_write;
	startup.wShowWindow	= SW_HIDE;
	startup.dwFlags		= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	// ��������
	BOOL result = CreateProcessA (
		/* lpApplicationName */		NULL,
		/* lpCommandLine */			(LPSTR) command,
		/* lpProcessAttributes */	NULL,
		/* lpThreadAttributes */	NULL,
		/* bInheritHandles */		TRUE,					// �ӽ��̱�����Լ̳о��
		/* dwCreationFlags */		0,
		/* lpEnvironment */			NULL,
		/* lpCurrentDirectory */	NULL,
		/* lpStartupInfo */			& startup,
		/* lpProcessInformation */	& process
	);

	// �ͷŲ���
	delete [] command;

	// ����ʧ��
	if(result == FALSE) {
		Console::error("#HANDLER#Failed to run child process (File: php.exe)\n");

		return false;
	}

	// �رչܵ����
	CloseHandle(pipe_in_read);
	CloseHandle(pipe_out_write);
	CloseHandle(pipe_err_write);

	// �ر��߳̾��
	CloseHandle(process.hThread);

	// ������
	this->process		= (void *) process.hProcess;
	this->stream_in		= (void *) pipe_in_write;
	this->stream_out	= (void *) pipe_out_read;
	this->stream_error	= (void *) pipe_err_read;

	// ���Ͳ���
	uintptr_t argv [ ] = {
		(uintptr_t)		"%hs",
		(uintptr_t)		path,
	};

	// ����ű�
	if(this->send(0, "~!", 1, (void *) argv) == true) {
		Console::error("#HANDLER#Failed to load script: %s\n", path);

		return false;
	}

	return true;
}


/* �ر� */
void handler_php::close (void) {
	// û�н���ֱ�ӷ���
	if(this->process == NULL) {
		return;
	}

	// ��ֹ�ӽ���
	TerminateProcess(this->process, 0);

	// �ȴ��ӽ��̽���
	WaitForSingleObject(this->process, INFINITE);

	// �ͷ����йܵ����
	CloseHandle(this->stream_in);
	CloseHandle(this->stream_out);
	CloseHandle(this->stream_error);

	// �ͷŽ��̾��
	CloseHandle(this->process);

	// ���ý��̾��
	this->process	= NULL;
}


/* �������� */
bool handler_php::send (unsigned int index, const char *function, int argc, void *argv) {
	// ˢ���ӽ���״̬
	this->refresh();

	// ���û�н��̾��, ֱ���������
	if(this->process == NULL) {
		return true;
	}

	// ���ͺ�������
	if(this->write("%d@%s\n", argc, function) == false) {
		goto failed;
	}

	// ��ʽ��ַ
	uintptr_t	format	= (uintptr_t) argv;
	// ����ֵ��ַ
	uintptr_t	value	= format + sizeof(uintptr_t);

	// ���Ͳ�������
	for(int i = 0; i < argc; i ++) {
		// ���Ͳ�����Ϣ
		if(this->write(*((const char **) format), *((uint32_t *) value)) == false) {
			goto failed;
		}

		// ���Ͳ���������־
		if(this->write("%hs", "\n") == false) {
			goto failed;
		}

		// ָ����һ������
		format	= value  + sizeof(uintptr_t);
		value	= format + sizeof(uintptr_t);
	}

	// ������Ϣ����
	unsigned int	size_error	= 0;
	unsigned int	size_out	= 0;

	// ��ȡ������Ϣ
	char *	data_error	= this->read(this->stream_error,	& size_error,	500);
	char *	data_out	= this->read(this->stream_out,		& size_out,		100);

	// �����׼���������
	if(data_out != NULL) {
		Console::notice("#HANDLER#STDOUT: %hs\n", data_out);

		// �ͷŻ�����
		free(data_out);
	}

	// �жϱ�׼�������Ƿ񷵻�
	if(data_error == NULL) {
		Console::warning("#HANDLER#STDERR return 0 byte  [ACCEPT]\n");

		// ֱ���������
		return true;
	}

	// �Ƿ����ִ��
	bool	next	= true;

	if(size_error > 0) {
		// �жϿ����ַ�
		switch(*data_error) {
			// �ܾ�
			case 0x00:
				next	= false;

				// �����ʾ
				if(*function != '~') {
					Console::warning("#HANDLER#%s handled   [REJECT]\n", function);
				}

				break;

			// ����
			case 0x01:
				break;

			default:
				{
					char * buffer	= new char [size_error + 1];

					// ���ƴ�����Ϣ
					memcpy(buffer, data_error, size_error);
					// �����ַ���
					memset(buffer + size_error, 0, 1);

					// �������
					Console::warning("#HANDLER#STDERR return unexpected string: %s\n", buffer);
					Console::warning("#HANDLER#STDERR return no control character   [ACCEPT]\n");

					delete [] buffer;
				}
		}
	}

	free(data_error);

	return next;


failed:
	// �������
	Console::error("#HANDLER#Failed to write to STDIN   [ACCEPT]\n");

	// �������
	return true;
}

