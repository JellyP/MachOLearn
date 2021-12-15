loader.h   [plain text]
/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
#ifndef _MACHO_LOADER_H_
#define _MACHO_LOADER_H_

/*
 * This file describes the format of mach object files.
 */

/*
 * <mach/machine.h> is needed here for the cpu_type_t and cpu_subtype_t types
 * and contains the constants for the possible values of these types.
 * 此处需要 <mach/machine.h> 用于 cpu_type_t 和 cpu_subtype_t 类型，并包含这些类型的可能值的常量。
 */
#include <mach/machine.h>

/*
 * <mach/vm_prot.h> is needed here for the vm_prot_t type and contains the 
 * constants that are or'ed together for the possible values of this type.
 * <mach/vm_prot.h> 此处需要 vm_prot_t 类型，并包含针对此类型的可能值进行或运算的常量。
 */
#include <mach/vm_prot.h>

/*
 * <machine/thread_status.h> is expected to define the flavors of the thread
 * states and the structures of those flavors for each machine.
 * <machine/thread_status.h> 定义不同cpu架构下线程的状态以及每台机器的这些线程状态的结构。
 */
#include <mach/machine/thread_status.h>
#include <architecture/byte_order.h>

/*
 * The mach header appears at the very beginning of the object file.
 */
struct mach_header {
	unsigned long	magic;		/* mach magic number identifier */
	cpu_type_t	cputype;	/* cpu specifier */
	cpu_subtype_t	cpusubtype;	/* machine specifier */
	unsigned long	filetype;	/* type of file */
	unsigned long	ncmds;		/* number of load commands */
	unsigned long	sizeofcmds;	/* the size of all the load commands */
	unsigned long	flags;		/* flags */
};

/* Constant for the magic field of the mach_header */
#define	MH_MAGIC	0xfeedface	/* the mach magic number */
#define MH_CIGAM	NXSwapInt(MH_MAGIC)

/*
 * The layout of the file depends on the filetype.  For all but the MH_OBJECT
 * file type the segments are padded out and aligned on a segment alignment
 * boundary for efficient demand pageing.  The MH_EXECUTE, MH_FVMLIB, MH_DYLIB,
 * MH_DYLINKER and MH_BUNDLE file types also have the headers included as part
 * of their first segment.
 * MachoO文件的布局取决于文件类型。对于除 MH_OBJECT 文件类型之外的所有文件类型，
 * 段补零并在段对齐边界上对齐，以实现高效的需求分页。 
 * MH_EXECUTE、MH_FVMLIB、MH_DYLIB、MH_DYLINKER 和 MH_BUNDLE 文件类型也将headers作为其第一段的一部分。
 * 
 * The file type MH_OBJECT is a compact format intended as output of the
 * assembler and input (and possibly output) of the link editor (the .o
 * format).  All sections are in one unnamed segment with no segment padding. 
 * This format is used as an executable format when the file is so small the
 * segment padding greatly increases it's size.
 * 文件类型 MH_OBJECT 是一种紧凑格式，旨在作为汇编器的输出和链接编辑器（.o 格式）的输入（也可能是输出）。 所有部分都在一个未命名的段中，没有段填充。
 * 当文件太小以至于段补零会大大增加它的大小时，此格式可以用作可执行格式。
 *
 * The file type MH_PRELOAD is an executable format intended for things that
 * not executed under the kernel (proms, stand alones, kernels, etc).  The
 * format can be executed under the kernel but may demand paged it and not
 * preload it before execution.
 * 文件类型 MH_PRELOAD 是一种可执行格式，用于不在内核（proms、standalones、kernels 等）下执行的东西。 
 * 该格式可以在内核下执行，但可能需要对其进行分页而不是在执行前预加载。
 * A core file is in MH_CORE format and can be any in an arbritray legal
 * Mach-O file.
 * 核心文件采用 MH_CORE 格式，可以是任意合法 Mach-O 文件中的任何文件。
 *
 * Constants for the filetype field of the mach_header
 * mach_header里定义的filetype常量含义
 */
#define	MH_OBJECT	0x1		/* relocatable object file 可迁移对象文件 */
#define	MH_EXECUTE	0x2		/* demand paged executable file 具有分页的可执行文件*/
#define	MH_FVMLIB	0x3		/* fixed VM shared library file 特定虚拟内存共享库文件*/
#define	MH_CORE		0x4		/* core file 核心文件 */
#define	MH_PRELOAD	0x5		/* preloaded executable file 需要预加载的可执行文件 */
#define	MH_DYLIB	0x6		/* dynamicly bound shared library file 动态绑定共享库文件*/
#define	MH_DYLINKER	0x7		/* dynamic link editor 动态链接编辑器*/
#define	MH_BUNDLE	0x8		/* dynamicly bound bundle file 动态绑定捆绑文件*/

/* Constants for the flags field of the mach_header */
/* mach_header里定义的flags常量含义 */
/* 
 * the object file has no undefined
 * references, can be executed
 * 目标文件没有未定义的引用，可以执行
*/
#define	MH_NOUNDEFS	0x1
/* 
 * the object file is the output of an
 * incremental link against a base file
 * and can't be link edited again
 * 目标文件是针对基本文件的增量链接的输出，无法再次进行链接编辑
 */		
#define	MH_INCRLINK	0x2
/*
 * the object file is input for the
 * dynamic linker and can't be staticly
 * link edited again 
 * 目标文件是动态链接器的输入，不能再次静态链接编辑
 */
#define MH_DYLDLINK	0x4	
/* 
 * the object file's undefined
 * references are bound by the dynamic
 * linker when loaded. 
 * 目标文件的未定义引用在加载时由动态链接器绑定。
 */
#define MH_BINDATLOAD	0x8
/* 
 * the file has it's dynamic undefined
 * references prebound. 
 * 文件预先绑定了动态未定义的引用。
 */
#define MH_PREBOUND	0x10		

/*
 * The load commands directly follow the mach_header.  The total size of all
 * of the commands is given by the sizeofcmds field in the mach_header.  All
 * load commands must have as their first two fields cmd and cmdsize.  The cmd
 * field is filled in with a constant for that command type.  Each command type
 * has a structure specifically for it.  The cmdsize field is the size in bytes
 * of the particular load command structure plus anything that follows it that
 * is a part of the load command (i.e. section structures, strings, etc.).  To
 * advance to the next load command the cmdsize can be added to the offset or
 * pointer of the current load command.  The cmdsize MUST be a multiple of
 * sizeof(long) (this is forever the maximum alignment of any load commands).
 * The padded bytes must be zero.  All tables in the object file must also
 * follow these rules so the file can be memory mapped.  Otherwise the pointers
 * to these tables will not work well or at all on some machines.  With all
 * padding zeroed like objects will compare byte for byte.
 * 加载命令直接跟在 mach_header 之后。所有命令的总大小由 mach_header 中的 sizeofcmds 字段给出。
 * 所有加载命令的前两个字段必须是 cmd 和 cmdsize。 
 * cmd 字段用该命令类型的常量填充。每个命令类型都有一个专门针对它的结构。 
 * cmdsize 字段是特定加载命令结构的大小（以字节为单位）加上作为加载命令一部分的任何内容（即节结构、字符串等）。
 * 要前进到下一个加载命令，可以将 当前load command的指针或者偏移量加上cmdsize 。 cmdsize 必须是 sizeof(long) 的倍数（这永远是任何加载命令的最大对齐）
 * 被填充的字节必须是按位填0。目标文件中的所有表也必须遵循这些规则，以便文件可以进行内存映射。否则，指向这些表的指针在某些机器上将无法正常工作或根本无法工作。将所有填充归零的对象将逐字节比较。
 */
struct load_command {
	unsigned long cmd;		/* type of load command */
	unsigned long cmdsize;		/* total size of command in bytes */
};

/* Constants for the cmd field of all load commands, the type */
/* 所有加载命令的 cmd 字段的常量，类型 */
#define	LC_SEGMENT	0x1	/* segment of this file to be mapped 要映射的此文件的段 */
#define	LC_SYMTAB	0x2	/* link-edit stab symbol table info 链接编辑 stab 符号表信息 */
#define	LC_SYMSEG	0x3	/* link-edit gdb symbol table info (obsolete) 链接编辑 gdb 符号表信息（已过时） */
#define	LC_THREAD	0x4	/* thread 线程 */
#define	LC_UNIXTHREAD	0x5	/* unix thread (includes a stack) unix 线程（包括一个堆栈）*/
#define	LC_LOADFVMLIB	0x6	/* load a specified fixed VM shared library 加载指定的特定VM共享库 */
#define	LC_IDFVMLIB	0x7	/* fixed VM shared library identification 特定 VM 共享库标识 */
#define	LC_IDENT	0x8	/* object identification info (obsolete) 对象标识信息（已过时）*/
#define LC_FVMFILE	0x9	/* fixed VM file inclusion (internal use) 固定的 VM 文件包含（内部使用） */
#define LC_PREPAGE      0xa     /* prepage command (internal use) prepage 命令（内部使用）*/
#define	LC_DYSYMTAB	0xb	/* dynamic link-edit symbol table info 动态链接编辑符号表信息 */
#define	LC_LOAD_DYLIB	0xc	/* load a dynamicly linked shared library 加载动态链接的共享库 */
#define	LC_ID_DYLIB	0xd	/* dynamicly linked shared lib identification 动态链接共享库标识 */
#define LC_LOAD_DYLINKER 0xe	/* load a dynamic linker 加载动态链接器 */
#define LC_ID_DYLINKER	0xf	/* dynamic linker identification 动态链接器识别 */
#define	LC_PREBOUND_DYLIB 0x10	/* modules prebound for a dynamicly 动态预绑定模块*/
				/*  linked shared library */

/*
 * A variable length string in a load command is represented by an lc_str
 * union.  The strings are stored just after the load command structure and
 * the offset is from the start of the load command structure.  The size
 * of the string is reflected in the cmdsize field of the load command.
 * Once again any padded bytes to bring the cmdsize field to a multiple
 * of sizeof(long) must be zero.
 * 加载命令中的可变长度字符串由 lc_str 的 union表示。 
 * 字符串存储在加载命令结构之后，偏移量从加载命令结构的开头开始。 
 * 字符串的大小反映在加载命令的 cmdsize 字段中。 再次强调 cmdsize 字段成为 sizeof(long) 倍数的任何填充字节必须为零。
 */
union lc_str {
	unsigned long	offset;	/* offset to the string  */
	char		*ptr;	/* pointer to the string string的地址*/
};

/*
 * The segment load command indicates that a part of this file is to be
 * mapped into the task's address space.  The size of this segment in memory,
 * vmsize, maybe equal to or larger than the amount to map from this file,
 * filesize.  The file is mapped starting at fileoff to the beginning of
 * the segment in memory, vmaddr.  The rest of the memory of the segment,
 * if any, is allocated zero fill on demand.  The segment's maximum virtual
 * memory protection and initial virtual memory protection are specified
 * by the maxprot and initprot fields.  If the segment has sections then the
 * section structures directly follow the segment command and their size is
 * reflected in cmdsize.
 * 段加载命令表明该文件的一部分将被映射到任务的地址空间中。
 * 该段在内存中的大小vmsize 可能等于或大于从该文件映射的数量filesize
 * 该文件从 fileoff 开始映射到内存中段 vmaddr 的开头。
 * 该段的其余内存（如果有）用0填充
 * 段的最大虚拟内存保护和初始虚拟内存保护由 maxprot 和 initprot 字段指定。 
 * 如果段有段，则段结构直接跟在段命令之后，它们的大小反映在 cmdsize 中。
 */
struct segment_command {
	unsigned long	cmd;		/* LC_SEGMENT */
	unsigned long	cmdsize;	/* includes sizeof section structs 包括section结构体的大小 */
	char		segname[16];	/* segment name 段名 */
	unsigned long	vmaddr;		/* memory address of this segment */
	unsigned long	vmsize;		/* memory size of this segment 段的内存地址*/
	unsigned long	fileoff;	/* file offset of this segment 该段的文件偏移量 */
	unsigned long	filesize;	/* amount to map from the file 文件大小 */
	vm_prot_t	maxprot;	/* maximum VM protection 最大虚拟内存保护 */
	vm_prot_t	initprot;	/* initial VM protection 初始虚拟内存保护*/
	unsigned long	nsects;		/* number of sections in segment 段中的节数 */
	unsigned long	flags;		/* flags 标记*/
};

/* Constants for the flags field of the segment_command */
/* 
* the file contents for this segment is for
* the high part of the VM space, the low part
* is zero filled (for stacks in core files) 
* 这个段的文件内容是虚拟内存空间的高部分，低部分是零填充（对于核心文件中的堆栈）
*/
#define	SG_HIGHVM	0x1
/* 
* this segment is the VM that is allocated by
* a fixed VM library, for overlap checking in
* the link editor 
* 该段是由固定VM库分配的VM，用于链接编辑器中的重叠检查
*/
#define	SG_FVMLIB	0x2	
/* 
* this segment has nothing that was relocated
* in it and nothing relocated to it, that is
* it maybe safely replaced without relocation
* 该段中没有任何重定位的内容，也没有重定位的内容，也就是说可以安全地替换而无需重定位
*/
#define	SG_NORELOC	0x4	

/*
 * A segment is made up of zero or more sections.  Non-MH_OBJECT files have
 * all of their segments with the proper sections in each, and padded to the
 * specified segment alignment when produced by the link editor.  The first
 * segment of a MH_EXECUTE and MH_FVMLIB format file contains the mach_header
 * and load commands of the object file before it's first section.  The zero
 * fill sections are always last in their segment (in all formats).  This
 * allows the zeroed segment padding to be mapped into memory where zero fill
 * sections might be.
 *
 * 一个segment由零个或多个sections组成。 
 * 非 MH_OBJECT 文件的所有segment都有适当的sections，并在链接编辑器生成时填充到指定的segment对齐方式。
 * MH_EXECUTE 和 MH_FVMLIB 格式文件的第一个segment包括mach_header以及在第一section之前的加载命令。
 * 补零section始终位于其段中的最后（在所有格式中）。 这允许将补零的段填充到补零的section里
 *
 * The MH_OBJECT format has all of it's sections in one segment for
 * compactness.  There is no padding to a specified segment boundary and the
 * mach_header and load commands are not part of the segment.
 * MH_OBJECT 格式将其所有部分放在一个段中以保持紧凑。 
 * 指定的段边界没有填充，并且 mach_header 和 load 命令不是段的一部分。
 *
 * Sections with the same section name, sectname, going into the same segment,
 * segname, are combined by the link editor.  The resulting section is aligned
 * to the maximum alignment of the combined sections and is the new section's
 * alignment.  The combined sections are aligned to their original alignment in
 * the combined section.  Any padded bytes to get the specified alignment are
 * zeroed.
 * 具有相同sectname和segname的section，会被链接编辑器组合
 * 合成的新section会按照组合section里的最大对齐方式对齐
 * 组合的section会按照原来的对齐方式对齐，字节对齐添加的字节都是0
 *
 * The format of the relocation entries referenced by the reloff and nreloc
 * fields of the section structure for mach object files is described in the
 * header file <reloc.h>.
 * mach 目标文件的节结构的 reloff 和 nreloc 字段所引用的重定位条目的格式在头文件 <reloc.h> 中描述。
 */
struct section {
	char		sectname[16];	/* name of this section section 名字*/
	char		segname[16];	/* segment this section goes in 进入这个section的段名*/
	unsigned long	addr;		/* memory address of this section 内存地址*/
	unsigned long	size;		/* size in bytes of this section 字节大小*/
	unsigned long	offset;		/* file offset of this section 文件偏移*/
	unsigned long	align;		/* section alignment (power of 2) section对齐*/
	unsigned long	reloff;		/* file offset of relocation entries 重定位条目的文件偏移量 */
	unsigned long	nreloc;		/* number of relocation entries 重定位条目数 */
	unsigned long	flags;		/* flags (section type and attributes section类型和属性)*/
	unsigned long	reserved1;	/* reserved 保留字段 */
	unsigned long	reserved2;	/* reserved */
};

/*
 * The flags field of a section structure is separated into two parts a section
 * type and section attributes.  The section types are mutually exclusive (it
 * can only have one type) but the section attributes are not (it may have more
 * than one attribute).
 * section结构的flags字段分为section类型和section属性两部分。 
 * section类型是互斥的（它只能有一种类型）但section属性不是（它可能有多个属性）。
 */
#define SECTION_TYPE		 0x000000ff	/* 256 section types */
#define SECTION_ATTRIBUTES	 0xffffff00	/*  24 section attributes */

/* Constants for the type of a section */
#define	S_REGULAR		0x0	/* regular section 常规section*/
#define	S_ZEROFILL		0x1	/* zero fill on demand section 需要补零的section*/
#define	S_CSTRING_LITERALS	0x2	/* section with only literal C strings 仅包含文字 C 字符串的部分*/
#define	S_4BYTE_LITERALS	0x3	/* section with only 4 byte literals 只有 4 个字节的部分*/
#define	S_8BYTE_LITERALS	0x4	/* section with only 8 byte literals 只有 8 个字节的部分*/
#define	S_LITERAL_POINTERS	0x5	/* section with only pointers to 只有指向的部分*/
					/*  literals */
/*
 * For the two types of symbol pointers sections and the symbol stubs section
 * they have indirect symbol table entries.  For each of the entries in the
 * section the indirect symbol table entries, in corresponding order in the
 * indirect symbol table, start at the index stored in the reserved1 field
 * of the section structure.  Since the indirect symbol table entries
 * correspond to the entries in the section the number of indirect symbol table
 * entries is inferred from the size of the section divided by the size of the
 * entries in the section.  For symbol pointers sections the size of the entries
 * in the section is 4 bytes and for symbol stubs sections the byte size of the
 * stubs is stored in the reserved2 field of the section structure.
 */
#define	S_NON_LAZY_SYMBOL_POINTERS	0x6	/* section with only non-lazy
						   symbol pointers */
#define	S_LAZY_SYMBOL_POINTERS		0x7	/* section with only lazy symbol
						   pointers */
#define	S_SYMBOL_STUBS			0x8	/* section with only symbol
						   stubs, byte size of stub in
						   the reserved2 field */
#define	S_MOD_INIT_FUNC_POINTERS	0x9	/* section with only function
						   pointers for initialization*/
/*
 * Constants for the section attributes part of the flags field of a section
 * structure.
 */
#define SECTION_ATTRIBUTES_USR	 0xff000000	/* User setable attributes */
#define S_ATTR_PURE_INSTRUCTIONS 0x80000000	/* section contains only true
						   machine instructions */
#define SECTION_ATTRIBUTES_SYS	 0x00ffff00	/* system setable attributes */
#define S_ATTR_SOME_INSTRUCTIONS 0x00000400	/* section contains some
						   machine instructions */
#define S_ATTR_EXT_RELOC	 0x00000200	/* section has external
						   relocation entries */
#define S_ATTR_LOC_RELOC	 0x00000100	/* section has local
						   relocation entries */


/*
 * The names of segments and sections in them are mostly meaningless to the
 * link-editor.  But there are few things to support traditional UNIX
 * executables that require the link-editor and assembler to use some names
 * agreed upon by convention.
 *
 * The initial protection of the "__TEXT" segment has write protection turned
 * off (not writeable).
 *
 * The link-editor will allocate common symbols at the end of the "__common"
 * section in the "__DATA" segment.  It will create the section and segment
 * if needed.
 */

/* The currently known segment names and the section names in those segments */

#define	SEG_PAGEZERO	"__PAGEZERO"	/* the pagezero segment which has no */
					/* protections and catches NULL */
					/* references for MH_EXECUTE files */


#define	SEG_TEXT	"__TEXT"	/* the tradition UNIX text segment */
#define	SECT_TEXT	"__text"	/* the real text part of the text */
					/* section no headers, and no padding */
#define SECT_FVMLIB_INIT0 "__fvmlib_init0"	/* the fvmlib initialization */
						/*  section */
#define SECT_FVMLIB_INIT1 "__fvmlib_init1"	/* the section following the */
					        /*  fvmlib initialization */
						/*  section */

#define	SEG_DATA	"__DATA"	/* the tradition UNIX data segment */
#define	SECT_DATA	"__data"	/* the real initialized data section */
					/* no padding, no bss overlap */
#define	SECT_BSS	"__bss"		/* the real uninitialized data section*/
					/* no padding */
#define SECT_COMMON	"__common"	/* the section common symbols are */
					/* allocated in by the link editor */

#define	SEG_OBJC	"__OBJC"	/* objective-C runtime segment */
#define SECT_OBJC_SYMBOLS "__symbol_table"	/* symbol table */
#define SECT_OBJC_MODULES "__module_info"	/* module information */
#define SECT_OBJC_STRINGS "__selector_strs"	/* string table */
#define SECT_OBJC_REFS "__selector_refs"	/* string table */

#define	SEG_ICON	 "__ICON"	/* the NeXT icon segment */
#define	SECT_ICON_HEADER "__header"	/* the icon headers */
#define	SECT_ICON_TIFF   "__tiff"	/* the icons in tiff format */

#define	SEG_LINKEDIT	"__LINKEDIT"	/* the segment containing all structs */
					/* created and maintained by the link */
					/* editor.  Created with -seglinkedit */
					/* option to ld(1) for MH_EXECUTE and */
					/* FVMLIB file types only */

#define SEG_UNIXSTACK	"__UNIXSTACK"	/* the unix stack segment */

/*
 * Fixed virtual memory shared libraries are identified by two things.  The
 * target pathname (the name of the library as found for execution), and the
 * minor version number.  The address of where the headers are loaded is in
 * header_addr.
 */
struct fvmlib {
	union lc_str	name;		/* library's target pathname */
	unsigned long	minor_version;	/* library's minor version number */
	unsigned long	header_addr;	/* library's header address */
};

/*
 * A fixed virtual shared library (filetype == MH_FVMLIB in the mach header)
 * contains a fvmlib_command (cmd == LC_IDFVMLIB) to identify the library.
 * An object that uses a fixed virtual shared library also contains a
 * fvmlib_command (cmd == LC_LOADFVMLIB) for each library it uses.
 */
struct fvmlib_command {
	unsigned long	cmd;		/* LC_IDFVMLIB or LC_LOADFVMLIB */
	unsigned long	cmdsize;	/* includes pathname string */
	struct fvmlib	fvmlib;		/* the library identification */
};

/*
 * Dynamicly linked shared libraries are identified by two things.  The
 * pathname (the name of the library as found for execution), and the
 * compatibility version number.  The pathname must match and the compatibility
 * number in the user of the library must be greater than or equal to the
 * library being used.  The time stamp is used to record the time a library was
 * built and copied into user so it can be use to determined if the library used
 * at runtime is exactly the same as used to built the program.
 */
struct dylib {
    union lc_str  name;			/* library's path name */
    unsigned long timestamp;		/* library's build time stamp */
    unsigned long current_version;	/* library's current version number */
    unsigned long compatibility_version;/* library's compatibility vers number*/
};

/*
 * A dynamicly linked shared library (filetype == MH_DYLIB in the mach header)
 * contains a dylib_command (cmd == LC_ID_DYLIB) to identify the library.
 * An object that uses a dynamicly linked shared library also contains a
 * dylib_command (cmd == LC_LOAD_DYLIB) for each library it uses.
 */
struct dylib_command {
	unsigned long	cmd;		/* LC_ID_DYLIB or LC_LOAD_DYLIB */
	unsigned long	cmdsize;	/* includes pathname string */
	struct dylib	dylib;		/* the library identification */
};

/*
 * A program (filetype == MH_EXECUTE) or bundle (filetype == MH_BUNDLE) that is
 * prebound to it's dynamic libraries has one of these for each library that
 * the static linker used in prebinding.  It contains a bit vector for the
 * modules in the library.  The bits indicate which modules are bound (1) and
 * which are not (0) from the library.  The bit for module 0 is the low bit
 * of the first byte.  So the bit for the Nth module is:
 * (linked_modules[N/8] >> N%8) & 1
 */
struct prebound_dylib_command {
	unsigned long	cmd;		/* LC_PREBOUND_DYLIB */
	unsigned long	cmdsize;	/* includes strings */
	union lc_str	name;		/* library's path name */
	unsigned long	nmodules;	/* number of modules in library */
	union lc_str	linked_modules;	/* bit vector of linked modules */
};

/*
 * A program that uses a dynamic linker contains a dylinker_command to identify
 * the name of the dynamic linker (LC_LOAD_DYLINKER).  And a dynamic linker
 * contains a dylinker_command to identify the dynamic linker (LC_ID_DYLINKER).
 * A file can have at most one of these.
 */
struct dylinker_command {
	unsigned long	cmd;		/* LC_ID_DYLINKER or LC_LOAD_DYLINKER */
	unsigned long	cmdsize;	/* includes pathname string */
	union lc_str    name;		/* dynamic linker's path name */
};

/*
 * Thread commands contain machine-specific data structures suitable for
 * use in the thread state primitives.  The machine specific data structures
 * follow the struct thread_command as follows.
 * Each flavor of machine specific data structure is preceded by an unsigned
 * long constant for the flavor of that data structure, an unsigned long
 * that is the count of longs of the size of the state data structure and then
 * the state data structure follows.  This triple may be repeated for many
 * flavors.  The constants for the flavors, counts and state data structure
 * definitions are expected to be in the header file <machine/thread_status.h>.
 * These machine specific data structures sizes must be multiples of
 * sizeof(long).  The cmdsize reflects the total size of the thread_command
 * and all of the sizes of the constants for the flavors, counts and state
 * data structures.
 *
 * For executable objects that are unix processes there will be one
 * thread_command (cmd == LC_UNIXTHREAD) created for it by the link-editor.
 * This is the same as a LC_THREAD, except that a stack is automatically
 * created (based on the shell's limit for the stack size).  Command arguments
 * and environment variables are copied onto that stack.
 */
struct thread_command {
	unsigned long	cmd;		/* LC_THREAD or  LC_UNIXTHREAD */
	unsigned long	cmdsize;	/* total size of this command */
	/* unsigned long flavor		   flavor of thread state */
	/* unsigned long count		   count of longs in thread state */
	/* struct XXX_thread_state state   thread state for this flavor */
	/* ... */
};

/*
 * The symtab_command contains the offsets and sizes of the link-edit 4.3BSD
 * "stab" style symbol table information as described in the header files
 * <nlist.h> and <stab.h>.
 */
struct symtab_command {
	unsigned long	cmd;		/* LC_SYMTAB */
	unsigned long	cmdsize;	/* sizeof(struct symtab_command) */
	unsigned long	symoff;		/* symbol table offset */
	unsigned long	nsyms;		/* number of symbol table entries */
	unsigned long	stroff;		/* string table offset */
	unsigned long	strsize;	/* string table size in bytes */
};

/*
 * This is the second set of the symbolic information which is used to support
 * the data structures for the dynamicly link editor.
 *
 * The original set of symbolic information in the symtab_command which contains
 * the symbol and string tables must also be present when this load command is
 * present.  When this load command is present the symbol table is organized
 * into three groups of symbols:
 *	local symbols (static and debugging symbols) - grouped by module
 *	defined external symbols - grouped by module (sorted by name if not lib)
 *	undefined external symbols (sorted by name)
 * In this load command there are offsets and counts to each of the three groups
 * of symbols.
 *
 * This load command contains a the offsets and sizes of the following new
 * symbolic information tables:
 *	table of contents
 *	module table
 *	reference symbol table
 *	indirect symbol table
 * The first three tables above (the table of contents, module table and
 * reference symbol table) are only present if the file is a dynamicly linked
 * shared library.  For executable and object modules, which are files
 * containing only one module, the information that would be in these three
 * tables is determined as follows:
 * 	table of contents - the defined external symbols are sorted by name
 *	module table - the file contains only one module so everything in the
 *		       file is part of the module.
 *	reference symbol table - is the defined and undefined external symbols
 *
 * For dynamicly linked shared library files this load command also contains
 * offsets and sizes to the pool of relocation entries for all sections
 * separated into two groups:
 *	external relocation entries
 *	local relocation entries
 * For executable and object modules the relocation entries continue to hang
 * off the section structures.
 */
struct dysymtab_command {
    unsigned long cmd;		/* LC_DYSYMTAB */
    unsigned long cmdsize;	/* sizeof(struct dysymtab_command) */

    /*
     * The symbols indicated by symoff and nsyms of the LC_SYMTAB load command
     * are grouped into the following three groups:
     *    local symbols (further grouped by the module they are from)
     *    defined external symbols (further grouped by the module they are from)
     *    undefined symbols
     *
     * The local symbols are used only for debugging.  The dynamic binding
     * process may have to use them to indicate to the debugger the local
     * symbols for a module that is being bound.
     *
     * The last two groups are used by the dynamic binding process to do the
     * binding (indirectly through the module table and the reference symbol
     * table when this is a dynamicly linked shared library file).
     */
    unsigned long ilocalsym;	/* index to local symbols */
    unsigned long nlocalsym;	/* number of local symbols */

    unsigned long iextdefsym;	/* index to externally defined symbols */
    unsigned long nextdefsym;	/* number of externally defined symbols */

    unsigned long iundefsym;	/* index to undefined symbols */
    unsigned long nundefsym;	/* number of undefined symbols */

    /*
     * For the for the dynamic binding process to find which module a symbol
     * is defined in the table of contents is used (analogous to the ranlib
     * structure in an archive) which maps defined external symbols to modules
     * they are defined in.  This exists only in a dynamicly linked shared
     * library file.  For executable and object modules the defined external
     * symbols are sorted by name and is use as the table of contents.
     */
    unsigned long tocoff;	/* file offset to table of contents */
    unsigned long ntoc;		/* number of entries in table of contents */

    /*
     * To support dynamic binding of "modules" (whole object files) the symbol
     * table must reflect the modules that the file was created from.  This is
     * done by having a module table that has indexes and counts into the merged
     * tables for each module.  The module structure that these two entries
     * refer to is described below.  This exists only in a dynamicly linked
     * shared library file.  For executable and object modules the file only
     * contains one module so everything in the file belongs to the module.
     */
    unsigned long modtaboff;	/* file offset to module table */
    unsigned long nmodtab;	/* number of module table entries */

    /*
     * To support dynamic module binding the module structure for each module
     * indicates the external references (defined and undefined) each module
     * makes.  For each module there is an offset and a count into the
     * reference symbol table for the symbols that the module references.
     * This exists only in a dynamicly linked shared library file.  For
     * executable and object modules the defined external symbols and the
     * undefined external symbols indicates the external references.
     */
    unsigned long extrefsymoff;  /* offset to referenced symbol table */
    unsigned long nextrefsyms;	 /* number of referenced symbol table entries */

    /*
     * The sections that contain "symbol pointers" and "routine stubs" have
     * indexes and (implied counts based on the size of the section and fixed
     * size of the entry) into the "indirect symbol" table for each pointer
     * and stub.  For every section of these two types the index into the
     * indirect symbol table is stored in the section header in the field
     * reserved1.  An indirect symbol table entry is simply a 32bit index into
     * the symbol table to the symbol that the pointer or stub is referring to.
     * The indirect symbol table is ordered to match the entries in the section.
     */
    unsigned long indirectsymoff; /* file offset to the indirect symbol table */
    unsigned long nindirectsyms;  /* number of indirect symbol table entries */

    /*
     * To support relocating an individual module in a library file quickly the
     * external relocation entries for each module in the library need to be
     * accessed efficiently.  Since the relocation entries can't be accessed
     * through the section headers for a library file they are separated into
     * groups of local and external entries further grouped by module.  In this
     * case the presents of this load command who's extreloff, nextrel,
     * locreloff and nlocrel fields are non-zero indicates that the relocation
     * entries of non-merged sections are not referenced through the section
     * structures (and the reloff and nreloc fields in the section headers are
     * set to zero).
     *
     * Since the relocation entries are not accessed through the section headers
     * this requires the r_address field to be something other than a section
     * offset to identify the item to be relocated.  In this case r_address is
     * set to the offset from the vmaddr of the first LC_SEGMENT command.
     *
     * The relocation entries are grouped by module and the module table
     * entries have indexes and counts into them for the group of external
     * relocation entries for that the module.
     *
     * For sections that are merged across modules there must not be any
     * remaining external relocation entries for them (for merged sections
     * remaining relocation entries must be local).
     */
    unsigned long extreloff;	/* offset to external relocation entries */
    unsigned long nextrel;	/* number of external relocation entries */

    /*
     * All the local relocation entries are grouped together (they are not
     * grouped by their module since they are only used if the object is moved
     * from it staticly link edited address).
     */
    unsigned long locreloff;	/* offset to local relocation entries */
    unsigned long nlocrel;	/* number of local relocation entries */

};	

/*
 * An indirect symbol table entry is simply a 32bit index into the symbol table 
 * to the symbol that the pointer or stub is refering to.  Unless it is for a
 * non-lazy symbol pointer section for a defined symbol which strip(1) as 
 * removed.  In which case it has the value INDIRECT_SYMBOL_LOCAL.  If the
 * symbol was also absolute INDIRECT_SYMBOL_ABS is or'ed with that.
 */
#define INDIRECT_SYMBOL_LOCAL	0x80000000
#define INDIRECT_SYMBOL_ABS	0x40000000


/* a table of contents entry */
struct dylib_table_of_contents {
    unsigned long symbol_index;	/* the defined external symbol
				   (index into the symbol table) */
    unsigned long module_index;	/* index into the module table this symbol
				   is defined in */
};	

/* a module table entry */
struct dylib_module {
    unsigned long module_name;	/* the module name (index into string table) */

    unsigned long iextdefsym;	/* index into externally defined symbols */
    unsigned long nextdefsym;	/* number of externally defined symbols */
    unsigned long irefsym;		/* index into reference symbol table */
    unsigned long nrefsym;	/* number of reference symbol table entries */
    unsigned long ilocalsym;	/* index into symbols for local symbols */
    unsigned long nlocalsym;	/* number of local symbols */

    unsigned long iextrel;	/* index into external relocation entries */
    unsigned long nextrel;	/* number of external relocation entries */

    unsigned long iinit;	/* index into the init section */
    unsigned long ninit;	/* number of init section entries */

    unsigned long		/* for this module address of the start of */
	objc_module_info_addr;  /*  the (__OBJC,__module_info) section */
    unsigned long		/* for this module size of */
	objc_module_info_size;	/*  the (__OBJC,__module_info) section */
};	

/* 
 * The entries in the reference symbol table are used when loading the module
 * (both by the static and dynamic link editors) and if the module is unloaded
 * or replaced.  Therefore all external symbols (defined and undefined) are
 * listed in the module's reference table.  The flags describe the type of
 * reference that is being made.  The constants for the flags are defined in
 * <mach-o/nlist.h> as they are also used for symbol table entries.
 */
struct dylib_reference {
    unsigned long isym:24,	/* index into the symbol table */
    		  flags:8;	/* flags to indicate the type of reference */
};

/*
 * The symseg_command contains the offset and size of the GNU style
 * symbol table information as described in the header file <symseg.h>.
 * The symbol roots of the symbol segments must also be aligned properly
 * in the file.  So the requirement of keeping the offsets aligned to a
 * multiple of a sizeof(long) translates to the length field of the symbol
 * roots also being a multiple of a long.  Also the padding must again be
 * zeroed. (THIS IS OBSOLETE and no longer supported).
 */
struct symseg_command {
	unsigned long	cmd;		/* LC_SYMSEG */
	unsigned long	cmdsize;	/* sizeof(struct symseg_command) */
	unsigned long	offset;		/* symbol segment offset */
	unsigned long	size;		/* symbol segment size in bytes */
};

/*
 * The ident_command contains a free format string table following the
 * ident_command structure.  The strings are null terminated and the size of
 * the command is padded out with zero bytes to a multiple of sizeof(long).
 * (THIS IS OBSOLETE and no longer supported).
 */
struct ident_command {
	unsigned long cmd;		/* LC_IDENT */
	unsigned long cmdsize;		/* strings that follow this command */
};

/*
 * The fvmfile_command contains a reference to a file to be loaded at the
 * specified virtual address.  (Presently, this command is reserved for NeXT
 * internal use.  The kernel ignores this command when loading a program into
 * memory).
 */
struct fvmfile_command {
	unsigned long cmd;		/* LC_FVMFILE */
	unsigned long cmdsize;		/* includes pathname string */
	union lc_str	name;		/* files pathname */
	unsigned long	header_addr;	/* files virtual address */
};

#endif _MACHO_LOADER_H_
