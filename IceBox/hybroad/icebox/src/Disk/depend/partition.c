/*
 * sfdisk version 3.0 - aeb - 950813
 *
 * Copyright (C) 1995  Andries E. Brouwer (aeb@cwi.nl)
 *
 * This program is free software. You can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either Version 1
 * or (at your option) any later version.
 *
 * A.V. Le Blanc (LeBlanc@mcc.ac.uk) wrote Linux fdisk 1992-1994,
 * patched by various people (faith@cs.unc.edu, martin@cs.unc.edu,
 * leisner@sdsp.mc.xerox.com, esr@snark.thyrsus.com, aeb@cwi.nl)
 * 1993-1995, with version numbers (as far as I have seen) 0.93 - 2.0e.
 * This program had (head,sector,cylinder) as basic unit, and was
 * (therefore) broken in several ways for the use on larger disks -
 * for example, my last patch (from 2.0d to 2.0e) was required
 * to allow a partition to cross cylinder 8064, and to write an
 * extended partition past the 4GB mark.
 *
 * The current program is a rewrite from scratch, and I started a
 * version numbering at 3.0.
 * 	Andries Brouwer, aeb@cwi.nl, 950813
 *
 * Well, a good user interface is still lacking. On the other hand,
 * many configurations cannot be handled by any other fdisk.
 * I changed the name to sfdisk to prevent confusion. - aeb, 970501
 *
 * Changes:
 * 19990319 - Arnaldo Carvalho de Melo <acme@conectiva.com.br> - i18n
 */

#define PROGNAME "sfdisk"
#define VERSION "3.07"
#define DATE "990908"

#include <stdio.h>
#include <stdlib.h>		/* atoi, free */
#include <stdarg.h>		/* varargs */
#include <unistd.h>		/* read, write */
#include <fcntl.h>		/* O_RDWR */
#include <errno.h>		/* ERANGE */
#include <string.h>		/* index() */
#include <ctype.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <linux/unistd.h>	/* _syscall */
#include <linux/hdreg.h>	/* HDIO_GETGEO */
#include "nls.h"
#include "common.h"

#include "Assertions.h"

#define IND_FREE free
#define IND_MALLOC malloc

//#include "diskm.h"
#define OUTPUT_LEVEL LOG_ERROR//LOG_NONE//LOG_ERROR

#define SIZE(a)	(sizeof(a)/sizeof(a[0]))

static int countOfPartition = 0;
struct Part_Info
{
    unsigned char partitionInput[11][11];
}partitionInputInfo[4];
/*
 * Table of contents:
 *  A. About seeking
 *  B. About sectors
 *  C. About heads, sectors and cylinders
 *  D. About system Ids
 *  E. About partitions
 *  F. The standard input
 *  G. The command line
 *  H. Listing the current situation
 *  I. Writing the new situation
 */
int exit_status = 0;

int force = 0;		/* 1: do what I say, even if it is stupid ... */
static int quiet = 0;		/* 1: suppress all warnings */
/* IA-64 gcc spec file currently does -DLinux... */
#undef Linux
int Linux = 0;		/* 1: suppress warnings irrelevant for Linux */
int DOS = 0;		/* 1: shift extended partitions by #sectors, not 1 */
int DOS_extended = 0;	/* 1: use starting cylinder boundary of extd partn */
int dump = 0;           /* 1: list in a format suitable for later input */
int verify = 0;         /* 1: check that listed partition is reasonable */
int no_write = 0;	/* 1: do not actually write to disk */
int no_reread = 0;	/* 1: skip the BLKRRPART ioctl test at startup */
int leave_last = 0;	/* 1: don't allocate the last cylinder */
int opt_list = 0;
char *save_sector_file = NULL;
char *restore_sector_file = NULL;

static void
warn(char *s, ...) {
    va_list p;

    va_start(p, s);
    fflush(stdout);
    if (!quiet)
        vfprintf (stderr, s, p);
    va_end(p);
}

static void
error(char *s, ...) {
    va_list p;

    va_start(p, s);
    fflush(stdout);
    fprintf(stderr, "\n" PROGNAME ": ");
    vfprintf(stderr, s, p);
    va_end(p);
}

static void
fatal(char *s, ...) {
    va_list p;

    va_start(p, s);
    fflush(stdout);
    fprintf(stderr, "\n" PROGNAME ": ");
    vfprintf(stderr, s, p);
    va_end(p);
    return;
}

/*
 *  A. About seeking
 */

/*
 * sseek: seek to specified sector - return 0 on failure
 *
 * For >4GB disks lseek needs a > 32bit arg, and we have to use llseek.
 * On the other hand, a 32 bit sector number is OK until 2TB.
 * The routines _llseek and sseek below are the only ones that
 * know about the loff_t type.
 *
 * Note: we use 512-byte sectors here, irrespective of the hardware ss.
 */
#if 0
#if !defined (__alpha__) && !defined (__ia64__)
static
_syscall5(int,  _llseek,  uint32_t,  fd, ulong, hi, ulong, lo,
    loff_t *, res, uint32_t, wh);
#endif
#endif

static int
sseek(char *dev, unsigned int fd, unsigned long s) {
    loff_t in, out;
    in = ((loff_t) s << 9);
    out = 1;

    //#if !defined (__alpha__) && !defined (__ia64__)
    //    if (_llseek (fd, in>>32, in & 0xffffffff, &out, SEEK_SET) != 0) {
    //#else
    if ((out = lseek(fd, in, SEEK_SET)) != in) {
        //#endif
        perror("llseek");
        error(_("seek error on %s - cannot seek to %lu\n"), dev, s);
        return 0;
    }

    if (in != out) {
        error(_("seek error: wanted 0x%08x%08x, got 0x%08x%08x\n"),
            (uint32_t)(in>>32), (uint32_t)(in & 0xffffffff),
            (uint32_t)(out>>32), (uint32_t)(out & 0xffffffff));
        return 0;
    }
    return 1;
}

/*
 *  B. About sectors
 */

/*
 * We preserve all sectors read in a chain - some of these will
 * have to be modified and written back.
 */
struct sector {
    struct sector *next;
    unsigned long sectornumber;
    int to_be_written;
    char data[512];
} *sectorhead;

static void
free_sectors(void) {
    struct sector *s;

    while (sectorhead) {
        s = sectorhead;
        sectorhead = s->next;
        IND_FREE(s);
    }
}

static struct sector *
get_sector(char *dev, int fd, unsigned long sno) {
    struct sector *s;

    for(s = sectorhead; s; s = s->next)
        if(s->sectornumber == sno)
            return s;

    if (!sseek(dev, fd, sno))
        return 0;

    if (!(s = (struct sector *) IND_MALLOC(sizeof(struct sector))))
        fatal(_("out of memory - giving up\n"));

    if (read(fd, s->data, sizeof(s->data)) != sizeof(s->data)) {
        if (errno)		/* 0 in case we read past end-of-disk */
            perror("read");
        error(_("read error on %s - cannot read sector %lu\n"), dev, sno);
        IND_FREE(s);
        return 0;
    }

    s->next = sectorhead;
    sectorhead = s;
    s->sectornumber = sno;
    s->to_be_written = 0;

    return s;
}

static int
msdos_signature (struct sector *s) {
    if (*(unsigned short *) (s->data + 0x1fe) != 0xaa55) {
        error(_("ERROR: sector %lu does not have an msdos signature\n"),
            s->sectornumber);
        return 0;
    }
    return 1;
}

static int
write_sectors(char *dev, int fd) {
    struct sector *s;

    for (s = sectorhead; s; s = s->next)
        if (s->to_be_written) {
            if (!sseek(dev, fd, s->sectornumber))
                return 0;
            if (write(fd, s->data, sizeof(s->data)) != sizeof(s->data)) {
                perror("write");
                error(_("write error on %s - cannot write sector %lu\n"),
                    dev, s->sectornumber);
                return 0;
            }
            s->to_be_written = 0;
        }
    return 1;
}

static void
ulong_to_chars(unsigned long u, char *uu) {
    int i;

    for(i=0; i<4; i++) {
        uu[i] = (u & 0xff);
        u >>= 8;
    }
}


static int
save_sectors(char *dev, int fdin) {
    struct sector *s;
    char ss[516];
    int fdout;

    fdout = open(save_sector_file, O_WRONLY | O_CREAT, 0664);
    if (fdout < 0) {
        perror(save_sector_file);
        error(_("cannot open partition sector save file (%s)\n"),
            save_sector_file);
        return 0;
    }

    for (s = sectorhead; s; s = s->next)
        if (s->to_be_written) {
            ulong_to_chars(s->sectornumber, ss);
            if (!sseek(dev, fdin, s->sectornumber))
                return 0;
            if (read(fdin, ss+4, 512) != 512) {
                perror("read");
                error(_("read error on %s - cannot read sector %lu\n"),
                    dev, s->sectornumber);
                return 0;
            }
            if (write(fdout, ss, sizeof(ss)) != sizeof(ss)) {
                perror("write");
                error(_("write error on %s\n"), save_sector_file);
                return 0;
            }
        }
    return 1;
}

static void reread_disk_partition(char *dev, int fd);


/*
 *  C. About heads, sectors and cylinders
 */

/*
 * <linux/hdreg.h> defines HDIO_GETGEO and
 * struct hd_geometry {
 *      unsigned char heads;
 *      unsigned char sectors;
 *      unsigned short cylinders;
 *      unsigned long start;
 * };
 *
 * For large disks g.cylinders is truncated, so we use BLKGETSIZE.
 */

/*
 * We consider several geometries for a disk:
 * B - the BIOS geometry, gotten from the kernel via HDIO_GETGEO
 * F - the fdisk geometry
 * U - the user-specified geometry
 *
 * 0 means unspecified / unknown
 */
struct geometry {
    unsigned long cylindersize;
    unsigned long heads, sectors, cylinders;
    unsigned long start;
} B, F, U;

static struct geometry
get_geometry(char *dev, int fd, int silent) {
    struct hd_geometry g;
    long size;
    struct geometry R;

    if (ioctl(fd, BLKGETSIZE, &size)) {
        size = 0;
        if (!silent)
            SYSTEM_LOG(_("Disk %s: cannot get size\n"), dev);
    }
    if (ioctl(fd, HDIO_GETGEO, &g)) {
        g.heads = g.sectors = g.cylinders = g.start = 0;
        if (!silent)
            SYSTEM_LOG(_("Disk %s: cannot get geometry\n"), dev);
    }
    R.heads = g.heads;
    R.sectors = g.sectors;
    R.cylindersize = R.heads * R.sectors;
    R.cylinders = (R.cylindersize ? size / R.cylindersize : 0);
    R.start = g.start;
    return R;
}

static void
get_cylindersize(char *dev, int fd, int silent) {
    struct geometry R;

    R = get_geometry(dev, fd, silent);

    B.heads = (U.heads ? U.heads : R.heads);
    B.sectors = (U.sectors ? U.sectors : R.sectors);
    B.cylinders = (U.cylinders ? U.cylinders : R.cylinders);

    B.cylindersize = B.heads * B.sectors;

    if (R.start && !force) {
        warn(
            _("Warning: start=%d - this looks like a partition rather than\n"
                "the entire disk. Using fdisk on it is probably meaningless.\n"
                "[Use the --force option if you really want this]\n"), R.start);
        exit(1);
    }

    if (R.heads && B.heads != R.heads)
        warn(_("Warning: HDIO_GETGEO says that there are %d heads\n"),
            R.heads);
    if (R.sectors && B.sectors != R.sectors)
        warn(_("Warning: HDIO_GETGEO says that there are %d sectors\n"),
            R.sectors);
    if (R.cylinders && B.cylinders != R.cylinders
        && B.cylinders < 65536 && R.cylinders < 65536)
        warn(_("Warning: BLKGETSIZE/HDIO_GETGEO says that there are %d cylinders\n"),
            R.cylinders);

    if (B.sectors > 63)
        warn(_("Warning: unlikely number of sectors (%d) - usually at most 63\n"
                "This will give problems with all software that uses C/H/S addressing.\n"),
            B.sectors);
    if (!silent)
        SYSTEM_LOG(_("\nDisk %s: %lu cylinders, %lu heads, %lu sectors/track\n"),
            dev, B.cylinders, B.heads, B.sectors);
}

typedef struct { unsigned char h,s,c; } chs; /* has some c bits in s */
chs zero_chs = { 0,0,0 };

typedef struct { unsigned long h,s,c; } longchs;
longchs zero_longchs;

static chs
longchs_to_chs (longchs aa, struct geometry G) {
    chs a;

    if (aa.h < 256 && aa.s < 64 && aa.c < 1024) {
        a.h = aa.h;
        a.s = aa.s | ((aa.c >> 2) & 0xc0);
        a.c = (aa.c & 0xff);
    } else if (G.heads && G.sectors) {
        a.h = G.heads - 1;
        a.s = G.sectors | 0xc0;
        a.c = 0xff;
    } else
        a = zero_chs;
    return a;
}

static longchs
chs_to_longchs (chs a) {
    longchs aa;

    aa.h = a.h;
    aa.s = (a.s & 0x3f);
    aa.c = (a.s & 0xc0);
    aa.c = (aa.c << 2) + a.c;
    return aa;
}

static longchs
ulong_to_longchs (unsigned long sno, struct geometry G) {
    longchs aa;

    if (G.heads && G.sectors && G.cylindersize) {
        aa.s = 1 + sno % G.sectors;
        aa.h = (sno / G.sectors) % G.heads;
        aa.c = sno / G.cylindersize;
        return aa;
    } else {
        return zero_longchs;
    }
}

static chs
ulong_to_chs (unsigned long sno, struct geometry G) {
    return longchs_to_chs(ulong_to_longchs(sno, G), G);
}

#if 0
static unsigned long
longchs_to_ulong (longchs aa, struct geometry G) {
    return (aa.c*G.cylindersize + aa.h*G.sectors + aa.s - 1);
}

static unsigned long
chs_to_ulong (chs a, struct geometry G) {
    return longchs_to_ulong(chs_to_longchs(a), G);
}
#endif

static int
is_equal_chs (chs a, chs b) {
    return (a.h == b.h && a.s == b.s && a.c == b.c);
}

static int
chs_ok (chs a, char *v, char *w) {
    longchs aa = chs_to_longchs(a);
    int ret = 1;

    if (is_equal_chs(a, zero_chs))
        return 1;
    if (B.heads && aa.h >= B.heads) {
        warn(_("%s of partition %s has impossible value for head: "
                "%d (should be in 0-%d)\n"), w, v, aa.h, B.heads-1);
        ret = 0;
    }
    if (B.sectors && (aa.s == 0 || aa.s > B.sectors)) {
        warn(_("%s of partition %s has impossible value for sector: "
                "%d (should be in 1-%d)\n"), w, v, aa.s, B.sectors);
        ret = 0;
    }
    if (B.cylinders && aa.c >= B.cylinders) {
        warn(_("%s of partition %s has impossible value for cylinders: "
                "%d (should be in 0-%d)\n"), w, v, aa.c, B.cylinders-1);
        ret = 0;
    }
    return ret;
}

/*
 *  D. About system Ids
 */

#define EMPTY_PARTITION		0
#define EXTENDED_PARTITION	5
#define WIN98_EXTENDED		0x0f
#define WIN95_FAT32 	 	0x0c
#define DM6_AUX1PARTITION	0x51
#define DM6_AUX3PARTITION	0x53
#define DM6_PARTITION		0x54
#define EZD_PARTITION		0x55
#define LINUX_SWAP              0x82
#define LINUX_NATIVE	        0x83
#define LINUX_EXTENDED		0x85
#define BSD_PARTITION		0xa5
static int Yx_sys_type=LINUX_NATIVE;
/* List of partition types now in i386_sys_types.c */

static const char *
sysname(unsigned char type) {
    struct systypes *s;

    for (s = i386_sys_types; s->name; s++)
        if (s->type == type)
            return _(s->name);
    return _("Unknown");
}


static int
is_extended(unsigned char type) {
    return (type == EXTENDED_PARTITION
        || type == LINUX_EXTENDED
        || type == WIN98_EXTENDED);
}

static int
is_bsd(unsigned char type) {
    return (type == BSD_PARTITION);
}

/*
 *  E. About partitions
 */

/* MS/DOS partition */

struct partition {
    unsigned char bootable;		/* 0 or 0x80 */
    chs begin_chs;
    unsigned char sys_type;
    chs end_chs;
    unsigned int start_sect;	/* starting sector counting from 0 */
    unsigned int nr_sects;		/* nr of sectors in partition */
};

/* Unfortunately, partitions are not aligned, and non-Intel machines
   are unhappy with non-aligned integers. So, we need a copy by hand. */
static int
copy_to_int(unsigned char *cp) {
    unsigned int m;

    m = *cp++;
    m += (*cp++ << 8);
    m += (*cp++ << 16);
    m += (*cp++ << 24);
    return m;
}

static void
copy_from_int(int m, char *cp) {
    *cp++ = (m & 0xff); m >>= 8;
    *cp++ = (m & 0xff); m >>= 8;
    *cp++ = (m & 0xff); m >>= 8;
    *cp++ = (m & 0xff);
}

static void
copy_to_part(char *cp, struct partition *p) {
    p->bootable = *cp++;
    p->begin_chs.h = *cp++;
    p->begin_chs.s = *cp++;
    p->begin_chs.c = *cp++;
    p->sys_type = *cp++;
    p->end_chs.h = *cp++;
    p->end_chs.s = *cp++;
    p->end_chs.c = *cp++;
    p->start_sect = copy_to_int((unsigned char *)cp);
    p->nr_sects = copy_to_int((unsigned char *)(cp+4));
}

static void
copy_from_part(struct partition *p, char *cp) {
    *cp++ = p->bootable;
    *cp++ = p->begin_chs.h;
    *cp++ = p->begin_chs.s;
    *cp++ = p->begin_chs.c;
    *cp++ = p->sys_type;
    *cp++ = p->end_chs.h;
    *cp++ = p->end_chs.s;
    *cp++ = p->end_chs.c;
    copy_from_int(p->start_sect, cp);
    copy_from_int(p->nr_sects, cp+4);
}

/* Roughly speaking, Linux doesn't use any of the above fields except
   for partition type, start sector and number of sectors. (However,
   see also linux/drivers/scsi/fdomain.c.)
   The only way partition type is used (in the kernel) is the comparison
   for equality with EXTENDED_PARTITION (and these Disk Manager types). */

struct part_desc {
    unsigned long start;
    unsigned long size;
    unsigned long sector, offset; /* disk location of this info */
    struct partition p;
    struct part_desc *ep;	  /* extended partition containing this one */
    int ptype;
#define DOS_TYPE	0
#define BSD_TYPE	1
} zero_part_desc;

static struct part_desc *
outer_extended_partition(struct part_desc *p) {
    while (p->ep)
        p = p->ep;
    return p;
}

static int
is_parent(struct part_desc *pp, struct part_desc *p) {
    while (p) {
        if (pp == p)
            return 1;
        p = p->ep;
    }
    return 0;
}

struct disk_desc {
    struct part_desc partitions[128];
    int partno;
} oldp, newp;

/* determine where on the disk this information goes */
static void
add_sector_and_offset(struct disk_desc *z) {
    int pno;
    struct part_desc *p;

    for (pno = 0; pno < z->partno; pno++) {
        p = &(z->partitions[pno]);
        p->offset = 0x1be + (pno%4)*sizeof(struct partition);
        p->sector = (p->ep ? p->ep->start : 0);
    }
}

/* tell the kernel to reread the partition tables */
static int
reread_ioctl(int fd) {
    if(ioctl(fd, BLKRRPART)) {
        perror("BLKRRPART");
        return -1;
    }
    return 0;
}

static int
is_blockdev(int fd) {
    struct stat statbuf;

    return(fstat(fd, &statbuf) == 0 && S_ISBLK(statbuf.st_mode));
}

/* reread after writing */
static void
reread_disk_partition(char *dev, int fd) {
    SYSTEM_LOG(_("Re-reading the partition table ...\n"));
    fflush(stdout);
    sync();
    sleep(3);			/* superfluous since 1.3.20 */

    if(reread_ioctl(fd) && is_blockdev(fd))
        SYSTEM_LOG(_("The command to re-read the partition table failed\n"
                "Reboot your system now, before using mkfs\n"));

    if (close(fd)) {
        perror(dev);
        SYSTEM_LOG(_("Error closing %s\n"), dev);
    }
    SYSTEM_LOG("\n");
}

/* find Linux name of this partition, assuming that it will have a name */
static int
index_to_linux(int pno, struct disk_desc *z) {
    int i, ct = 1;
    struct part_desc *p = &(z->partitions[0]);
    for (i=0; i<pno; i++,p++)
        if(i < 4 || (p->size > 0 && !is_extended(p->p.sys_type)))
            ct++;
    return ct;
}

static int
linux_to_index(int lpno, struct disk_desc *z) {
    int i, ct = 0;
    struct part_desc *p = &(z->partitions[0]);
    for (i=0; i<z->partno && ct < lpno; i++,p++)
        if((i < 4 || (p->size > 0 && !is_extended(p->p.sys_type)))
            && ++ct == lpno)
            return i;
    return -1;
}


/*
 * List partitions - in terms of sectors, blocks or cylinders
 */
#define F_SECTOR   1
#define F_BLOCK    2
#define F_CYLINDER 3
#define F_MEGABYTE 4

int default_format = F_MEGABYTE;
int specified_format = 0;
int show_extended = 0;
int one_only = 0;
int one_only_pno;
int increment = 0;


static unsigned long
unitsize(int format) {
    default_format = (B.cylindersize ? F_CYLINDER : F_MEGABYTE);
    if (!format && !(format = specified_format))
        format = default_format;

    switch(format) {
    default:
    case F_CYLINDER:
        if(B.cylindersize)
            return B.cylindersize;
    case F_SECTOR:
        return 1;
    case F_BLOCK:
        return 2;
    case F_MEGABYTE:
        return 2048;
    }
}

static unsigned long
get_disksize(int format) {
    unsigned long cs = B.cylinders;
    if (cs && leave_last)
        cs--;
    return (cs * B.cylindersize) / unitsize(format);
}

static void
out_partition_header(char *dev, int format, struct geometry G) {
    if (dump) {
        SYSTEM_LOG(_("# partition table of %s\n"), dev);
        SYSTEM_LOG("unit: sectors\n\n");
        return;
    }

    default_format = (G.cylindersize ? F_CYLINDER : F_MEGABYTE);
    if (!format && !(format = specified_format))
        format = default_format;

    switch(format) {
    default:
        SYSTEM_LOG(_("unimplemented format - using %s\n"),
            G.cylindersize ? _("cylinders") : _("sectors"));
    case F_CYLINDER:
        if (G.cylindersize) {
            SYSTEM_LOG(_("Units = cylinders of %lu bytes, blocks of 1024 bytes"
                    ", counting from %d\n\n"),
                G.cylindersize<<9, increment);
            SYSTEM_LOG(_("   Device Boot Start     End   #cyls   #blocks   Id  System\n"));
            break;
        }
        /* fall through */
    case F_SECTOR:
        SYSTEM_LOG(_("Units = sectors of 512 bytes, counting from %d\n\n"),
            increment);
        SYSTEM_LOG(_("   Device Boot    Start       End  #sectors  Id  System\n"));
        break;
    case F_BLOCK:
        SYSTEM_LOG(_("Units = blocks of 1024 bytes, counting from %d\n\n"),
            increment);
        SYSTEM_LOG(_("   Device Boot   Start       End   #blocks   Id  System\n"));
        break;
    case F_MEGABYTE:
        SYSTEM_LOG(_("Units = megabytes of 1048576 bytes, blocks of 1024 bytes"
                ", counting from %d\n\n"), increment);
        SYSTEM_LOG(_("   Device Boot Start   End     MB   #blocks   Id  System\n"));
        break;
    }
}

static void
out_rounddown(int width, unsigned long n, unsigned long unit, int inc) {
    SYSTEM_LOG("%*lu", width, inc + n/unit);
    if (unit != 1)
        putchar((n % unit) ? '+' : ' ');
    putchar(' ');
}

static void
out_roundup(int width, unsigned long n, unsigned long unit, int inc) {
    if (n == (unsigned long)(-1))
        SYSTEM_LOG("%*s", width, "-");
    else
        SYSTEM_LOG("%*lu", width, inc + n/unit);
    if (unit != 1)
        putchar(((n+1) % unit) ? '-' : ' ');
    putchar(' ');
}

static void
out_roundup_size(int width, unsigned long n, unsigned long unit) {
    SYSTEM_LOG("%*lu", width, (n+unit-1)/unit);
    if (unit != 1)
        putchar((n % unit) ? '-' : ' ');
    putchar(' ');
}

static int
get_fdisk_geometry(struct part_desc *p) {
    chs b = p->p.end_chs;
    longchs bb = chs_to_longchs(b);
    F.heads = bb.h+1;
    F.sectors = bb.s;
    F.cylindersize = F.heads*F.sectors;
    return (F.sectors != B.sectors || F.heads != B.heads);
}

static void
out_partition(char *dev, int format, struct part_desc *p,
    struct disk_desc *z, struct geometry G) {
    unsigned long start, end, size;
    int pno, lpno;

    if (!format && !(format = specified_format))
        format = default_format;

    pno = p - &(z->partitions[0]); 	/* our index */
    lpno = index_to_linux(pno, z); 	/* name of next one that has a name */
    if(pno == linux_to_index(lpno, z)) {  /* was that us? */
        printf("%s", partname(dev, lpno, 10));  /* yes */
    } else if(show_extended)
        printf("    -     ");
    else
        return;
    putchar(dump ? ':' : ' ');

    start = p->start;
    end = p->start + p->size - 1;
    size = p->size;

    if (dump) {
        printf(" start=%9lu", start);
        printf(", size=%8lu", size);
        if (p->ptype == DOS_TYPE) {
            printf(", Id=%2x", p->p.sys_type);
            if (p->p.bootable == 0x80)
                SYSTEM_LOG(", bootable");
        }
        printf("\n");
        return;
    }

    if(p->ptype != DOS_TYPE || p->p.bootable == 0)
        printf("   ");
    else if(p->p.bootable == 0x80)
        printf(" * ");
    else
        printf(" ? ");		/* garbage */

    switch(format) {
    case F_CYLINDER:
        if (G.cylindersize) {
            out_rounddown(6, start, G.cylindersize, increment);
            out_roundup(6, end, G.cylindersize, increment);
            out_roundup_size(6, size, G.cylindersize);
            out_rounddown(8, size, 2, 0);
            break;
        }
        /* fall through */
    default:
    case F_SECTOR:
        out_rounddown(9, start, 1, increment);
        out_roundup(9, end, 1, increment);
        out_rounddown(9, size, 1, 0);
        break;
    case F_BLOCK:
#if 0
        SYSTEM_LOG("%8lu,%3lu ",
            p->sector/2, ((p->sector & 1) ? 512 : 0) + p->offset);
#endif
        out_rounddown(8, start, 2, increment);
        out_roundup(8, end, 2, increment);
        out_rounddown(8, size, 2, 0);
        break;
    case F_MEGABYTE:
        out_rounddown(5, start, 2048, increment);
        out_roundup(5, end, 2048, increment);
        out_roundup_size(5, size, 2048);
        out_rounddown(8, size, 2, 0);
        break;
    }
    if (p->ptype == DOS_TYPE) {
        printf(" %2x  %s\n",
            p->p.sys_type, sysname(p->p.sys_type));
    } else {
        printf("\n");
    }

    /* Is chs as we expect? */
    if (!quiet && p->ptype == DOS_TYPE) {
        chs a, b;
        longchs aa, bb;
        a = (size ? ulong_to_chs(start,G) : zero_chs);
        b = p->p.begin_chs;
        aa = chs_to_longchs(a);
        bb = chs_to_longchs(b);
        if(a.s && !is_equal_chs(a, b))
            SYSTEM_LOG(_("\t\tstart: (c,h,s) expected (%ld,%ld,%ld) found (%ld,%ld,%ld)\n"),
                aa.c, aa.h, aa.s, bb.c, bb.h, bb.s);
        a = (size ? ulong_to_chs(end,G) : zero_chs);
        b = p->p.end_chs;
        aa = chs_to_longchs(a);
        bb = chs_to_longchs(b);
        if(a.s && !is_equal_chs(a, b))
            SYSTEM_LOG(_("\t\tend: (c,h,s) expected (%ld,%ld,%ld) found (%ld,%ld,%ld)\n"),
                aa.c, aa.h, aa.s, bb.c, bb.h, bb.s);
        if(G.cylinders && G.cylinders < 1024 && bb.c > G.cylinders)
            SYSTEM_LOG(_("partition ends on cylinder %ld, beyond the end of the disk\n"),
                bb.c);
    }
}

static void
out_partitions(char *dev, struct disk_desc *z) {
    struct part_desc *p;
    int pno, format = 0;

    if (z->partno == 0)
        SYSTEM_LOG(_("No partitions found\n"));
    else {
        for (pno=0; pno < z->partno; pno++) {
            p = &(z->partitions[pno]);
            if (p->size != 0 && p->p.sys_type != 0) {
                if (get_fdisk_geometry(p) && !dump)
                    SYSTEM_LOG(
                        _("Warning: The first partition looks like it was made\n"
                            "  for C/H/S=*/%ld/%ld (instead of %ld/%ld/%ld).\n"
                            "For this listing I'll assume that geometry.\n"),
                        F.heads, F.sectors, B.cylinders, B.heads, B.sectors);
                break;
            }
        }
        out_partition_header(dev, format, F);
        for(pno=0; pno < z->partno; pno++) {
            out_partition(dev, format, &(z->partitions[pno]), z, F);
            if(show_extended && pno%4==3)
                SYSTEM_LOG("\n");
        }
    }
}

static int
disj(struct part_desc *p, struct part_desc *q) {
    return
        ((p->start + p->size <= q->start)
         || (is_extended(p->p.sys_type)
             && q->start + q->size <= p->start + p->size));
}

static char *
pnumber(struct part_desc *p, struct disk_desc *z) {
    static char buf[20];
    int this, next;
    struct part_desc *p0 = &(z->partitions[0]);

    this = index_to_linux(p-p0, z);
    next = index_to_linux(p-p0+1, z);

    if (next > this)
        sprintf(buf, "%d", this);
    else
        sprintf(buf, "[%d]", this);
    return buf;
}

static int
partitions_ok(struct disk_desc *z) {
    struct part_desc *partitions = &(z->partitions[0]), *p, *q;
    int partno = z->partno;

#define PNO(p) pnumber(p, z)

    /* Have at least 4 partitions been defined? */
    if (partno < 4) {
        if (!partno)
            fatal(_("no partition table present.\n"));
        else
            fatal(_("strange, only %d partitions defined.\n"), partno);
        return 0;
    }

    /* Are the partitions of size 0 marked empty?
       And do they have start = 0? And bootable = 0? */
    for (p = partitions; p - partitions < partno; p++)
        if (p->size == 0) {
            if(p->p.sys_type != EMPTY_PARTITION)
                warn(_("Warning: partition %s has size 0 but is not marked Empty\n"),
                    PNO(p));
            else if(p->p.bootable != 0)
                warn(_("Warning: partition %s has size 0 and is bootable\n"),
                    PNO(p));
            else if(p->p.start_sect != 0)
                warn(_("Warning: partition %s has size 0 and nonzero start\n"),
                    PNO(p));
            /* all this is probably harmless, no error return */
        }

    /* Are the logical partitions contained in their extended partitions? */
    for (p = partitions+4; p < partitions+partno; p++)
        if (p->ptype == DOS_TYPE)
            if (p->size && !is_extended(p->p.sys_type)) {
                q = p->ep;
                if (p->start < q->start || p->start + p->size > q->start + q->size) {
                    warn(_("Warning: partition %s "), PNO(p));
                    warn(_("is not contained in partition %s\n"), PNO(q));
                    return 0;
                }
            }

    /* Are the data partitions mutually disjoint? */
    for (p = partitions; p < partitions+partno; p++)
        if (p->size && !is_extended(p->p.sys_type))
            for (q = p+1; q < partitions+partno; q++)
                if (q->size && !is_extended(q->p.sys_type))
                    if(!((p->start > q-> start) ? disj(q,p) : disj(p,q))) {
                        warn(_("Warning: partitions %s "), PNO(p));
                        warn(_("and %s overlap\n"), PNO(q));
                        return 0;
                    }

    /* Are the data partitions and the extended partition
       table sectors disjoint? */
    for (p = partitions; p < partitions+partno; p++)
        if (p->size && !is_extended(p->p.sys_type))
            for (q = partitions; q < partitions+partno; q++)
                if (is_extended(q->p.sys_type))
                    if (p->start <= q->start && p->start + p->size > q->start) {
                        warn(_("Warning: partition %s contains part of "), PNO(p));
                        warn(_("the partition table (sector %lu),\n"), q->start);
                        warn(_("and will destroy it when filled\n"));
                        return 0;
                    }

    /* Do they start past zero and end before end-of-disk? */
    { unsigned long ds = get_disksize(F_SECTOR);
        for (p = partitions; p < partitions+partno; p++)
            if (p->size) {
                if(p->start == 0) {
                    warn(_("Warning: partition %s starts at sector 0\n"), PNO(p));
                    return 0;
                }
                if (p->size && p->start + p->size > ds) {
                    warn(_("Warning: partition %s extends past end of disk\n"), PNO(p));
                    return 0;
                }
            }
    }

    /* At most one chain of DOS extended partitions ? */
    /* It seems that the OS/2 fdisk has the additional requirement
       that the extended partition must be the fourth one */
    { int ect = 0;
        for (p = partitions; p < partitions+4; p++)
            if (p->p.sys_type == EXTENDED_PARTITION)
                ect++;
        if (ect > 1 && !Linux) {
            warn(_("Among the primary partitions, at most one can be extended\n"));
            warn(_(" (although this is not a problem under Linux)\n"));
            return 0;
        }
    }

    /*
     * Do all partitions start at a cylinder boundary ?
     * (this is not required for Linux)
     * The first partition starts after MBR.
     * Logical partitions start slightly after the containing extended partn.
     */
    if (B.cylindersize) {
        for(p = partitions; p < partitions+partno; p++)
            if (p->size) {
                if(p->start % B.cylindersize != 0
                    && (!p->ep || p->start / B.cylindersize != p->ep->start / B.cylindersize)
                    && (p->p.start_sect >= B.cylindersize)) {
                    warn(_("Warning: partition %s does not start "
                            "at a cylinder boundary\n"), PNO(p));
                    if (!Linux)
                        return 0;
                }
                if((p->start + p->size) % B.cylindersize) {
                    warn(_("Warning: partition %s does not end "
                            "at a cylinder boundary\n"), PNO(p));
                    if (!Linux)
                        return 0;
                }
            }
    }

    /* Usually, one can boot only from primary partitions. */
    /* In fact, from a unique one only. */
    /* do not warn about bootable extended partitions -
       often LILO is there */
    { int pno = -1;
        for(p = partitions; p < partitions+partno; p++)
            if (p->p.bootable) {
                if (pno == -1)
                    pno = p - partitions;
                else if (p - partitions < 4) {
                    warn(_("Warning: more than one primary partition is marked "
                            "bootable (active)\n"
                            "This does not matter for LILO, but the DOS MBR will "
                            "not boot this disk.\n"));
                    break;
                }
                if (p - partitions >= 4) {
                    warn(_("Warning: usually one can boot from primary partitions "
                            "only\nLILO disregards the `bootable' flag.\n"));
                    break;
                }
            }
        if (pno == -1 || pno >= 4)
            warn(_("Warning: no primary partition is marked bootable (active)\n"
                    "This does not matter for LILO, but the DOS MBR will "
                    "not boot this disk.\n"));
    }

    /* Is chs as we expect? */
    for(p = partitions; p < partitions+partno; p++)
        if(p->ptype == DOS_TYPE) {
            chs a, b;
            longchs aa, bb;
            a = p->size ? ulong_to_chs(p->start,B) : zero_chs;
            b = p->p.begin_chs;
            aa = chs_to_longchs(a);
            bb = chs_to_longchs(b);
            if (!chs_ok(b, PNO(p), "start"))
                return 0;
            if(a.s && !is_equal_chs(a, b))
                warn(_("partition %s: start: (c,h,s) expected (%ld,%ld,%ld) found (%ld,%ld,%ld)\n"),
                    PNO(p), aa.c, aa.h, aa.s, bb.c, bb.h, bb.s);
            a = p->size ? ulong_to_chs(p->start + p->size - 1, B) : zero_chs;
            b = p->p.end_chs;
            aa = chs_to_longchs(a);
            bb = chs_to_longchs(b);
            if (!chs_ok(b, PNO(p), "end"))
                return 0;
            if(a.s && !is_equal_chs(a, b))
                warn(_("partition %s: end: (c,h,s) expected (%ld,%ld,%ld) found (%ld,%ld,%ld)\n"),
                    PNO(p), aa.c, aa.h, aa.s, bb.c, bb.h, bb.s);
            if(B.cylinders && B.cylinders < 1024 && bb.c > B.cylinders)
                warn(_("partition %s ends on cylinder %ld, beyond the end of the disk\n"),
                    PNO(p), bb.c);
        }

    return 1;

#undef PNO
}

static void
extended_partition(char *dev, int fd, struct part_desc *ep, struct disk_desc *z) {
    char *cp;
    struct sector *s;
    unsigned long start, here, next;
    int i, moretodo = 1;
    struct partition p;
    struct part_desc *partitions = &(z->partitions[0]);
    int pno = z->partno;

    here = start = ep->start;

    if (B.cylindersize && start % B.cylindersize) {
        /* This is BAD */
        if (DOS_extended) {
            here = start -= (start % B.cylindersize);
            SYSTEM_LOG(_("Warning: shifted start of the extd partition from %ld to %ld\n"),
                ep->start, start);
            SYSTEM_LOG(_("(For listing purposes only. Do not change its contents.)\n"));
        } else {
            SYSTEM_LOG(_("Warning: extended partition does not start at a "
                    "cylinder boundary.\n"));
            SYSTEM_LOG(_("DOS and Linux will interpret the contents differently.\n"));
        }
    }

    while (moretodo) {
        moretodo = 0;

        if (!(s = get_sector(dev, fd, here)))
            break;

        if (!msdos_signature(s))
            break;

        cp = s->data + 0x1be;

        if (pno+4 >= SIZE(z->partitions)) {
            SYSTEM_LOG(_("too many partitions - ignoring those past nr (%d)\n"),
                pno-1);
            break;
        }

        next = 0;

        for (i=0; i<4; i++,cp += sizeof(struct partition)) {
            partitions[pno].sector = here;
            partitions[pno].offset = cp - s->data;
            partitions[pno].ep = ep;
            copy_to_part(cp,&p);
            if (is_extended(p.sys_type)) {
                partitions[pno].start = start + p.start_sect;
                if (next)
                    SYSTEM_LOG(_("tree of partitions?\n"));
                else
                    next = partitions[pno].start;		/* follow `upper' branch */
                moretodo = 1;
            } else {
                partitions[pno].start = here + p.start_sect;
            }
            partitions[pno].size = p.nr_sects;
            partitions[pno].ptype = DOS_TYPE;
            partitions[pno].p = p;
            pno++;
        }
        here = next;
    }

    z->partno = pno;
}

#define BSD_DISKMAGIC   (0x82564557UL)
#define BSD_MAXPARTITIONS       8
#define BSD_FS_UNUSED           0
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
struct bsd_disklabel {
    u32	d_magic;
    char	d_junk1[4];
    char    d_typename[16];
    char    d_packname[16];
    char	d_junk2[92];
    u32	d_magic2;
    char	d_junk3[2];
    u16	d_npartitions;          /* number of partitions in following */
    char	d_junk4[8];
    struct  bsd_partition {         /* the partition table */
        u32   p_size;         /* number of sectors in partition */
        u32   p_offset;       /* starting sector */
        u32   p_fsize;        /* filesystem basic fragment size */
        u8    p_fstype;       /* filesystem type, see below */
        u8    p_frag;         /* filesystem fragments per block */
        u16   p_cpg;          /* filesystem cylinders per group */
    } d_partitions[BSD_MAXPARTITIONS];      /* actually may be more */
};

static void
bsd_partition(char *dev, int fd, struct part_desc *ep, struct disk_desc *z) {
    struct bsd_disklabel *l;
    struct bsd_partition *bp, *bp0;
    unsigned long start = ep->start;
    struct sector *s;
    struct part_desc *partitions = &(z->partitions[0]);
    int pno = z->partno;

    if (!(s = get_sector(dev,fd,start+1)))
        return;
    l = (struct bsd_disklabel *) (s->data);
    if (l->d_magic != BSD_DISKMAGIC)
        return;

    bp = bp0 = &l->d_partitions[0];
    while (bp - bp0 <= BSD_MAXPARTITIONS) {
        if (pno+1 >= SIZE(z->partitions)) {
            SYSTEM_LOG(_("too many partitions - ignoring those "
                    "past nr (%d)\n"), pno-1);
            break;
        }
        if (bp->p_fstype != BSD_FS_UNUSED) {
            partitions[pno].start = bp->p_offset;
            partitions[pno].size = bp->p_size;
            partitions[pno].sector = start+1;
            partitions[pno].offset = (char *)bp - (char *)bp0;
            partitions[pno].ep = 0;
            partitions[pno].ptype = BSD_TYPE;
            pno++;
        }
        bp++;
    }
    z->partno = pno;
}

#define MAKE_VERSION(p,q,r)     (65536*(p) + 256*(q) + (r))

static int
linux_version_code(void) {
    struct utsname my_utsname;
    int p, q, r;

    if (uname(&my_utsname) == 0) {
        p = atoi(strtok(my_utsname.release, "."));
        q = atoi(strtok(NULL, "."));
        r = atoi(strtok(NULL, "."));
        return MAKE_VERSION(p,q,r);
    }
    return 0;
}

static int
msdos_partition(char *dev, int fd, unsigned long start, struct disk_desc *z) {
    int i;
    char *cp;
    struct partition pt;
    struct sector *s;
    struct part_desc *partitions = &(z->partitions[0]);
    int pno = z->partno;
    int bsd_later = (linux_version_code() >= MAKE_VERSION(2,3,40));

    if (!(s = get_sector(dev, fd, start)))
        return 0;

    if (!msdos_signature(s))
        return 0;

    cp = s->data + 0x1be;
    copy_to_part(cp,&pt);

    /* If I am not mistaken, recent kernels will hide this from us,
       so we will never actually see traces of a Disk Manager */
    if (pt.sys_type == DM6_PARTITION
        || pt.sys_type == EZD_PARTITION
        || pt.sys_type == DM6_AUX1PARTITION
        || pt.sys_type == DM6_AUX3PARTITION) {
        SYSTEM_LOG(_("detected Disk Manager - unable to handle that\n"));
        return 0;
    }
    { unsigned int sig = *(unsigned short *)(s->data + 2);
        if (sig <= 0x1ae
            && *(unsigned short *)(s->data + sig) == 0x55aa
            && (1 & *(unsigned char *)(s->data + sig + 2))) {
            SYSTEM_LOG(_("DM6 signature found - giving up\n"));
            return 0;
        }
    }

    for (pno=0; pno<4; pno++,cp += sizeof(struct partition)) {
        partitions[pno].sector = start;
        partitions[pno].offset = cp - s->data;
        copy_to_part(cp,&pt);
        partitions[pno].start = start + pt.start_sect;
        partitions[pno].size = pt.nr_sects;
        partitions[pno].ep = 0;
        partitions[pno].p = pt;
    }

    z->partno = pno;

    for (i=0; i<4; i++) {
        if (is_extended(partitions[i].p.sys_type)) {
            if (!partitions[i].size) {
                SYSTEM_LOG(_("strange..., an extended partition of size 0?\n"));
                continue;
            }
            extended_partition(dev, fd, &partitions[i], z);
        }
        if (!bsd_later && is_bsd(partitions[i].p.sys_type)) {
            if (!partitions[i].size) {
                SYSTEM_LOG(_("strange..., a BSD partition of size 0?\n"));
                continue;
            }
            bsd_partition(dev, fd, &partitions[i], z);
        }
    }

    if (bsd_later) {
        for (i=0; i<4; i++) {
            if (is_bsd(partitions[i].p.sys_type)) {
                if (!partitions[i].size) {
                    SYSTEM_LOG(_("strange..., a BSD partition of size 0?\n"));
                    continue;
                }
                bsd_partition(dev, fd, &partitions[i], z);
            }
        }
    }

    return 1;
}

static int
osf_partition(char *dev, int fd, unsigned long start, struct disk_desc *z) {
    return 0;
}

static int
sun_partition(char *dev, int fd, unsigned long start, struct disk_desc *z) {
    return 0;
}

static int
amiga_partition(char *dev, int fd, unsigned long start, struct disk_desc *z) {
    return 0;
}

static void
get_partitions(char *dev, int fd, struct disk_desc *z) {
    z->partno = 0;

    if (!msdos_partition(dev, fd, 0, z)
        && !osf_partition(dev, fd, 0, z)
        && !sun_partition(dev, fd, 0, z)
        && !amiga_partition(dev, fd, 0, z)) {
        SYSTEM_LOG(_(" %s: unrecognized partition\n"), dev);
        return;
    }
}

static int
write_partitions(char *dev, int fd, struct disk_desc *z) {
    struct sector *s;
    struct part_desc *partitions = &(z->partitions[0]), *p;
    int pno = z->partno;

    if (no_write) {
        SYSTEM_LOG(_("-n flag was given: Nothing changed\n"));
        exit(0);
    }

    for (p = partitions; p < partitions+pno; p++) {
        s = get_sector(dev, fd, p->sector);
        if (!s) return 0;
        s->to_be_written = 1;
        copy_from_part(&(p->p), s->data + p->offset);
        //*(unsigned short *)(&(s->data[0x1fe])) = 0xaa55;
        // TDT - friendlier for switch endian
        *(unsigned char *)(&(s->data[510])) = 0x55;
        *(unsigned char *)(&(s->data[511])) = 0xaa;
    }
    if (save_sector_file) {
        if (!save_sectors(dev, fd)) {
            fatal(_("Failed saving the old sectors - aborting\n"));
            return 0;
        }
    }
    if (!write_sectors(dev, fd)) {
        error(_("Failed writing the partition on %s\n"), dev);
        return 0;
    }
    return 1;
}

/*
 *  F. The standard input
 */

/*
 * Input format:
 * <start> <size> <type> <bootable> <c,h,s> <c,h,s>
 * Fields are separated by whitespace or comma or semicolon possibly
 * followed by whitespace; initial and trailing whitespace is ignored.
 * Numbers can be octal, decimal or hexadecimal, decimal is default
 * The <c,h,s> parts can (and probably should) be omitted.
 * Bootable is specified as [*|-], with as default not-bootable.
 * Type is given in hex, without the 0x prefix, or is [E|S|L|X], where
 * L (LINUX_NATIVE (83)) is the default, S is LINUX_SWAP (82), and E
 * is EXTENDED_PARTITION (5), X is LINUX_EXTENDED (85).
 * The default value of start is the first nonassigned sector/cylinder/...
 * The default value of size is as much as possible (until next
 * partition or end-of-disk).
 * .: end of chain of extended partitions.
 *
 * On interactive input an empty line means: all defaults.
 * Otherwise empty lines are ignored.
 */

int eof, eob;

struct dumpfld {
    int fldno;
    char *fldname;
    int is_bool;
} dumpflds[] = {
    { 0, "start", 0 },
    { 1, "size", 0 },
    { 2, "Id", 0 },
    { 3, "bootable", 1 },
    { 4, "bh", 0 },
    { 5, "bs", 0 },
    { 6, "bc", 0 },
    { 7, "eh", 0 },
    { 8, "es", 0 },
    { 9, "ec", 0 }
};

/*
 * Read a line, split it into fields
 *
 * (some primitive handwork, but a more elaborate parser seems
 *  unnecessary)
 */
#define RD_EOF (-1)
#define RD_CMD (-2)

/* read a number, use default if absent */
static int
get_ul(char *u, unsigned long *up, unsigned long def, int base) {
    char *nu;

    if (*u) {
        errno = 0;
        *up = strtoul(u, &nu, base);
        if (errno == ERANGE) {
            SYSTEM_LOG(_("number too big\n"));
            return -1;
        }
        if (*nu) {
            SYSTEM_LOG(_("trailing junk after number\n"));
            return -1;
        }
    } else
        *up = def;
    return 0;
}

/* There are two common ways to structure extended partitions:
   as nested boxes, and as a chain. Sometimes the partitions
   must be given in order. Sometimes all logical partitions
   must lie inside the outermost extended partition.
NESTED: every partition is contained in the surrounding partitions
and is disjoint from all others.
CHAINED: every data partition is contained in the surrounding partitions
and disjoint from all others, but extended partitions may lie outside
(insofar as allowed by all_logicals_inside_outermost_extended).
ONESECTOR: all data partitions are mutually disjoint; extended partitions
each use one sector only (except perhaps for the outermost one).
*/
int partitions_in_order = 0;
int all_logicals_inside_outermost_extended = 1;
enum { NESTED, CHAINED, ONESECTOR } boxes = NESTED;

/* find the default value for <start> - assuming entire units */
static unsigned long
first_free(int pno, int pIs_extended, struct part_desc *ep, int format, unsigned long mid, struct disk_desc *z)
{
    unsigned long ff, fff;
    unsigned long unit = unitsize(format);
    struct part_desc *partitions = &(z->partitions[0]), *pp = 0;

    /* if containing ep undefined, look at its container */
    if (ep && ep->p.sys_type == EMPTY_PARTITION)
        ep = ep->ep;

    if (ep) {
        if (boxes == NESTED || (boxes == CHAINED && !pIs_extended))
            pp = ep;
        else if (all_logicals_inside_outermost_extended)
            pp = outer_extended_partition(ep);
    }
#if 0
    ff = pp ? (pp->start + unit - 1) / unit : 0;
#else
    /* rounding up wastes almost an entire cylinder - round down
       and leave it to compute_start_sect() to fix the difference */
    ff = pp ? pp->start / unit : 0;
#endif
    /* MBR and 1st sector of an extended partition are never free */
    if (unit == 1)
        ff++;

again:
    for(pp = partitions; pp < partitions+pno; pp++) {
        if (!is_parent(pp, ep) && pp->size > 0) {
            if ((partitions_in_order || pp->start / unit <= ff
                    || (mid && pp->start / unit <= mid))
                && (fff = (pp->start + pp->size + unit - 1) / unit) > ff) {
                ff = fff;
                goto again;
            }
        }
    }

    return ff;
}

/* find the default value for <size> - assuming entire units */
static unsigned long
max_length(int pno, int pIs_extended, struct part_desc *ep, int format, unsigned long start, struct disk_desc *z)
{
    unsigned long fu;
    unsigned long unit = unitsize(format);
    struct part_desc *partitions = &(z->partitions[0]), *pp = 0;

    /* if containing ep undefined, look at its container */
    if (ep && ep->p.sys_type == EMPTY_PARTITION)
        ep = ep->ep;

    if (ep) {
        if (boxes == NESTED || (boxes == CHAINED && !pIs_extended))
            pp = ep;
        else if (all_logicals_inside_outermost_extended)
            pp = outer_extended_partition(ep);
    }
    fu = pp ? (pp->start + pp->size) / unit : get_disksize(format);

    for(pp = partitions; pp < partitions+pno; pp++)
        if (!is_parent(pp, ep) && pp->size > 0
            && pp->start / unit >= start && pp->start / unit < fu)
            fu = pp->start / unit;

    return (fu > start) ? fu - start : 0;
}

/* compute starting sector of a partition inside an extended one */
/* ep is 0 or points to surrounding extended partition */
static int
compute_start_sect(struct part_desc *p, struct part_desc *ep)
{
    unsigned long base;
    int inc = (DOS && B.sectors) ? B.sectors : 1;
    int delta;

    if (ep && p->start + p->size >= ep->start + 1)
        delta = p->start - ep->start - inc;
    else if (p->start == 0 && p->size > 0)
        delta = -inc;
    else
        delta = 0;
    if (delta < 0) {
        p->start -= delta;
        p->size += delta;
        if (is_extended(p->p.sys_type) && boxes == ONESECTOR)
            p->size = inc;
        else if ((int)(p->size) <= 0) {
            warn(_("no room for partition descriptor\n"));
            return 0;
        }
    }
    base = (!ep ? 0 : (is_extended(p->p.sys_type) ? outer_extended_partition(ep) : ep)->start);
    p->ep = ep;
    if (p->p.sys_type == EMPTY_PARTITION && p->size == 0) {
        p->p.start_sect = 0;
        p->p.begin_chs = zero_chs;
        p->p.end_chs = zero_chs;
    } else {
        p->p.start_sect = p->start - base;
        p->p.begin_chs = ulong_to_chs(p->start,B);
        p->p.end_chs = ulong_to_chs(p->start + p->size - 1,B);
    }
    p->p.nr_sects = p->size;
    return 1;
}    

/* build the extended partition surrounding a given logical partition */
static int
build_surrounding_extended(struct part_desc *p, struct part_desc *ep,
    struct disk_desc *z) {
    int inc = (DOS && B.sectors) ? B.sectors : 1;
    int format = F_SECTOR;
    struct part_desc *p0 = &(z->partitions[0]), *eep = ep->ep;

    if (boxes == NESTED) {
        ep->start = first_free(ep-p0, 1, eep, format, p->start, z);
        ep->size = max_length(ep-p0, 1, eep, format, ep->start, z);
        if (ep->start > p->start || ep->start + ep->size < p->start + p->size) {
            warn(_("cannot build surrounding extended partition\n"));
            return 0;
        }
    } else {
        ep->start = p->start;
        if(boxes == CHAINED)
            ep->size = p->size;
        else
            ep->size = inc;
    }

    ep->p.nr_sects = ep->size;
    ep->p.bootable = 0;
    ep->p.sys_type = EXTENDED_PARTITION;
    if (!compute_start_sect(ep, eep) || !compute_start_sect(p, ep)) {
        ep->p.sys_type = EMPTY_PARTITION;
        ep->size = 0;
        return 0;
    }

    return 1;
}

static void set_sys_type(int type)
{
    Yx_sys_type = type;
}
static int
read_line(int pno, struct part_desc *ep, char *dev, int interactive,
    struct disk_desc *z) {
    unsigned char line[1000];
    unsigned char *fields[11];
    int fno, pct = pno%4;
    struct part_desc p, *orig;
    unsigned long ff, ff1, ul, ml, ml1, def;
    int format, lpno, is_extd;

    if (eof || eob)
        return -1;

    lpno = index_to_linux(pno, z);

    if (interactive) {
        if (pct == 0 && (show_extended || pno == 0))
            warn("\n");
#if !defined(hi3560e)
        warn("%s:", partname(dev, lpno, 10));
#endif
    }

    /* read input line - skip blank lines when reading from a file */
    do {
        //fno = read_stdin(fields, line, SIZE(fields), SIZE(line));
        //SYSTEM_LOG("fno = %d\n", fno);
        fno = 2;
        SYSTEM_LOG("@@@->here->%s\n", partitionInputInfo[pno].partitionInput[0]);
        fields[0] = partitionInputInfo[pno].partitionInput[0];
        fields[1] = partitionInputInfo[pno].partitionInput[1];
        SYSTEM_LOG("@@@->here->%s\n", partitionInputInfo[pno].partitionInput[1]);
        int i;
        for(i = 0; i < 2; i ++)
        {
            SYSTEM_LOG("%s\t", fields[i]);
        }
        SYSTEM_LOG("\n");
        SYSTEM_LOG("###-> %s   SIZE(fields) = %d   SIZE(line) = %d\n", line, SIZE(fields), SIZE(line));
    } while(fno == RD_CMD || (fno == 0 && !interactive));
    if (fno == RD_EOF) {
        return -1;
    } else if (fno > 10 && *(fields[10]) != 0) {
        SYSTEM_LOG(_("too many input fields\n"));
        return 0;
    }

    if (fno == 1 && !strcmp((char *)(fields[0]), ".")) {
        eob = 1;
        return -1;
    }

    /* use specified format, but round to cylinders if F_MEGABYTE specified */
    format = 0;
    if (B.cylindersize && specified_format == F_MEGABYTE)
        format = F_CYLINDER;

    orig = (one_only ? &(oldp.partitions[pno]) : 0);

    p = zero_part_desc;
    p.ep = ep;

    /* first read the type - we need to know whether it is extended */
    /* stop reading when input blank (defaults) and all is full */
    is_extd = 0;
    if (fno == 0) {		/* empty line */
        if (orig && is_extended(orig->p.sys_type))
            is_extd = 1;
        ff = first_free(pno, is_extd, ep, format, 0, z);
        ml = max_length(pno, is_extd, ep, format, ff, z);
        if (ml == 0 && is_extd == 0) {
            is_extd = 1;
            ff = first_free(pno, is_extd, ep, format, 0, z);
            ml = max_length(pno, is_extd, ep, format, ff, z);
        }
        if (ml == 0 && pno >= 4) {
            /* no free blocks left - don't read any further */
            warn(_("No room for more\n"));
            return -1;
        }
    }
    if (fno < 3 || !*(fields[2]))
        ul = orig ? orig->p.sys_type :
            (is_extd || (pno > 3 && pct == 1 && show_extended))
            ? EXTENDED_PARTITION : Yx_sys_type;// WIN95_FAT32;//LINUX_NATIVE;
    else if(!strcmp((char *)(fields[2]), "L"))
        ul = LINUX_NATIVE;
    else if(!strcmp((char *)(fields[2]), "S"))
        ul = LINUX_SWAP;
    else if(!strcmp((char *)(fields[2]), "E"))
        ul = EXTENDED_PARTITION;
    else if(!strcmp((char *)(fields[2]), "X"))
        ul = LINUX_EXTENDED;
    else if (get_ul((char *)(fields[2]), &ul, LINUX_NATIVE, 16))
        return 0;
    if (ul > 255) {
        warn(_("Illegal type\n"));
        return 0;
    }
    p.p.sys_type = ul;
    is_extd = is_extended(ul);

    /* find start */
    ff = first_free(pno, is_extd, ep, format, 0, z);
    ff1 = ff * unitsize(format);
    def = orig ? orig->start : (pno > 4 && pct > 1) ? 0 : ff1;
    if (fno < 1 || !*(fields[0]))
        p.start = def;
    else {
        if (get_ul((char *)(fields[0]), &ul, def / unitsize(0), 0))
            return 0;
        p.start = ul * unitsize(0);
        p.start -= (p.start % unitsize(format));
    }

    /* find length */
    ml = max_length(pno, is_extd, ep, format, p.start / unitsize(format), z);
    ml1 = ml * unitsize(format);
    def = orig ? orig->size : (pno > 4 && pct > 1) ? 0 : ml1;
    if (fno < 2 || !*(fields[1]))
        p.size = def;
    else {
        if (get_ul((char *)(fields[1]), &ul, def / unitsize(0), 0))
            return 0;
        p.size = ul * unitsize(0) + unitsize(format) - 1;
        p.size -= (p.size % unitsize(format));
    }
    if (p.size > ml1) {
        warn(_("Warning: given size (%lu) exceeds max allowable size (%lu)\n"),
            (p.size + unitsize(0) - 1) / unitsize(0), ml1 / unitsize(0));
        if (!force)
            return 0;
    }
    if (p.size == 0 && pno >= 4 && (fno < 2 || !*(fields[1]))) {
        warn(_("Warning: empty partition\n"));
        if (!force)
            return 0;
    }
    p.p.nr_sects = p.size;

    if (p.size == 0 && !orig) {
        if(fno < 1 || !*(fields[0]))
            p.start = 0;
        if(fno < 3 || !*(fields[2]))
            p.p.sys_type = EMPTY_PARTITION;
    }

    if (p.start < ff1 && p.size > 0) {
        warn(_("Warning: bad partition start (earliest %lu)\n"),
            (ff1 + unitsize(0) - 1) / unitsize(0));
        if (!force)
            return 0;
    }

    if (fno < 4 || !*(fields[3]))
        ul = (orig ? orig->p.bootable : 0);
    else if (!strcmp((char *)(fields[3]), "-"))
        ul = 0;
    else if (!strcmp((char *)(fields[3]), "*") || !strcmp((char *)(fields[3]), "+"))
        ul = 0x80;
    else {
        warn(_("unrecognized bootable flag - choose - or *\n"));
        return 0;
    }
    p.p.bootable = ul;

    if (ep && ep->p.sys_type == EMPTY_PARTITION) {
        if(!build_surrounding_extended(&p, ep, z))
            return 0;
    } else
        if(!compute_start_sect(&p, ep))
            return 0;

    { longchs aa = chs_to_longchs(p.p.begin_chs), bb;

        if (fno < 5) {
            bb = aa;
        } else if (fno < 7) {
            warn(_("partial c,h,s specification?\n"));
            return 0;
        } else if(get_ul((char *)(fields[4]), &bb.c, aa.c, 0) ||
            get_ul((char *)(fields[5]), &bb.h, aa.h, 0) ||
            get_ul((char *)(fields[6]), &bb.s, aa.s, 0))
            return 0;
        p.p.begin_chs = longchs_to_chs(bb,B);
    }
    { longchs aa = chs_to_longchs(p.p.end_chs), bb;

        if (fno < 8) {
            bb = aa;
        } else if (fno < 10) {
            warn(_("partial c,h,s specification?\n"));
            return 0;
        } else if(get_ul((char *)(fields[7]), &bb.c, aa.c, 0) ||
            get_ul((char *)(fields[8]), &bb.h, aa.h, 0) ||
            get_ul((char *)(fields[9]), &bb.s, aa.s, 0))
            return 0;
        p.p.end_chs = longchs_to_chs(bb, B);
    }

    if (pno > 3 && p.size && show_extended && p.p.sys_type != EMPTY_PARTITION
        && (is_extended(p.p.sys_type) != (pct == 1))) {
        warn(_("Extended partition not where expected\n"));
        if (!force)
            return 0;
    }

    z->partitions[pno] = p;
    if (pno >= z->partno)
        z->partno += 4;		/* reqd for out_partition() */

    if (interactive)
        out_partition(dev, 0, &(z->partitions[pno]), z, B);

    return 1;
}

/* ep either points to the extended partition to contain this one,
   or to the empty partition that may become extended or is 0 */
static int
read_partition(char *dev, int interactive, int pno, struct part_desc *ep,
    struct disk_desc *z) {
    struct part_desc *p = &(z->partitions[pno]);
    int i;

    SYSTEM_LOG("&&& -> read_partition\n");
    if (one_only) {
        *p = oldp.partitions[pno];
        if (one_only_pno != pno)
            goto ret;
    } else if (!show_extended && pno > 4 && pno%4)
        goto ret;

    while (!(i = read_line(pno, ep, dev, interactive, z)))
        if (!interactive)
            fatal(_("bad input\n"));
    if (i < 0) {
        p->ep = ep;
        return 0;
    }

ret:
    p->ep = ep;
    if (pno >= z->partno)
        z->partno += 4;
    return 1;
}

static void
read_partition_chain(char *dev, int interactive, struct part_desc *ep,
    struct disk_desc *z) {
    int i, base;

    SYSTEM_LOG("&&& -> read_partition_chain\n");
    eob = 0;
    while (1) {
        base = z->partno;
        if (base+4 > SIZE(z->partitions)) {
            SYSTEM_LOG(_("too many partitions\n"));
            break;
        }
        for (i=0; i<4; i++)
            if (!read_partition(dev, interactive, base+i, ep, z))
                return;
        for (i=0; i<4; i++) {
            ep = &(z->partitions[base+i]);
            if (is_extended(ep->p.sys_type) && ep->size)
                break;
        }
        if (i == 4) {
            /* nothing found - maybe an empty partition is going
               to be extended */
            if (one_only || show_extended)
                break;
            ep = &(z->partitions[base+1]);
            if (ep->size || ep->p.sys_type != EMPTY_PARTITION)
                break;
        }
    }
}

static void
read_input(char *dev, int interactive, struct disk_desc *z) {
    int i;
    struct part_desc *partitions = &(z->partitions[0]), *ep;

    for (i=0; i < SIZE(z->partitions); i++)
        partitions[i] = zero_part_desc;
    z->partno = 0;

    if (interactive)
        warn(_("Input in the following format; absent fields get a default value.\n"
                "<start> <size> <type [E,S,L,X,hex]> <bootable [-,*]> <c,h,s> <c,h,s>\n"
                "Usually you only need to specify <start> and <size> (and perhaps <type>).\n"));
    eof = 0;

    for (i=0; i<4; i++)
    {
        SYSTEM_LOG("###partitionInput[1]->%s\n", partitionInputInfo[i].partitionInput[1]);
        read_partition(dev, interactive, i, 0, z);
    }
    for (i=0; i<4; i++) {
        ep = partitions+i;
        if (is_extended(ep->p.sys_type) && ep->size)
            read_partition_chain(dev, interactive, ep, z);
    }
    add_sector_and_offset(z);
}

#define PRINT_ID 0400
#define CHANGE_ID 01000

static const struct option long_opts[] = {
    { "change-id",	  no_argument, NULL, 'c' + CHANGE_ID },
    { "print-id",	  no_argument, NULL, 'c' + PRINT_ID },
    { "id",		  no_argument, NULL, 'c' },
    { "dump",             no_argument, NULL, 'd' },
    { "force",            no_argument, NULL, 'f' },
    { "show-geometry",	  no_argument, NULL, 'g' },
    { "increment",        no_argument, NULL, 'i' },
    { "list",             no_argument, NULL, 'l' },
    { "quiet",            no_argument, NULL, 'q' },
    { "show-size",        no_argument, NULL, 's' },
    { "unit",       required_argument, NULL, 'u' },
    { "version",          no_argument, NULL, 'v' },
    { "show-extended",    no_argument, NULL, 'x' },
    { "help",	          no_argument, NULL, '?' },
    { "one-only",         no_argument, NULL, '1' },
    { "cylinders",  required_argument, NULL, 'C' },
    { "heads",      required_argument, NULL, 'H' },
    { "sectors",    required_argument, NULL, 'S' },
    { "activate",   optional_argument, NULL, 'A' },
    { "DOS",              no_argument, NULL, 'D' },
    { "DOS-extended",	  no_argument, NULL, 'E' },
    { "Linux",            no_argument, NULL, 'L' },
    { "re-read",          no_argument, NULL, 'R' },
    { "list-types",       no_argument, NULL, 'T' },
    { "unhide",     optional_argument, NULL, 'U' },
    { "no-reread",        no_argument, NULL, 160 },
    { "IBM",              no_argument, NULL, 161 },
    { "leave-last",       no_argument, NULL, 161 },
    /* undocumented flags - not all completely implemented */
    { "in-order",         no_argument, NULL, 128 },
    { "not-in-order",     no_argument, NULL, 129 },
    { "inside-outer",     no_argument, NULL, 130 },
    { "not-inside-outer", no_argument, NULL, 131 },
    { "nested",           no_argument, NULL, 132 },
    { "chained",          no_argument, NULL, 133 },
    { "onesector",        no_argument, NULL, 134 },
    { NULL, 0, NULL, 0 }
};

//static int disk_m_partition(char *dev, int cntOfPartitions, int *dividedPercentage);

int total_size;

#if	0
int
main(int argc, char **argv){
    int part_percent[4];
    part_percent[0] = 24;
    part_percent[1] = 34;
    part_percent[2] = 33;
    part_percent[3] = 9;
    disk_m_partition(argv[1], 4, part_percent);
    return 0;
}
#endif

/*
 *  H. Listing the current situation
 */

static int
my_open (char *dev, int rw, int silent) {
    int fd, mode;

    mode = (rw ? O_RDWR : O_RDONLY);
    fd = open(dev, mode);
    if (fd < 0 && !silent) {
        perror(dev);
        fatal(_("cannot open %s %s\n"), dev, rw ? _("read-write") : _("for reading"));
    }
    return fd;
}

static int asc_to_index(char *pnam, struct disk_desc *z)
{
    int pnum, pno;

    if (*pnam == '#') {
        pno = atoi(pnam+1);
    } else {
        pnum = atoi(pnam);
        pno = linux_to_index(pnum, z);
    }
    if (!(pno >= 0 && pno < z->partno))
        fatal(_("%s: no such partition\n"), pnam);
    return pno;
}


//"usage: sfdisk --id device partition-number [Id]\n"
void do_change_id(char *dev, char *pnam, char *id)
{
    int fd, rw, pno;
    struct disk_desc *z;
    unsigned long i;

    z = &oldp;

    rw = !no_write;
    fd = my_open(dev, rw, 0);

    free_sectors();
    get_cylindersize(dev, fd, 1);
    get_partitions(dev, fd, z);

    pno = asc_to_index(pnam, z);
    if (id == 0) {
        SYSTEM_LOG("%x\n", z->partitions[pno].p.sys_type);
        return;
    }
    i = strtoul(id, NULL, 16);
    if (i > 255)
        fatal(_("Bad Id %x\n"), i);
    z->partitions[pno].p.sys_type = i;

    if(write_partitions(dev, fd, z))
        warn(_("Done\n\n"));
    else
        exit_status = 1;
}

/*
   char *sys_type = "83";
   do_change_id("/dev/sda", "2", sys_type);
   do_change_id("/dev/sda", "1", "c");

*/
/*
 *  I. Writing the new situation
 */
int disk_partition_create(char *dev, int cntOfPartitions, int *dividedPercentage, int systype)
{
    int fd;
    struct stat statbuf;
    int interactive = isatty(0);
    struct disk_desc *z;
    int i;

    set_sys_type(systype);
    //check the param
    if(cntOfPartitions > 4 || cntOfPartitions < 0){
        fatal(_("Error count of partitions given\n"));
        return 1;
    }
    if(cntOfPartitions > 0 && dividedPercentage == NULL){
        fatal(_("Error divided part given\n"));
        return 1;
    }
    int total = 0;
    for(i = 0;i < cntOfPartitions;i++){
        if(dividedPercentage[i] <= 100 && dividedPercentage[i] >= 0){
            total += dividedPercentage[i];
        } else {
            fatal(_("Error divided part given\n"));
            return 1;
        }
    }
    if(total <= 0 || total > 100)	{
        fatal(_("Error divided part given\n"));
        return 1;
    }		
    if (stat(dev, &statbuf) < 0) {
        perror(dev);
        fatal(_("Fatal error: cannot find %s\n"), dev);
        return 2;
    }
    if (!S_ISBLK(statbuf.st_mode)) {
        warn(_("Warning: %s is not a block device\n"), dev);
        no_reread = 1;
    }
    fd = my_open(dev, !no_write, 0);
    //->
    struct geometry R;

    R = get_geometry(dev, fd, 0);
    SYSTEM_LOG("******  %lu\n", R.cylinders);
    unsigned long remain_cylinders = R.cylinders;
    if(R.cylinders > 0) {
        countOfPartition = cntOfPartitions;
        for(i = 0;i < 4;i++)
        {
            /*
               if(i == cntOfPartitions - 1)
               {
               sprintf((char *)(partitionInputInfo[i].partitionInput[0]), "%lu", R.cylinders - remain_cylinders);
               sprintf((char *)(partitionInputInfo[i].partitionInput[1]), "%lu", remain_cylinders);
               }
               else */if(i <= cntOfPartitions - 1)
            {
                sprintf((char *)(partitionInputInfo[i].partitionInput[0]), "%lu", R.cylinders - remain_cylinders);
                if(remain_cylinders >= R.cylinders*dividedPercentage[i]/100)
                    sprintf((char *)(partitionInputInfo[i].partitionInput[1]), "%lu", R.cylinders*dividedPercentage[i]/100);
                else
                    sprintf((char *)(partitionInputInfo[i].partitionInput[1]), "%lu", remain_cylinders);
                remain_cylinders -= R.cylinders*dividedPercentage[i]/100;
            }
            else
            {
                sprintf((char *)(partitionInputInfo[i].partitionInput[0]), "%d", 0);
                sprintf((char *)(partitionInputInfo[i].partitionInput[1]), "%d", 0);
            }
            SYSTEM_LOG("-------->%s - %s\n", partitionInputInfo[i].partitionInput[0], partitionInputInfo[i].partitionInput[1]);
        }
    }  else {
        return 2;
    }
    if(!no_write && !no_reread)    {
        warn(_("Checking that no-one is using this disk right now ...\n"));
        if(reread_ioctl(fd)) {
            SYSTEM_LOG(_("\nThis disk is currently in use.\n"));
            if (!force) {
                SYSTEM_LOG(_("Use the --force flag to overrule all checks.\n"));
                return 3;
            }
        } else
            warn(_("OK\n"));
    }		
    z = &oldp;
    free_sectors();
    get_cylindersize(dev, fd, 0);
    get_partitions(dev, fd, z);
    SYSTEM_LOG(_("Old situation:\n"));
    out_partitions(dev, z);

    if (one_only && (one_only_pno = linux_to_index(one_only, z)) < 0)
        fatal(_("Partition %d does not exist, cannot change it\n"), one_only);

    z = &newp;

    //while(1) 
    {
        read_input(dev, interactive, z);

        SYSTEM_LOG(_("New situation:\n"));
        out_partitions(dev, z);
        if (!partitions_ok(z) && !force) {
            if(!interactive)
                fatal(_("I don't like these partitions - nothing changed.\n"
                        "(If you really want this, use the --force option.)\n"));
            else
                SYSTEM_LOG(_("I don't like this - probably you should answer No\n"));
        }
    }

    if(write_partitions(dev, fd, z))
        SYSTEM_LOG(_("Successfully wrote the new partition table\n\n"));
    else
        exit_status = 1;
    reread_disk_partition(dev, fd);
    warn(_("If you created or changed a DOS partition, /dev/foo7, say, then use dd(1)\n"
            "to zero the first 512 bytes:  dd if=/dev/zero of=/dev/foo7 bs=512 count=1\n"
            "(See fdisk(8).)\n"));
    sync();			/* superstition */
    return 0;
}
