#ifndef BSD_SYSINFO_H
#define BSD_SYSINFO_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#define SI_LOAD_SHIFT     16

#define EVMVPAGE_COUNT    2
#define EVMVFREE_COUNT    3
#define EVMVACTIVE_COUNT  4
#define EKERNIPC_SHMEM    5

struct sysinfo {
	long uptime;                   /* Seconds since boot */
	unsigned long loads[3];        /* 1, 5, and 15 minute load averages */
	unsigned long totalram;        /* Total usable main memory size */
	unsigned long freeram;         /* Available memory size */
	unsigned long sharedram;       /* Amount of shared memory */
	unsigned long bufferram;       /* Memory used by buffers */
	unsigned long totalswap;       /* Total swap space size */
	unsigned long freeswap;	       /* swap space still available */
	unsigned short procs;          /* Number of current processes */
	unsigned short pad;	       /* Leaving this for linux compatability */
	unsigned long totalhigh;       /* Total high memory size */
	unsigned long freehigh;        /* Available high memory size */
        unsigned int mem_unit;         /* Memory unit size in bytes */
	char _f[20-2*sizeof(long)-sizeof(unsigned int)];      /* Leaving this for linux compatability */
};

int sysinfo(struct sysinfo *info);

__END_DECLS

#endif
