#ifndef __BUTTONS_H__
#define __BUTTONS_H__

extern void init_buttons();

/*	KEYA	PC13
 *	KEYB	PB2
 */

//extern int get_keyb();
int btna_hit();
int btnb_hit();
void btnb_wait(); 

#endif  // __BUTTONS_H__


