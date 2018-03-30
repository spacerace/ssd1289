#include <string.h>
#include "shell.h"
#include "usart.h"
#include "leds.h"
#include "buttons.h"
#include "freertos/include/FreeRTOS.h"
#include "shell.h"

#define SHELL_PROMPT	"? "

int parse_cmd(char *cmd);

int init_console() {
		return 0;
}

int console_main() {
	char c;
	int cmd_idx;
	char cmd[16];
	
	usart1_puts(SHELL_PROMPT);
	cmd_idx = 0;

	for(;;) {
		c = usart1_getc();
		
		if(c == '\r') {
			cmd[cmd_idx] = '\0';
			cmd_idx = 0;
			usart1_puts("\r\n");
			parse_cmd(cmd);			
		} else {
			if(cmd_idx == 14) {
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
	char temp[32];
	
	sprintf(temp, "DEBUG: cmd='%s' len=%d\n", cmd, cmd_len);
	usart1_puts(temp);
	
	return 0;
}
