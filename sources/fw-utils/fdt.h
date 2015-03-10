#ifndef _FDT_H
#define _FDT_H

struct fdt_header {
	unsigned long magic;			 /* magic word FDT_MAGIC */
	unsigned long totalsize;		 /* total size of DT block */
	unsigned long off_dt_struct;		 /* offset to structure */
	unsigned long off_dt_strings;	 /* offset to strings */
	unsigned long off_mem_rsvmap;	 /* offset to memory reserve map */
	unsigned long version;		 /* format version */
	unsigned long last_comp_version;	 /* last compatible version */

	/* version 2 fields below */
	unsigned long boot_cpuid_phys;	 /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
	unsigned long size_dt_strings;	 /* size of the strings block */

	/* version 17 fields below */
	unsigned long size_dt_struct;	 /* size of the structure block */
};

#define FDT_MAGIC	0xd00dfeed	/* 4: version, 4: total size */
#define FDT_TAGSIZE	sizeof(unsigned long)

#define FDT_BEGIN_NODE	0x1		/* Start node: full name */
#define FDT_END_NODE	0x2		/* End node */
#define FDT_PROP	0x3		/* Property: name off,
					   size, content */
#define FDT_NOP		0x4		/* nop */
#define FDT_END		0x9

#define FDT_V1_SIZE	(7*sizeof(unsigned long))
#define FDT_V2_SIZE	(FDT_V1_SIZE + sizeof(unsigned long))
#define FDT_V3_SIZE	(FDT_V2_SIZE + sizeof(unsigned long))
#define FDT_V16_SIZE	FDT_V3_SIZE
#define FDT_V17_SIZE	(FDT_V16_SIZE + sizeof(unsigned long))

#define FDT_FIRST_SUPPORTED_VERSION	0x10
#define FDT_LAST_SUPPORTED_VERSION	0x11

#define SW_MAGIC		(~FDT_MAGIC)
#endif /* _FDT_H */
