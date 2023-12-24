#include <stdio.h>
#include <limits.h>

#include <sys/sysinfo.h>

int main(void)
{
	struct sysinfo info;

	if (sysinfo(&info) == -1){
		printf("Ooops!\n");
		return 1;
	}

	printf("unit is %d bytes long\n\n", info.mem_unit);

	printf("loads[0] = %.2lf\n", (double)info.loads[0] / (double)USHRT_MAX);
	printf("loads[1] = %.2lf\n", (double)info.loads[1] / (double)USHRT_MAX);
	printf("loads[2] = %.2lf\n", (double)info.loads[2] / (double)USHRT_MAX);
	printf("totalswap = %ldMB\n", (info.totalswap / 1024));
	printf("freeswap = %ldMB\n", (info.freeswap / 1024));
	printf("totalram = %ldMB\n", (info.totalram / 1024));
	printf("freeram = %ldMB\n", (info.freeram / 1024));
	printf("sharedram = %ldMB\n", (info.sharedram / 1024));
	printf("bufferram = %ldMB\n", (info.bufferram / 1024));
	printf("procs = %d\n", info.procs);
	printf("uptime = %ldsec\n", info.uptime);

	return 0;
}
