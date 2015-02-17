/*
	$ base.h, 0.1.0113
*/

#pragma warning (disable: 4996)

#ifndef _BASE_H_
#define _BASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <time.h>


#if defined(__cplusplus)
	extern "C" {
#endif


/* �ƶ��ո� */
size_t moveUp(size_t data);
size_t moveDown(size_t data);
size_t moveLeft(size_t data);
size_t moveRight(size_t data);


/* ���ַ�װ */
size_t pack(void *number);
void unpack(void *number, size_t data);


/* ��־���� */
void flagInit();
void flagClear();
int flagSet(size_t data, size_t direction);
size_t flagGet(size_t data);


/* ���в��� */
void queueInit();
void queueClear();
void queueAdd(size_t data);
size_t queueFetch();


/* ���ݼ��� */
size_t * calculate(unsigned char *number, size_t *step);


/* ͼ���û����� */
void gui();
void show(int select, unsigned char *number);
void enable(int calc, int play);
void step(char *buffer);
void focus(int set);


/* ��ť���� */
void actCalc(char *buffer, size_t length);
void actPlay();


#if defined(__cplusplus)
	}
#endif

#endif  /* _BASE_H_ */