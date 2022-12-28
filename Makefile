all:
	gcc main.c vt_one_hundrify.c map.c rgbtohsv.c readpng.c fetch.c -lcurl -lpng -o radascii -g -fsanitize=address
