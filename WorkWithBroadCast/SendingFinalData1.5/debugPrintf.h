#ifndef _DEBUGER_H_
#define _DEBUGER_H_

#ifdef DEBUG
	#define debug_printf printf
#else 
	#define debug_printf
#endif

#endif
