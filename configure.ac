###Copyright (C) 2018 Vincent Torri <vincent dot torri at gmail dot com>
###This code is public domain and can be freely used or copied.

dnl defines the version name of  the libdwarf.so
m4_define([v_maj], [0])
m4_define([v_min], [1])
m4_define([v_mic], [1])
m4_define([v_ver], [v_maj.v_min.v_mic])

m4_define([v_rel], [])
m4_define([lt_cur], [m4_eval(v_maj + v_min)])
m4_define([lt_rev], [v_mic])
m4_define([lt_age], [v_min])

### Sets the release name. 
###m4_define([v_date], [m4_esyscmd_s([date "+%Y%m%d"])])
###Also SET (VERSION ..) in CMakeLists.txt

AC_PREREQ([2.52])
### 2nd arg to AC_INIT is the version 'number'.
AC_INIT([libdwarf], [v_ver], [libdwarf-list -at- linuxmail -dot- org])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIRS([m4])

### Additional options to configure

AC_ARG_ENABLE([dwarfgen],
   [AS_HELP_STRING([--enable-dwarfgen],
                   [enable dwarfgen compilation @<:@default=no@:>@])],
   [
    AS_IF(
        [test "x${enableval}" = "xyes"],
        [enable_dwarfgen="yes" ],
        [enable_dwarfgen="no"])
   ],
   [enable_dwarfgen="no"])
AM_CONDITIONAL([HAVE_DWARFGEN], [test "x${enable_dwarfgen}" = "xyes"])

AC_ARG_ENABLE([dwarfexample],
   [AS_HELP_STRING([--enable-dwarfexample],
                   [enable dwarfexample compilation @<:@default=no@:>@])],
   [
    AS_IF(
        [test "x${enableval}" = "xyes"],
        [enable_dwarfexample="yes"],
        [enable_dwarfexample="no"])
   ],
   [enable_dwarfexample="no"])
AM_CONDITIONAL([HAVE_DWARFEXAMPLE], [test "x${enable_dwarfexample}" = "xyes"])

AC_ARG_ENABLE([sanitize],
   [AS_HELP_STRING([--enable-sanitize],
                   [enable sanitize compiler option @<:@default=no@:>@])],
   [
    AS_IF(
        [test "x${enableval}" = "xyes"],
        [enable_sanitize="yes"],
        [enable_sanitize="no"])
   ],
   [enable_sanitize="no"])

AC_ARG_ENABLE([namestable],
   [AS_HELP_STRING([--enable-namestable],
                   [enable name string functions implemented as binary search (default is with C switch) @<:@default=no@:>@])],
   [
    AS_IF(
        [test "x${enableval}" = "xyes"],
        [enable_namestable="yes"],
        [enable_namestable="no"])
   ],
   [enable_namestable="no"])


AS_IF(
    [test "x${enable_namestable}" = "xyes"],
    [AC_SUBST([dwarf_namestable], [-s])],
    [AC_SUBST([dwarf_namestable], [-t])])

AC_ARG_ENABLE([libelf],
   [AS_HELP_STRING([--disable-libelf],
     [disable use of libelf (default is enable) @<:@default=yes@:>@])],
   [ AS_IF(
        [test "x${enableval}" = "xyes"],
        [dwarf_with_libelf="yes"],
        [dwarf_with_libelf="no"])
   ],
   [dwarf_with_libelf="yes"])

AC_ARG_ENABLE([wall],
   [AS_HELP_STRING([--enable-wall],
                   [enable -Wall and other options @<:@default=no@:>@])],
   [
    AS_IF(
        [test "x${enableval}" = "xyes"],
        [enable_wall="yes"],
        [enable_wall="no"])
   ],
   [enable_wall="no"])

### Default options with respect to host

AC_CANONICAL_HOST

have_win32="no"
case "$host_os" in
   mingw*)
      have_win32="yes"
   ;;
esac

AM_CONDITIONAL([HAVE_WIN32], [test "x${have_win32}" = "xyes"])


### Checks for programs

AM_PROG_AR

### We don't use dist-xz *.xz output from make dist,
### so don't mention it.
AM_INIT_AUTOMAKE([1.6 dist-xz])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AS_IF(
    [test "x${have_win32}" = "xyes"],
    [lt_cv_deplibs_check_method=pass_all])
LT_INIT([win32-dll disable-shared static])

version_info="lt_cur:lt_rev:lt_age"
release_info="v_rel"
AC_SUBST([version_info])
AC_SUBST([release_info])

VMAJ=v_maj
AC_SUBST([VMAJ])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CC_C_O

###PKG_PROG_PKG_CONFIG Intentionally not using pkg-config

### Checks for libraries

AC_SUBST([DWARF_LIBS])
AC_ARG_VAR([DWARF_LIBS], [linker flags when linking libdwarf])
AC_SUBST([DWARFGEN_LIBS])
AC_ARG_VAR([DWARFGEN_LIBS], [extra linker flags when linking dwarfgen])

### Checks for header files

### MacOS does not have malloc.h
AC_CHECK_HEADERS([unistd.h sys/types.h regex.h malloc.h])
### for uintptr_t 
AC_CHECK_HEADERS([stdint.h inttypes.h stddef.h])

requirements_libdwarf_pc=
requirements_libdwarf_libs=

AC_CHECK_HEADERS([zlib.h])
AS_IF(
    [test "x${ac_cv_header_zlib_h}" = "xyes"],
    [ have_pc_zlib="yes" ; echo "have zlib" ; 
      DWARF_LIBS="${DWARF_LIBS} -lz" ;
      requirements_libdwarf_libs=-lz ],
    [ have_pc_zlib="no" ; echo "no zlib" ])

AC_SUBST([requirements_libdwarf_pc])
AC_SUBST([requirements_libdwarf_libs])

requirements_libdwarfp_pc=
requirements_libdwarfp_libs=

### for use in casts to uint to avoid 32bit warnings.
### Also needed by C++ cstdint
AC_TYPE_UINTPTR_T
AC_TYPE_INTPTR_T
### Now we know uintptr_t is either in stdint.h or
### is defined in config.h by configure.

# test Elf headers in the preprocessor path search
# We need Elf.h defines like Ehdr32 and relocations
# to read them with our local elf-section reader code.
# On Solaris libelf.h has relocation defines we need.
# so we check for libelf.h too.
CPPFLAGS_save=${CPPFLAGS}
# dwarfgen needs an actual libelf else it cannot be built.
### we set $dwarf_with_libelf above.

if test  "$dwarf_with_libelf" = "yes" ; then
  AC_CHECK_HEADERS([sgidefs.h])
  AC_CHECK_HEADERS([libelf.h libelf/libelf.h])
  AC_CHECK_HEADERS([elf.h elfaccess.h sys/elf_386.h sys/elf_amd64.h sys/elf_SPARC.h sys/ia64/elf.h])
 
  ### if no libelf.h add no -lelf and turn off
  ### libelf recognition.
  ### if no dwarfgen do not add -lelf
  AS_IF([test "x${ac_cv_header_libelf_h}" != "xyes" -a "x${ac_cv_header_libelf_libelf_h}" != "xyes" ],
    [ dwarf_with_libelf="no"
      echo "no libelf headers, so no libelf" ],
    [ AS_IF([ test "x${enable_dwarfgen}" != "xyes" ], 
      [ dwarf_with_libelf="no"
        echo "dwarfgen not needed, so no libelf"],
      [ DWARFGEN_LIBS="${DWARFGEN_LIBS} -ldwarf -lelf"
        dwarf_with_libelf="yes"
        echo "Allowing use of libelf for dwarfgen."
        AC_DEFINE([DWARF_WITH_LIBELF],[1],
          [Set to 1 as we are building dwarfgen with libelf])])
    ])
### begin checking for Elf structs
# Elf64_Rela in elf.h
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
         [[
#ifdef HAVE_ELF_H
# include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif
#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
         ]],
         [[
Elf64_Rela p;
p.r_offset = 1;
         ]])
    ],
    [
     AC_DEFINE([HAVE_ELF64_RELA], [1], [Set to 1 if Elf64_Rela defined in elf.h.])
     have_elf64_rela="yes"
    ],
    [have_elf64_rela="no"])

AS_IF(
    [test "x${have_elf64_rela}" = "xno"],
    [
     CPPFLAGS_save=${CPPFLAGS}
     CPPFLAGS="${CPPFLAGS} -D__LIBELF64"
     AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
              [[
#ifdef HAVE_ELF_H
# include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif
#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
              ]],
              [[
Elf64_Rela p;
p.r_offset = 1;
              ]])
         ],
         [
          AC_DEFINE([HAVE_ELF64_RELA], [1], [Set to 1 if Elf64_Rela defined in elf.h.])
          have_elf64_rela="yes"
         ],
         [have_elf64_rela="no"])
     CPPFLAGS=${CPPFLAGS_save}
    ])

AC_MSG_CHECKING([for Elf64_Rela in elf.h])
AC_MSG_RESULT([${have_elf64_rela}])
# Elf64_Rel in elf.h
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
         [[
#ifdef HAVE_ELF_H
# include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif
#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
         ]],
         [[
Elf64_Rel p;
p.r_info = 1;
         ]])
    ],
    [
     AC_DEFINE([HAVE_ELF64_R_INFO], [1], [Set to 1 if Elf64_Rel structure as r_info field.])
     have_elf64_rel="yes"
    ],
    [have_elf64_rel="no"])

AS_IF(
    [test "x${have_elf64_rel}" = "xno"],
    [
     CPPFLAGS_save=${CPPFLAGS}
     CPPFLAGS="${CPPFLAGS} -D__LIBELF64"
     AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
              [[
#ifdef HAVE_ELF_H
# include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif

#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
              ]],
              [[
Elf64_Rel p;
p.r_info = 1;
              ]])
         ],
         [
          AC_DEFINE([HAVE_ELF64_R_INFO], [1], [Set to 1 if Elf64_Rel structure as r_info field.])
          have_elf64_rel="yes"
         ],
         [have_elf64_rel="no"])
     CPPFLAGS=${CPPFLAGS_save}
    ])

AC_MSG_CHECKING([for Elf64_Rel in elf.h])
AC_MSG_RESULT([${have_elf64_rel}])

# Elf64_Sym in elf.h
AC_LINK_IFELSE(
    [AC_LANG_PROGRAM(
         [[
#ifdef HAVE_ELF_H
# include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif
#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
         ]],
         [[
Elf64_Sym p;
p.st_info = 1;
         ]])
    ],
    [
     AC_DEFINE([HAVE_ELF64_SYM], [1], [Set to 1 if Elf64_Sym defined in elf.h.])
     have_elf64_sym="yes"
    ],
    [have_elf64_sym="no"])

AS_IF(
    [test "x${have_elf64_sym}" = "xno"],
    [
     CPPFLAGS_save=${CPPFLAGS}
     CPPFLAGS="${CPPFLAGS} -D__LIBELF64"
     AC_LINK_IFELSE(
         [AC_LANG_PROGRAM(
              [[
#ifdef HAVE_ELF_H
# include <elf.h>
#endif
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif
#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
              ]],
              [[
Elf64_Sym p;
p.st_info = 1;
              ]])
         ],
         [
          AC_DEFINE([HAVE_ELF64_SYM], [1], [Set to 1 if Elf64_Sym defined in elf.h.])
          have_elf64_sym="yes"
         ],
         [have_elf64_sym="no"])
     CPPFLAGS=${CPPFLAGS_save}
    ])

AC_MSG_CHECKING([for Elf64_Sym in elf.h])
AC_MSG_RESULT([${have_elf64_sym}])
### end checking for Elf structs
### Checks for Elf structures
# test if struct _Elf is used instead of struct Elf
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
         [[
#ifdef HAVE_LIBELF_H
# include <libelf.h>
#endif
#ifdef HAVE_LIBELF_LIBELF_H
# include <libelf/libelf.h>
#endif
/* This must be at global scope */
struct _Elf;
typedef struct _Elf Elf;
         ]],
         [[
struct _Elf *a = 0;
         ]])
    ],
    [
     have_struct__elf="yes"
     struct_elf="struct _Elf"
    ],
    [
     have_struct__elf="no"
     struct_elf="struct Elf"
    ])

AC_MSG_CHECKING([whether struct _Elf is used])
AC_MSG_RESULT([${have_struct__elf}])
AC_MSG_CHECKING([whether struct _Elf is used])
AC_MSG_RESULT([${have_struct__elf}])
else
  ### end where dwarf_with_libelf == "no" via
  ### --disable-libelf
  have_struct__elf="no"
  struct_elf="struct Elf"
  have_elf64_rela="no"
  have_elf64_rel="no"
  have_elf64_sym="no"
fi
AC_SUBST([struct_elf])




### Checks for compiler characteristics

AC_C_BIGENDIAN
AC_SUBST([DWARF_BIGENDIAN],[${ac_cv_c_bigendian}])
AC_ARG_VAR([DWARF_BIGENDIAN],
    [big endian yes/no needed various places])


# gcc accepts even totally bogus -Wno flags. Other compilers..no
# -Wno-long-long suppresses warnings on 'long long'
# -Wno-pedantic-ms-format (which only exists in mingw)
# suppresses warnings about I64 printf format.
c_common_flags="-Wall -Wextra -Wpointer-arith -Wmissing-declarations -Wcomment -Wformat -Wpedantic -Wuninitialized -Wshadow -Werror -Wno-long-long"
AS_IF(
    [ test "x$enable_wall" = "xyes" ],
    [
       cxx_compiler_flags="$c_common_flags -Wno-unused-private-field"

       c_compiler_flags="$c_common_flags -Wmissing-prototypes -Wdeclaration-after-statement -Wbad-function-cast -Wmissing-parameter-type -Wnested-externs"
    ]
    )
AS_IF(
    [ test "x$have_win32" = "xyes" ],
    [
       cxx_compiler_flags="$cxx_compiler_flags -Wno-pedantic-ms-format"

       c_compiler_flags="$c_compiler_flags -Wno-pedantic-ms-format"
    ]
    )

DWARF_CHECK_CXX_COMPILER_FLAGS([${cxx_compiler_flags}])
DWARF_CHECK_C_COMPILER_FLAGS([${c_compiler_flags}])

# unused attribute
AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
         [[
static unsigned int foo(unsigned int x, __attribute__ ((unused)) int y){
    unsigned int x2 = x + 1;
    return x2;
}
int goo() {
    unsigned int y = 0;
    y = foo(12, y);
}
         ]],
         [[
         ]])
    ],
    [
     have_unused="yes"
     AC_DEFINE(
         [HAVE_UNUSED_ATTRIBUTE], [1],
         [Set to 1 if __attribute__ ((unused)) is available.])
    ],
    [have_unused="no"])

AC_MSG_CHECKING([whether "unused" attribute is available])
AC_MSG_RESULT([${have_unused}])

# sanitize
AS_IF(
    [test "x${enable_sanitize}" = "xyes"],
    [
     CFLAGS_save=${CFLAGS}
     CFLAGS="${CFLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer"
     DWARF_CFLAGS=
     AC_COMPILE_IFELSE(
         [AC_LANG_PROGRAM([[]], [[]])],
         [
          enable_sanitize="yes"
          DWARF_CFLAGS="$DWARF_CFLAGS -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer"
          LDFLAGS="$LDFLAGS -fsanitize=address -fsanitize=leak -fsanitize=undefined -fno-omit-frame-pointer"
         ],
         [enable_sanitize="no"])
     CFLAGS="${CFLAGS_save} ${DWARF_CFLAGS}" 
     DWARF_CFLAGS=
    ])

AC_MSG_CHECKING([whether sanitize options are used])
AC_MSG_RESULT([${enable_sanitize}])


### Checks for linker characteristics


### Checks for library functions

AC_FUNC_ALLOCA

have_getehdr="no"
have_getshdr="no"
CPPFLAGS_save=${CPPFLAGS}
CPPFLAGS="${CPPFLAGS}"
LIBS_save=${LIBS}
LIBS="${LIBS} ${DWARF_LIBS}"
AC_SEARCH_LIBS(
    [elf64_getehdr], [elf],
    [
     have_getehdr="yes"
     AC_DEFINE(
         [HAVE_ELF64_GETEHDR], [1],
         [Set to 1 if the elf64_getehdr function is in libelf.])
     ],
     [have_getehdr="no"])
AC_SEARCH_LIBS(
    [elf64_getshdr], [elf],
    [
     have_getshdr="yes"
     AC_DEFINE(
         [HAVE_ELF64_GETSHDR], [1],
         [Set to 1 if the elf64_getshdr function is in libelf.])
     ],
     [have_getshdr="no"])
CPPFLAGS=${CPPFLAGS_save}
LIBS=${LIBS_save}

AS_IF(
    [test "x${have_pc_zlib}" = "xno"  -a "x${have_zlib}" = "xyes"],
    [AC_SEARCH_LIBS([z], [have_zlib="yes"], [have_zlib="no"])])

AS_IF(
    [test "x${have_pc_zlib}" = "xyes"  -o "x${have_zlib}" = "xyes"],
    [
     have_zlib="yes"
     AC_DEFINE([HAVE_ZLIB], [1], [Set to 1 if zlib decompression is available.])
     AC_DEFINE([HAVE_ZLIB_H], [1], [Set to 1 if zlib.h header file is available.])
    ],
    [
     have_zlib="no"
    ])

AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
         [[
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_REGEX_H
# include <regex.h>
#endif
         ]],
         [[
int i;
regex_t r;
int cflags = REG_EXTENDED;
const char *s = "abc";
i = regcomp(&r,s,cflags);
regfree(&r);
         ]])
    ],
    [
     AC_DEFINE([HAVE_REGEX], [1], [Set to 1 if regex is usable.])
     have_regex="yes"
    ],
    [have_regex="no"])

AC_MSG_CHECKING([for regex library])
AC_MSG_RESULT([${have_regex}])


### Checks for system services

AC_CONFIG_FILES([
Makefile
src/lib/libdwarf/Makefile
src/lib/libdwarfp/Makefile
src/bin/dwarfdump/Makefile
src/bin/dwarfgen/Makefile
src/bin/dwarfexample/Makefile
test/Makefile
doc/Makefile
libdwarf.pc
libdwarfp.pc
])

AC_OUTPUT

echo
echo "$PACKAGE $VERSION"
echo
echo "Configuration Options Summary:"
echo
echo "  BuildOS..............: ${build_os}"
echo "  HostOS...............: ${host_os}"
echo
echo "  shared library.......: ${enable_shared}"
echo "  static library.......: ${enable_static}"
echo
echo "  zlib support.........: ${have_zlib}"
echo "  sanitize support.....: ${enable_sanitize}"
echo "  BuildOS-BigEndian....: ${ac_cv_c_bigendian}"
echo
echo "  libdwarf.............: always"
echo "    namestable binary..: ${enable_namestable}"
echo "    elf64_getehdr......: ${have_getehdr}"
echo "    elf64_getshdr......: ${have_getshdr}"
echo "    Elf64_Rela.........: ${have_elf64_rela}"
echo "    Elf64_Sym..........: ${have_elf64_sym}"
echo "    Elf spelled........: ${struct_elf}"
echo "    libelf.............: ${dwarf_with_libelf}"
echo "  dwarfdump............: always"
echo "    regex..............: ${have_regex}"
echo "  dwarfgen.............: ${enable_dwarfgen}"
echo "  dwarfexample.........: ${enable_dwarfexample}"
echo
echo "Compilation............: make (or gmake)"
echo "  CPPFLAGS.............: $CPPFLAGS"
echo "  CFLAGS...............: $CFLAGS ${c_compiler_flags}"
echo "  LDFLAGS..............: $LDFLAGS"
echo "  LIBS.................: $LIBS"
echo "  DWARF_LIBS...........: $DWARF_LIBS"
echo "  DWARFGEN_LIBS........: $DWARFGEN_LIBS"
echo
echo "Installation...........: make install (as root if needed, with 'su' or 'sudo')"
echo "  prefix...............: $prefix"
echo
