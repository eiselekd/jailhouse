/*
 * Jailhouse, a Linux-based partitioning hypervisor
 *
 * Copyright (c) Siemens AG, 2013
 *
 * Authors:
 *  Jan Kiszka <jan.kiszka@siemens.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 */

#include <stdarg.h>
#include <jailhouse/control.h>
#include <jailhouse/printk.h>
#include <jailhouse/processor.h>
#include <jailhouse/string.h>
#include <asm/bitops.h>
#include <asm/spinlock.h>

static DEFINE_SPINLOCK(printk_lock);

#define console_write(msg)	arch_dbg_write(msg)
#include "printk-core.c"

static void dbg_write_stub(const char *msg)
{
}

void (*arch_dbg_write)(const char *msg) = dbg_write_stub;

void printk(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	spin_lock(&printk_lock);
	__vprintk(fmt, ap);
	spin_unlock(&printk_lock);

	va_end(ap);
}

void panic_printk(const char *fmt, ...)
{
	unsigned long cpu_id = phys_processor_id();
	va_list ap;

	if (test_and_set_bit(0, &panic_in_progress) && panic_cpu != cpu_id)
		return;
	panic_cpu = cpu_id;

	va_start(ap, fmt);

	__vprintk(fmt, ap);

	va_end(ap);
}
