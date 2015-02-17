/*
	$ Monitor Module   (C) 2005-2014 MF
	$ handler.h
*/


#ifndef _MODULE_HANDLER_H_
#define _MODULE_HANDLER_H_



/* ���ƺ������� */
#define MM_HANDLER_MAX				0x04




/* for DLL */
class handler_dll : public HandlerClass {
	private:
		/* DLL ��� */
		void *		handle;

		/* ����ָ�� */
		uintptr_t 	pointer		[MM_HANDLER_MAX];

		/* ���غ��� */
		bool	load	(unsigned int index, const char *name);


	public:
		virtual	bool	start	(const char *path);
		virtual	void	close	(void);

		virtual	bool	send	(unsigned int index, const char *function, int argc, void *argv);
};




/* for PHP */
class handler_php : public HandlerClass {
	private:
		/* ���̾�� */
		void *	process;
		
		/* ��ָ�� */
		void *	stream_in;
		void *	stream_out;
		void *	stream_error;

		/* ��ȡ�ܵ� */
		char *	read	(void *stream, unsigned int *size, unsigned int timeout);
		/* д��ܵ� */
		bool	write	(const char *format, ...);

		/* ˢ�½���״̬ */
		void	refresh	(void);

		/* ��ȡ�������� */
		char *	command	(void);


	public:
		virtual	bool	start	(const char *path);
		virtual	void	close	(void);

		virtual	bool	send	(unsigned int index, const char *function, int argc, void *argv);
};




#endif   /* HANDLER */