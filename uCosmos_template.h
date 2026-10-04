#ifndef UCOSMOS_TEMPLATE_H_
#define UCOSMOS_TEMPLATE_H_

void template_task(run_mode_t run_mode) {
	
	if(run_mode == os_run) {
		
	}
	
	else if(run_mode == os_constructor) {
		
	}
	
	else if(run_mode == os_destructor) {
		
	}
	
	#if OS_USE_TASK_IDENTIFY
	else if(run_mode == os_id) {
		printf(__func__);
	}
	#endif
}

#endif /* UCOSMOS_TEMPLATE_H_ */
