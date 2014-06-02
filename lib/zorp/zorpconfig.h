/* lib/zorp/zorpconfig.h.  Generated from zorpconfig.h.in by configure.  */
/* lib/zorp/zorpconfig.h.in.  Generated from configure.ac by autoheader.  */

/* Zorp brochure version */
#define BROCHURE_VERSION "5"

/* Enable debugging */
#define ENABLE_DEBUG 0

/* Enable IP option processing */
#define ENABLE_IPOPTIONS 0

/* Enable IPv6 support */
#define ENABLE_IPV6 1

/* Enable prefork support */
#define ENABLE_PREFORK 0

/* Enable trace messages */
#define ENABLE_TRACE 0

/* define if the compiler supports basic C++11 syntax */
#define HAVE_CXX11 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `gethostbyname_r' function. */
#define HAVE_GETHOSTBYNAME_R 1

/* Define to 1 if you have the `inet_aton' function. */
#define HAVE_INET_ATON 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Have MSG_PROXY flag (Linux 2.2) */
#define HAVE_MSG_PROXY 1

/* Define to 1 if you have the `prctl' function. */
#define HAVE_PRCTL 1

/* Zorp may enable core_dumping Linux 2.4- */
#define HAVE_PR_SET_DUMPABLE 1

/* Define to 1 if you have the `select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define to 1 if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* We have a Solaris style gethostbyname_r; */
/* #undef HAVE_SUN_GETHOSTBYNAME_R */

/* Define to 1 if you have the <sys/capability.h> header file. */
#define HAVE_SYS_CAPABILITY_H 1

/* Define to 1 if you have the <sys/prctl.h> header file. */
#define HAVE_SYS_PRCTL_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/syscall.h> header file. */
#define HAVE_SYS_SYSCALL_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the `vsnprintf' function. */
#define HAVE_VSNPRINTF 1

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#define LT_OBJDIR ".libs/"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME ""

/* Define to the full name and version of this package. */
#define PACKAGE_STRING ""

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME ""

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION ""

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P 8

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Zorp package version */
#define VERSION "5.0.0"

/* Configuration date */
#define ZORP_CONFIG_DATE "2014/05/27"

/* datadir */
#define ZORP_DATADIR "/home/coroner/src/zorp/share/zorp"

/* libdir */
#define ZORP_LIBDIR "/home/coroner/src/zorp/lib/zorp"

/* Required license version */
#define ZORP_LICENSE_VERSION "5.0"

/* pidfiledir */
#define ZORP_PIDFILEDIR "/home/coroner/src/zorp/var/run/zorp/"

/* Required product name in license */
#define ZORP_PRODUCT_NAME "Zorp Professional"

/* Zorp source revision number */
#define ZORP_SOURCE_REVISION "ssh+git://coroner@git.balabit/var/scm/git/zorp/zorp-core--mainline--5.0#f/B-09091-gpl-fixes#6d10e6871d0420cb863e42d9f2399373b024294a"

/* localstatedir */
#define ZORP_STATEDIR "/home/coroner/src/zorp/var"

/* sysconfdir */
#define ZORP_SYSCONFDIR "/home/coroner/src/zorp/etc/zorp"

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */
