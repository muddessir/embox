/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#include <hal/vfork.h>


void vfork_child_done(struct task *child, void * (*run)(void *)) {

}

void *task_exit_callback(void *arg) {
	return arg;
}
