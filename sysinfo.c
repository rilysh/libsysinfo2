/*
 * Copyright (C) 2010 Kostas Petrikas, All rights reserved.
 *
 * libsysinfo2 is a modified version by rilysh <nightquick@proton.me>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name(s) of the author(s) may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include <kvm.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/sysctl.h>
#include <sys/user.h>

#include "sysinfo.h"

#ifndef UNIT
#  define UNIT               1024    /* KB */
#endif

#define NLOADS               3
#define VMVPAGE_COUNT        "vm.stats.vm.v_page_count"
#define VMVFREE_COUNT        "vm.stats.vm.v_free_count"
#define VMVACTIVE_COUNT      "vm.stats.vm.v_active_count"
#define KERNIPC_SHMEM        "kern.ipc.shmmax"

#define CONVERT_UNIT(x) (((double)x * getpagesize()) / UNIT)

/* sysctl wrapper */
static int get_sysctl(const char *name, void *ptr, size_t len)
{
	size_t nlen = len;

	if (sysctlbyname(name, ptr, &nlen, NULL, 0) == -1)
		return -1;
	if (nlen != len)
		return -1;

	return 0;
}

/* Retrieve the system uptime in seconds */
static int get_system_uptime(struct sysinfo *si)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_UPTIME, &ts) == -1)
		return -1;

	si->uptime = ts.tv_sec;
	return 0;
}

/* Retrieve the system's loadavg */
static int get_system_loadavg(kvm_t *kvm, struct sysinfo *si)
{
	double load_avg[NLOADS];

	if (kvm_getloadavg(kvm, load_avg, NLOADS) == -1) {
		errno = 0;
		return -1;
	}

	si->loads[0] = (unsigned long)((double)load_avg[0] * USHRT_MAX);
	si->loads[1] = (unsigned long)((double)load_avg[1] * USHRT_MAX);
	si->loads[2] = (unsigned long)((double)load_avg[2] * USHRT_MAX);

	return 0;
}

/* Get information about total and available swap size */
static int get_system_swapinfo(kvm_t *kvm, struct sysinfo *si)
{
	struct kvm_swap kswap;

	if (kvm_getswapinfo(kvm, &kswap, 1, 0) == -1) {
		errno = 0;
	        return -1;
	}

	si->totalswap = (unsigned long)CONVERT_UNIT(kswap.ksw_total);
	si->freeswap = (si->totalswap - (unsigned long)CONVERT_UNIT(kswap.ksw_used));
	return 0;
}

/* Get total active process in the kernel */
static int get_procs_count(kvm_t *kvm, struct sysinfo *si)
{
	int nprocs;

	if (kvm_getprocs(kvm, KERN_PROC_ALL, 0, &nprocs) == NULL) {
		errno = 0;
		return -1;
	}

	si->procs = (unsigned short)nprocs;
	return 0;
}

/* Initial function */
int sysinfo(struct sysinfo *info)
{
	kvm_t *kvm;
	int ret;
	int total, free, active;
	unsigned long shmmax;

	if (info == NULL) {
		errno = EFAULT;
		return -1;
	}

	memset(info, 0, sizeof(struct sysinfo));	
	info->mem_unit = UNIT;

	/* kvm init */
	kvm = kvm_open(NULL, "/dev/null", "/dev/null", O_RDONLY, "kvm_open");
	if (kvm == NULL) {
		errno = EIO;
		return -1;
	}

	ret = get_system_loadavg(kvm, info);
	if (ret == -1)
		return -1;

	ret = get_system_swapinfo(kvm, info);
	if (ret == -1)
		return -1;

	ret = get_procs_count(kvm, info);
	if (ret == -1)
		return -1;

	/* Close the kvm session */
	if (kvm_close(kvm) == -1) {
		errno = 0;
		return -1;
	}

	ret = get_system_uptime(info);
	if (ret == -1)
		return -1;

	/* sysctl: vm.stats.vm.v_page_count */
	ret = get_sysctl(VMVPAGE_COUNT, &total, sizeof(total));
	if (ret == -1) {
		errno = EVMVPAGE_COUNT;
	        return -1;
	}

        /* sysctl: vm.stats.vm.v_free_count */
	ret = get_sysctl(VMVFREE_COUNT, &free, sizeof(free));
	if (ret == -1) {
		errno = EVMVFREE_COUNT;
		return -1;
	}

	/* sysctl: vm.stats.vm.v_active_count */
	ret = get_sysctl(VMVACTIVE_COUNT, &active, sizeof(active));
	if (ret == -1) {
		errno = EVMVACTIVE_COUNT;
	        return -1;
	}

	/* sysctl: kern.ipc.shmmax */
	ret = get_sysctl(KERNIPC_SHMEM, &shmmax, sizeof(shmmax));
	if (ret == -1) {
		errno = EKERNIPC_SHMEM;
	        return -1;
	}

	info->totalram = (unsigned long)CONVERT_UNIT(total);
	info->freeram = (unsigned long)CONVERT_UNIT(free);
	info->bufferram = (unsigned long)CONVERT_UNIT(active);
	info->sharedram = shmmax / UNIT;

	return 0;
}
