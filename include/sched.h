/*
 * Copyright (C) 2001 MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#ifndef _printf_h_
#define _printf_h_

#include <stdarg.h>
void printf(char *fmt, ...);

void _panic(const char *, int, const char *, ...)
__attribute__((noreturn));

void printcharc(char ch);

#define panic(...) _panic(__FILE__, __LINE__, __VA_ARGS__)

#endif /* _printf_h_ */
16231086_2018_jac@ubuntu:~/16231086-lab$ ^C
16231086_2018_jac@ubuntu:~/16231086-lab$ cat include/sched.h  
/* See COPYRIGHT for copyright information. */

#ifndef __SCHED_H__
#define __SCHED_H__

void sched_init(void);
void sched_yield(void);
void sched_intr(int); 

#endif /* __SCHED_H__ */