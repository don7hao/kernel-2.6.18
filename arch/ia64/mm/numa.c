/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * This file contains NUMA specific variables and functions which can
 * be split away from DISCONTIGMEM and are used on NUMA machines with
 * contiguous memory.
 * 
 *                         2002/08/07 Erich Focht <efocht@ess.nec.de>
 */

#include <linux/cpu.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/node.h>
#include <linux/init.h>
#include <linux/bootmem.h>
#include <asm/mmzone.h>
#include <asm/numa.h>


/*
 * The following structures are usually initialized by ACPI or
 * similar mechanisms and describe the NUMA characteristics of the machine.
 */
int num_node_memblks;
struct node_memblk_s node_memblk[NR_NODE_MEMBLKS];
struct node_cpuid_s node_cpuid[NR_CPUS];
/*
 * This is a matrix with "distances" between nodes, they should be
 * proportional to the memory access latency ratios.
 */
u8 numa_slit[MAX_NUMNODES * MAX_NUMNODES];

/* Identify which cnode a physical address resides on */
int
paddr_to_nid(unsigned long paddr)
{
	int	i;

	for (i = 0; i < num_node_memblks; i++)
		if (paddr >= node_memblk[i].start_paddr &&
		    paddr < node_memblk[i].start_paddr + node_memblk[i].size)
			break;

	return (i < num_node_memblks) ? node_memblk[i].nid : (num_node_memblks ? -1 : 0);
}

#if defined(CONFIG_SPARSEMEM) && defined(CONFIG_NUMA)
/*
 * Because of holes evaluate on section limits.
 * If the section of memory exists, then return the node where the section
 * resides.  Otherwise return node 0 as the default.  This is used by
 * SPARSEMEM to allocate the SPARSEMEM sectionmap on the NUMA node where
 * the section resides.
 */
int early_pfn_to_nid(unsigned long pfn)
{
	int i, section = pfn >> PFN_SECTION_SHIFT, ssec, esec;

	for (i = 0; i < num_node_memblks; i++) {
		ssec = node_memblk[i].start_paddr >> PA_SECTION_SHIFT;
		esec = (node_memblk[i].start_paddr + node_memblk[i].size +
			((1L << PA_SECTION_SHIFT) - 1)) >> PA_SECTION_SHIFT;
		if (section >= ssec && section < esec)
			return node_memblk[i].nid;
	}

	return 0;
}
#endif
