/* 
 * SPDX License Indentifier:MIT License
 * Machelyng Operating System
 * Copyright (c) 1991,1990,1989 University
 * All Rights Reserved.
 * 
 */



#ifndef	_MACHELYNG_BOOLEAN_H_
#define _MACHELYNG_BOOLEAN_H_

#ifdef DRIVER_MACHELYNG
typedef int		boolean_t;

#ifndef	TRUE
#define TRUE 1
#endif	/* TRUE */

#ifndef	FALSE
#define FALSE	 0
#endif	/* FALSE */
#endif /* DRIVER_MACHELYNG */
#endif /* _MACHELYNG_BOOLEAN_H _ */
