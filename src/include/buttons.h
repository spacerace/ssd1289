#ifndef __BUTTONS_H__
#define __BUTTONS_H__

extern void init_buttons();

/*	KEYA/BTNA	PC13
 *	KEYB/BTNB	PB2
 */

int btna_hit();
void btna_wait();
int btnb_hit();
void btnb_wait(); 

#endif  // __BUTTONS_H__


