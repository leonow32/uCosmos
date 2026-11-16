#ifndef UCOSMOS_TEMPLATE_H_
#define UCOSMOS_TEMPLATE_H_

// Task template 1: using switch-case
void template_task(run_mode_t run_mode) {
	
	switch(run_mode) {
		case os_run: {						// Mandatory

			break;
		}

		case os_constructor: {				// Optional

			break;
		}

		case os_destructor: {				// Optional

			break;
		}

		case os_id: {						// Mandatory
			task_name = __func__;			// Do not change this line
			break;
		}
	}
}

// Task template 2: using if-else
void template_task(run_mode_t run_mode) {
	
	if(run_mode == os_run) {				// Mandatory

	}
	else if(run_mode == os_constructor) {	// Optional

	}
	else if(run_mode == os_destructor) {	// Optional

	}
	else if(run_mode == os_id) {			// Mandatory
		task_name = __func__;				// Do not change this line
	}
}

#endif /* UCOSMOS_TEMPLATE_H_ */
