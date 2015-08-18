////////////////////////////////////////////////////////////////////////////
// File     : mapping_info.h
// Date     : 01/04/2012
// Author   : alain greiner
// Copyright (c) UPMC-LIP6
////////////////////////////////////////////////////////////////////////////
// The MAPPING_INFO data structure can be used with the GIET.
// It contains the mapping directive for one or several virtual spaces.
// Ech virtual space contains a variable number of virtual segments
// and a variable number of tasks. The number of virtual space can be one.
//
// The mapping table data structure is organised as the concatenation of
// a fixed size header, and 6 variable size arrays:
//
// - mapping_header_t   header              (MAPPING_HEADER_SIZE)
// - mapping_cluster_t  cluster[clusters]   (MAPPING_CLUSTER_SIZE * clusters)
// - mapping_pseg_t     pseg[psegs]         (MAPPING_PSEG_SIZE * psegs)
// - mapping_vspace_t   vspace[vspaces]     (MAPPING_VSPACE_SIZE * vspaces)
// - mapping_vseg_t     vseg[vsegs]         (MAPPING_VSEG_SIZE * vsegs)
// - mapping_vseg_t     vobj[vsegs]         (MAPPING_VOBJ_SIZE * vsegs)
// - mapping_task_t     task[tasks]         (MAPPING_TASK_SIZE * tasks)
//
// The number of clusters and the number of vspaces are defined in the header.
// The number of psegs are defined in each cluster.
// The number of vsegs, tasks ans mwmrs are defined in each vspace.
//
// It is intended to be stored in the boot ROM at address MAPPING_BOOT_BASE. 
// For each cluster, the base address of the first pseg descriptor
// is defined by a pseg_offset relative to MAPPING_BOOT_BASE.
// For each vspace, the base address of the first vseg descriptor
// is defined by a vseg_offset relative to MAPPING_BOOT_BASE.
// For each vspace, the base address of the first task desciptor
// is defined by a task_offset relative to MAPPING_BOOT_BASE.
// For each vspace, the base address of the first mwmr desciptor
// is defined by a mwmr_offset relative to MAPPING_BOOT_BASE.
////////////////////////////////////////////////////////////////////////////

#ifndef _MAPPING_INFO_H_
#define _MAPPING_INFO_H_

#define MAPPING_HEADER_SIZE     sizeof(mapping_header_t)
#define MAPPING_CLUSTER_SIZE    sizeof(mapping_cluster_t)
#define MAPPING_VSPACE_SIZE     sizeof(mapping_vspace_t)
#define MAPPING_VSEG_SIZE	    sizeof(mapping_vseg_t)
#define MAPPING_VOBJ_SIZE	    sizeof(mapping_vobj_t)
#define MAPPING_PSEG_SIZE	    sizeof(mapping_pseg_t)
#define MAPPING_TASK_SIZE	    sizeof(mapping_task_t)

#define C_MODE_MASK     0b1000      // cacheable
#define X_MODE_MASK     0b0100      // executable
#define W_MODE_MASK     0b0010      // writable
#define U_MODE_MASK     0b0001      // user access

#define IN_MAPPING_SIGNATURE    0xDEADBEEF
#define OUT_MAPPING_SIGNATURE   0xBABEF00D

enum 
{
    ELF = 0,    //loadable code object
    PTAB,       //page table 
    SCHED,      //schedulers
    PERI,       //hardware component
    MWMR,       //MWMR channel
    LOCK,       //Lock
    BUFFER,     //Any "no intialiasation needed" objects (stacks...)
    BARRIER     //Barrier
};


///////////////////////////////
typedef struct mapping_header_s
{
    unsigned int    signature;      // must contain MAPPING_SIGNATURE
	unsigned int	clusters;	    // number of clusters
	unsigned int	psegs;	        // number of psegs
    unsigned int    ttys;           // number of TTY terminals 
	unsigned int	globals;		// number of vsegs mapped in all vspaces
	unsigned int	vspaces;		// number of virtual spaces
	unsigned int	vsegs;  		// total number of virtual segments (for all vspaces)
	unsigned int	vobjs;  		// total number of virtual memory objects (for all vspaces)
	unsigned int	tasks;  		// total number of tasks (for all vspaces)
    char            name[32];       // mapping name
} mapping_header_t;

////////////////////////////////
typedef struct mapping_cluster_s
{
    unsigned int    procs;          // number of processors in cluster
    unsigned int    timers;         // number of timers in cluster
    unsigned int    dmas;           // number of DMA channels in cluster
} mapping_cluster_t;

/////////////////////////////
typedef struct mapping_pseg_s 
{
    char            name[32];       // pseg name (unique in a cluster)
	unsigned int    base;           // base address in physical space
	unsigned int	length;         // size (bytes)
    unsigned int    next_free_page; // physical page allocator
} mapping_pseg_t;

///////////////////////////////
typedef struct mapping_vspace_s
{
    char            name[32];       // virtual space name
    unsigned int    funcs_offset;   // offset of the vobj containing the function entry table (relative to vobj_offset)
	unsigned int	vsegs;		    // number of private virtual segments
	unsigned int	vobjs;		    // number of vobjs channels
	unsigned int	tasks;		    // number of tasks
	unsigned int	ttys;		    // number of required TTY terminals
    unsigned int    vseg_offset;    // index of first vseg in vspace 
    unsigned int    vobj_offset;    // index of first vobjs in vspace
    unsigned int    task_offset;    // index of first task in vspace
} mapping_vspace_t;

/////////////////////////////
typedef struct mapping_vseg_s 
{
	char            name[32];       // vseg name (unique in vspace)
	unsigned int    vbase;          // base address in virtual space (hexa)
	unsigned int    pbase;          // base address in physical space (hexa)
	unsigned int	length;         // size (bytes)
	unsigned int    psegid;	        // physical segment index
	unsigned char   mode;	        // C-X-W-U flags
    unsigned char   ident;          // identity mapping if non zero
	unsigned int	vobjs;		    // number of vobjs channels
    unsigned int    vobj_offset;    // index of first vobjs in vspace
    unsigned char   reserved;       // unused
} mapping_vseg_t;

/////////////////////////////
typedef struct mapping_task_s 
{
	char            name[32];       // task name (unique in vspace)
	unsigned int	clusterid;	    // physical cluster index
	unsigned int	proclocid;      // processor local index (inside cluster)
    unsigned int    vobjlocid;      // stack vobj index in vspace
    unsigned int    startid;        // index in start_vector (in seg_data)
    unsigned int    ttylocid;       // tty index (inside the vspace)
} mapping_task_t;

/////////////////////////////
typedef struct mapping_vobj_s 
{
    char            name[32];       // vobj name (unique in a vspace)
    char            binpath[64];    // path for the binary code ("*.bin")
	unsigned int    type;           // type of vobj
	unsigned int	length;         // size (bytes)
	unsigned int	align;          // required alignement (logarithm of 2)
	unsigned int	vaddr;          // virtual addresse of the vobj location (bytes)
	unsigned int	paddr;          // physical addresse of the vobj location (bytes)
} mapping_vobj_t;

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

