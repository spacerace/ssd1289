#include <string.h>
#include "shell.h"
#include "usart.h"
#include "leds.h"
#include "buttons.h"
#include "freertos/include/FreeRTOS.h"
#include "shell.h"
#include "ssd1289.h"

#define SHELL_PROMPT	"? "

int parse_cmd(char *cmd);
void shell_help();

int init_console() {
		return 0;
}

int console_main() {
	char c;
	int cmd_idx;
	char cmd[129];
	
	usart1_puts(SHELL_PROMPT);
	cmd_idx = 0;

	for(;;) {
		c = usart1_getc();
		
		if(c == '\r') {
			cmd[cmd_idx] = '\0';
			cmd_idx = 0;
			usart1_puts("\r\n");
			parse_cmd(cmd);
			usart1_puts(SHELL_PROMPT);
		} else {
			if(cmd_idx == 128) {
				usart1_puts("\r\n  error: cmd overflow.\r\n");
				cmd_idx = 0;
				usart1_puts(SHELL_PROMPT);
			} else {
				cmd[cmd_idx] = c;
				cmd_idx++;
				usart1_putc(c);
			}
		}
	}
}

int parse_cmd(char *cmd) {
	int cmd_len = strlen(cmd);

	if(!strncmp(cmd, "help", 4)) {
		shell_help();
		return 0;
	} else if(!strncmp(cmd, "reset", 5)) {
		usart1_puts("system reset.\r\n\r\n");
		return 0;
	} else if(!strncmp(cmd, "clear", 5)) {
		ssd1289_clear();
		return 0;
	} else if(!strncmp(cmd, "demo", 4)) {
		usart1_puts("restarting GUI demo...\r\n");
		return 0;
	} else if(!strncmp(cmd, "bl", 2)) {
		usart1_puts("backlight=\r\n");
		return 0;
	} else if(!strncmp(cmd, "serdisp", 7)) {
		usart1_puts("going into serial display mode...\r\n");
		ssd1289_fill_screen(RGB_COL_GREY);
		return 0;
	} else if(!strncmp(cmd, "text", 4)) {
		return 0;
	}
	
	usart1_puts("ERR? command not understood.\r\n");
	
	return -1;
}

void shell_help() {
	
	
}
