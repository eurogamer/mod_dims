AC_PREREQ(2.60)
AC_INIT(mod_dims, 3.3.0, [jeremy.collins@beetlebug.org])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/mod_dims.c])

# For custom installs.
LDFLAGS+=" -L/usr/local/lib"
CFLAGS+=" -I/usr/local/include"

# For MacPorts.
LDFLAGS+=" -L/opt/local/lib"
CFLAGS+=" -L/opt/local/include"

# Checks for programs.
AC_PROG_CC
AC_PROG_LIBTOOL

# Check for ImageMagic
AC_ARG_WITH(graphicsmagick, [  --with-graphicsmagick        Specify installation directory for GraphicsMagick library], [
    graphicsmagick_dir=$with_graphicsmagick
    LDFLAGS+=" -L$graphicsmagick_dir/lib"
    CFLAGS+=" -I$graphicsmagick_dir/include/GraphicsMagick"
],[
    CFLAGS+=" -I/usr/local/include/GraphicsMagick"
    CFLAGS+=" -I/opt/local/include/GraphicsMagick"
])
AC_SUBST(graphicsmagick_dir)

AC_CHECK_LIB(MagickWand, MagickWandGenesis,, [
    AC_CHECK_LIB(Wand, MagickWandGenesis,, [AC_MSG_ERROR(GraphicsMagick not found.)])
])
AC_CHECK_HEADER(wand/magick_wand.h)

# Check for libcurl
AC_ARG_WITH(curl, [  --with-curl        Specify installation directory for Curl library], [
    curl_dir=$with_curl
    LDFLAGS+=" -L$curl_dir/lib"
],[])
AC_SUBST(curl_dir)

AC_CHECK_LIB(curl, curl_global_init,, [AC_MSG_ERROR(curl not found.)])
AC_CHECK_HEADER(curl/curl.h)

# Check for Apache
AC_ARG_WITH(apache, [  --with-apache        Specify installation directory for Apache 2.2.x], [
    LDFLAGS+=" -L$with_apache/lib"
    CFLAGS+=" -I$with_apache/include"
    AC_PATH_PROGS([APXS], apxs2 apxs, , $with_apache/bin)
    AC_CHECK_HEADER(apr.h,, [AC_MSG_ERROR(Apache Portable Runtime headers not found.)])
    AC_CHECK_HEADER(httpd.h,, [AC_MSG_ERROR(Apache headers not found.)])
],[
    # Look for Apache Portable Runtime
    if test -d /usr/include/apr-1
    then
        CFLAGS+=" -I/usr/include/apr-1"
    elif test -d /usr/include/apr-1.0
    then
        CFLAGS+=" -I/usr/include/apr-1.0"
    fi

    CFLAGS+=" -D_LARGEFILE64_SOURCE -I/usr/include/apache2"

    AC_CHECK_HEADER(apr.h,, [AC_MSG_ERROR(Apache Portable Runtime headers not found.)])
    AC_CHECK_HEADER(httpd.h,, [AC_MSG_ERROR(Apache headers not found.)])
    AC_PATH_PROG([APXS], apxs)
])



# Checks for header files.
AC_CHECK_HEADERS([stdlib.h sys/time.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset sqrt strstr])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
