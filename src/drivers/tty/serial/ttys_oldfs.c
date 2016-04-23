/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    09.08.2013
 */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <util/err.h>
#include <util/indexator.h>

#include <mem/misc/pool.h>
#include <drivers/char_dev.h>
#include <drivers/serial/uart_device.h>
#include "idesc_serial.h"
#include <fs/file_operation.h>
#include <fs/node.h>
#include <fs/file_desc.h>


static int uart_fsop_open(struct node *node, struct file_desc *file_desc, int flags)  {
	struct uart *uart;
	uart = uart_dev_lookup(node->name);
	if (!uart) {
		return -ENOENT;
	}
	file_desc->file_info = uart;
	idesc_serial_create(uart, flags);

	//uart_open(uart);
	return 0;
}

static int uart_fsop_close(struct file_desc *desc) {
	return 0;
}

static size_t uart_fsop_read(struct file_desc *desc, void *buf, size_t size) {
	struct uart *uart;
	int i;
	char *b;

	b = buf;
	uart = desc->file_info;
	for(i = 0; i < size; i ++) {
		while(!uart->uart_ops->uart_hasrx(uart)) {
		}
		b[i] = uart->uart_ops->uart_getc(uart);
	}

	return i;
}

static size_t uart_fsop_write(struct file_desc *desc, void *buf, size_t size) {
	struct uart *uart;
	int i;
	char *b;

	b = buf;
	uart = desc->file_info;

	for(i = 0; i < size; i ++) {
		uart->uart_ops->uart_putc(uart, b[i]);
	}
	return 0;
}

const struct kfile_operations ttys_fops = {
	.open = uart_fsop_open,
	.close = uart_fsop_close,
	.read = uart_fsop_read,
	.write = uart_fsop_write
};

int ttys_register(const char*name, void *dev_info) {
	return char_dev_register(name, &ttys_fops);
}
