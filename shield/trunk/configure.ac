#
# This file is the main build configuration file for shield. It is used
# to determine your systems capabilities, and tries to adapt shield to
# take maximum advantage of the services your system offers.
#
# Process this file using the 'autoconf' command to produce a working
# configure script, which should in turn be executed in order to
# configure the build process.
#

AC_INIT(shield,0.1,axel.liljencrantz@freecode.no)

#
# If needed, run autoconf to regenerate the configure file
#
# This makes sure that after running autoconf once to create the first
# version of configure, we never again need to worry about manually
# running autoconf to handle an updates configure.ac.
#

AC_MSG_CHECKING([if autoconf needs to be run])
if test configure -ot configure.ac; then
    AC_MSG_RESULT([yes])
    if which autoconf >/dev/null; then
        # No need to provide any error messages if autoconf fails, the
        # shell and autconf should take care of that themselves
        AC_MSG_NOTICE([running autoconf])
        if autoconf; then
            ./configure "$@"
            exit
        fi
        exit 1
    else
        AC_MSG_ERROR(
            [cannot find the autoconf program in your path.
This program needs to be run whenever the configure.ac file is modified.
Please install it and try again.]
        )
    fi
else
    AC_MSG_RESULT([no])
fi


#
# Set up various programs needed for install
#

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CXXCPP
AC_PROG_INSTALL


#
# Detect directories which may contain additional headers, libraries
# and commands. This needs to be done early - before Autoconf starts
# to mess with CXXFLAGS and all the other environment variables.
#

for i in /usr/lib/oracle/xe/app/oracle/product/*/server/rdbms /usr/lib/oracle/xe/app/oracle/product/*/client/rdbms /usr/lib/oracle/xe/app/oracle/product/*/server /usr/lib/oracle/xe/app/oracle/product/*/client; do

	AC_MSG_CHECKING([for $i/include include directory])
	if test -d $i/include; then
		AC_MSG_RESULT(yes)
		CPPFLAGS="$CPPFLAGS -I$i/include/"
		CXXFLAGS="$CXXFLAGS -I$i/include/"
	else
	AC_MSG_RESULT(no)
	fi

	AC_MSG_CHECKING([for $i/public include directory])
	if test -d $i/public; then
		AC_MSG_RESULT(yes)
		CPPFLAGS="$CPPFLAGS -I$i/public/"
		CXXFLAGS="$CXXFLAGS -I$i/public/"
	else
	AC_MSG_RESULT(no)
	fi

	AC_MSG_CHECKING([for $i/lib library directory])
	if test -d $i/lib; then
		AC_MSG_RESULT(yes)
		LDFLAGS="$LDFLAGS -L$i/lib/ -Wl,-R$i/lib/"
	else
		AC_MSG_RESULT(no)
	fi

done

if test "$GXX" = yes; then
	LDFLAGS="$LDFLAGS -rdynamic"
	CXXFLAGS="$CXXFLAGS -pipe"
fi

# Tell the world what we know
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "shield is now configured."
echo "Use 'make' and 'make install' to build and install shield."

