#include <zcore/ztrace.h>
#include <zcore/zmem.h>
#include <zcore/zlog.h>

#include <signal.h>

void segfault_handler(int sig) {
	(void)sig;
	zlog_error("Segmentation fault caught! Exiting...");
	print_trace();
	exit(1);
}

int main() {
	signal(SIGSEGV, segfault_handler);
	int *a = zcalloc(10, sizeof(int));
	a[0] = 1;
	a[4] = 2;

	char *b = zcalloc(10, sizeof(char));
	b = zrealloc(b, 20 * sizeof(char));
	b = "Hello, World!";
	zmem_print_stats();
	zfree(a);
	zfree(b);
	zmem_print_stats();
	return 0;
}
