main:
	gcc main.c -o main `mysql_config --cflags --include --libs`
main_extra:
	gcc main.c -Wall -Wextra -o main `mysql_config --cflags --include --libs`
clean:
	-rm main
