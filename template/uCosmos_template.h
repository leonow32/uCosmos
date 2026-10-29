#ifndef UCOSMOS_TEMPLATE_H_
#define UCOSMOS_TEMPLATE_H_

void template_task(run_mode_t run_mode) {
	
	switch(run_mode) {
		case os_run: {

			break;
		}

		case os_constructor: {

			break;
		}

		case os_destructor: {

			break;
		}

		case os_id: {
			task_name = __func__;
			break;
		}
	}
}

#endif /* UCOSMOS_TEMPLATE_H_ */
