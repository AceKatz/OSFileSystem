#include <time.h>
int shadow_time() {
	return (int) time(NULL) / 86400;
}
