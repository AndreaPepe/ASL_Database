all:
	gcc *.c -o client `mysql_config --cflags --include --libs`
all_extra:
	gcc *.c -Wall -Wextra -o client `mysql_config --cflags --include --libs`
clean:
	-rm client
