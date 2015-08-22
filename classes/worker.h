/*
  +----------------------------------------------------------------------+
  | pthreads                                                             |
  +----------------------------------------------------------------------+
  | Copyright (c) Joe Watkins 2012 - 2015                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Joe Watkins <krakjoe@php.net>                                |
  +----------------------------------------------------------------------+
 */
#ifndef HAVE_PTHREADS_CLASS_WORKER_H
#define HAVE_PTHREADS_CLASS_WORKER_H
PHP_METHOD(Worker, shutdown);
PHP_METHOD(Worker, isShutdown);
PHP_METHOD(Worker, stack);
PHP_METHOD(Worker, unstack);
PHP_METHOD(Worker, getStacked);
PHP_METHOD(Worker, collect);

ZEND_BEGIN_ARG_INFO_EX(Worker_shutdown, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Worker_isShutdown, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Worker_stack, 0, 0, 1)
	ZEND_ARG_INFO(0, work)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(Worker_unstack, 0, 0, 0)
	ZEND_ARG_INFO(0, work)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(Worker_getStacked, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(Worker_collect, 0, 0, 1)
	ZEND_ARG_INFO(0, function)
ZEND_END_ARG_INFO()

extern zend_function_entry pthreads_worker_methods[];
#else
#	ifndef HAVE_PTHREADS_CLASS_WORKER
#	define HAVE_PTHREADS_CLASS_WORKER
zend_function_entry pthreads_worker_methods[] = {
	PHP_ME(Worker, shutdown, Worker_shutdown, ZEND_ACC_PUBLIC)
	PHP_ME(Worker, stack, Worker_stack, ZEND_ACC_PUBLIC)
	PHP_ME(Worker, unstack, Worker_unstack, ZEND_ACC_PUBLIC)
	PHP_ME(Worker, getStacked, Worker_getStacked, ZEND_ACC_PUBLIC)
	PHP_ME(Worker, isShutdown, Worker_isShutdown, ZEND_ACC_PUBLIC)
	PHP_ME(Worker, collect, Worker_collect, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ proto int Worker::stack(Collectable $work)
	Pushes an item onto the stack, returns the size of stack */
PHP_METHOD(Worker, stack)
{
	PTHREAD thread = PTHREADS_FETCH;
	zval 	*work;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &work, pthreads_collectable_entry)==SUCCESS) {
		RETURN_LONG(pthreads_stack_push(thread, work));
	}
} /* }}} */

/* {{{ proto int Worker::unstack([Collectable $work])
	Removes an item from the stack, if no item is specified the stack is cleared, returns the size of stack */
PHP_METHOD(Worker, unstack)
{
	PTHREAD thread = PTHREADS_FETCH;
	zval * work;
	
	if (ZEND_NUM_ARGS() > 0) {
		if (zend_parse_parameters(ZEND_NUM_ARGS(), "O", &work, pthreads_collectable_entry)==SUCCESS) {
			RETURN_LONG(pthreads_stack_pop(thread, work));
		}
	} else RETURN_LONG(pthreads_stack_pop(thread, NULL));
}

/* {{{ proto int Worker::getStacked()
	Returns the current size of the stack */
PHP_METHOD(Worker, getStacked)
{
	PTHREAD thread = PTHREADS_FETCH;

	RETURN_LONG(pthreads_stack_length(thread));
}

/* {{{ proto Worker::isShutdown()
	Will return true if the Worker has been shutdown */
PHP_METHOD(Worker, isShutdown)
{
	PTHREAD thread = PTHREADS_FETCH;

	RETURN_BOOL(pthreads_monitor_check(thread->monitor, PTHREADS_MONITOR_JOINED));
} /* }}} */

/* {{{ proto boolean Worker::shutdown()
		Will wait for execution of all Stackables to complete before shutting down the Worker */
PHP_METHOD(Worker, shutdown) 
{ 
	PTHREAD thread = PTHREADS_FETCH;

	RETURN_BOOL(pthreads_join(thread));
} /* }}} */

/* {{{ proto long Worker::getThreadId()
	Will return the identifier of the referenced Worker */
PHP_METHOD(Worker, getThreadId)
{
	ZVAL_LONG(return_value, (PTHREADS_FETCH_FROM(Z_OBJ_P(getThis())))->local.id);
} /* }}} */

/* {{{ proto long Worker::getCreatorId() 
	Will return the identifier of the thread ( or process ) that created the referenced Worker */
PHP_METHOD(Worker, getCreatorId)
{
	ZVAL_LONG(return_value, (PTHREADS_FETCH_FROM(Z_OBJ_P(getThis())))->creator.id);
} /* }}} */

/* {{{ proto bool Worker::collect(callable function) */
PHP_METHOD(Worker, collect) 
{
	pthreads_call_t call = PTHREADS_CALL_EMPTY;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "f", &call.fci, &call.fcc) != SUCCESS) {
		return;
	}

	ZVAL_BOOL(return_value, pthreads_stack_collect(PTHREADS_FETCH_FROM(Z_OBJ_P(getThis())), &call));
}
#	endif
#endif

