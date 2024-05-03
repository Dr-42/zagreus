#include <zcore/zlog.h>
#include <zcore/ztrace.h>

void foo() {
	print_trace();
}

int main() {
	zlog_info("Hello, World!\n");
	foo();
	return 0;
}
