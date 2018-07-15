%: %.c
	gcc -g -o $@ $< -Wall -Werror
