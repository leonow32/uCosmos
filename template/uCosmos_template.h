#ifndef UCOSMOS_TEMPLATE_H_
#define UCOSMOS_TEMPLATE_H_

void template_task(run_mode_t run_mode) {
	
	if(run_mode == os_run) {
		
	}
	
	else if(run_mode == os_constructor) {
		
	}
	
	else if(run_mode == os_destructor) {
		
	}
	
	else if(run_mode == os_id) {
		task_name = __func__;
	}
}

#endif /* UCOSMOS_TEMPLATE_H_ */
