#!/bin/sh
#
# Copyright (C) 2000-2003 the xine project
# Copyright (C) 2004 Hypnos UO Server Emulator
#
# This file is part of Hypnos UO Server Emulator
# Modified version of the autogen.sh script took from xine-lib.
# 
# Hypnos is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# Hypnos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
#
# $Id: autogen.sh,v 1.15 2004/09/05 02:14:39 storri Exp $
#
# Maintained by Diego Petten� <flameeyes@users.berlios.de>
#
# run this to generate all the initial makefiles, etc.

PROG=Hypnos

# Minimum value required to build
WANT_AUTOMAKE=1.6 export WANT_AUTOMAKE
AUTOMAKE_MIN=1.6.0
AUTOCONF_MIN=2.53
LIBTOOL_MIN=1.4.0

# Check how echo works in this /bin/sh
case `echo -n` in
-n)     _echo_n=   _echo_c='\c';;
*)      _echo_n=-n _echo_c=;;
esac

detect_configure_ac() {

  srcdir=`dirname $0`
  test -z "$srcdir" && srcdir=.

  (test -f $srcdir/configure.in) || {
    echo $_echo_n "*** Error ***: Directory "\`$srcdir\`" does not look like the"
    echo " top-level directory"
    exit 1
  }
}


#--------------------
# AUTOCONF
#-------------------
detect_autoconf() {
  set -- `type autoconf 2>/dev/null`
  RETVAL=$?
  NUM_RESULT=$#
  RESULT_FILE=$3
  if [ $RETVAL -eq 0 -a $NUM_RESULT -eq 3 -a -f "$RESULT_FILE" ]; then
    AC="`autoconf --version | sed -n 1p | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    AUTOCONF_MIN="`echo $AUTOCONF_MIN | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    if test $AC -lt 100 ; then
      AC=`expr $AC \* 10`
    fi
    if [ `expr $AC` -ge $AUTOCONF_MIN ]; then
      autoconf_ok=yes
    fi
  else
    echo
    echo "**Error**: You must have \`autoconf' >= $AUTOCONF_MIN installed to" 
    echo "           compile $PROG. Download the appropriate package"
    echo "           for your distribution or source from ftp.gnu.org."
    exit 1
  fi
}

run_autoheader () {
  if test x"$autoconf_ok" != x"yes"; then
    echo
    echo "**Warning**: Version of autoconf is less than $AUTOCONF_MIN."
    echo "             Some warning message might occur from autoconf"
    echo
  fi

  echo $_echo_n " + Running autoheader: $_echo_c";
    autoheader;
  echo "done."
}

run_autoconf () {
  if test x"$autoconf_ok" != x"yes"; then
    echo
    echo "**Warning**: Version of autoconf is less than $AUTOCONF_MIN."
    echo "             Some warning message might occur from autoconf"
    echo
  fi

  echo $_echo_n " + Running autoconf: $_echo_c";
    autoconf;
  echo "done."
}

#--------------------
# LIBTOOL
#-------------------
try_libtool_executable() {
  libtool=$1
  set -- `type $libtool 2>/dev/null`
  RETVAL=$?
  NUM_RESULT=$#
  RESULT_FILE=$3
  if [ $RETVAL -eq 0 -a $NUM_RESULT -eq 3 -a -f "$RESULT_FILE" ]; then
    LT="`$libtool --version | awk '{ print $4 }' | sed -e 's/[a-zA-Z\ \.\(\)\-\;]//g'`"
    LIBTOOL_MIN="`echo $LIBTOOL_MIN | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    if test $LT -lt 100 ; then
      LT=`expr $LT \* 10`
    fi
    if [ `expr $LT` -ge $LIBTOOL_MIN ]; then
      libtool_ok=yes
    fi
  fi
}

detect_libtool() {
  # try glibtool first, then libtool
  try_libtool_executable 'glibtool'
  if [ "x$libtool_ok" != "xyes" ]; then
    try_libtool_executable 'libtool'
    if [ "x$libtool_ok" != "xyes" ]; then
      echo
      echo "**Error**: You must have \`libtool' >= $LIBTOOL_MIN installed to" 
      echo "           compile $PROG. Download the appropriate package"
      echo "           for your distribution or source from ftp.gnu.org."
      exit 1
    fi
  fi
}

run_libtoolize() {
  if test x"$libtool_ok" != x"yes"; then
    echo
    echo "**Warning**: Version of libtool is less than $LIBTOOL_MIN."
    echo "             Some warning message might occur from libtool"
    echo
  fi

  echo $_echo_n " + Running libtoolize: $_echo_c";
    "${libtool}ize" --force --copy >/dev/null 2>&1;
  echo "done."
}

#--------------------
# AUTOMAKE
#--------------------
detect_automake() {
  #
  # expected output from 'type' is
  #   automake is /usr/local/bin/automake
  #
  set -- `type automake 2>/dev/null`
  RETVAL=$?
  NUM_RESULT=$#
  RESULT_FILE=$3
  if [ $RETVAL -eq 0 -a $NUM_RESULT -eq 3 -a -f "$RESULT_FILE" ]; then
    AM="`automake --version | sed -n 1p | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    AUTOMAKE_MIN="`echo $AUTOMAKE_MIN | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    if test $AM -lt 100 ; then
      AM=`expr $AM \* 10`
    fi
    if [ `expr $AM` -ge $AUTOMAKE_MIN ]; then
      automake_ok=yes
    fi
  else
    echo
    echo "**Error**: You must have \`automake' >= $AUTOMAKE_MIN installed to" 
    echo "           compile $PROG. Download the appropriate package"
    echo "           for your distribution or source from ftp.gnu.org."
    exit 1
  fi
}

run_automake () {
  if test x"$automake_ok" != x"yes"; then
    echo
    echo "**Warning**: Version of automake is less than $AUTOMAKE_MIN."
    echo "             Some warning message might occur from automake"
    echo
  fi

  echo $_echo_n " + Running automake: $_echo_c";

  automake --gnu --add-missing --copy;
  echo "done."
}

#--------------------
# ACLOCAL
#-------------------
detect_aclocal() {

  # if no automake, don't bother testing for aclocal
  set -- `type aclocal 2>/dev/null`
  RETVAL=$?
  NUM_RESULT=$#
  RESULT_FILE=$3
  if [ $RETVAL -eq 0 -a $NUM_RESULT -eq 3 -a -f "$RESULT_FILE" ]; then
    AC="`aclocal --version | sed -n 1p | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    ACLOCAL_MIN="`echo $AUTOMAKE_MIN | sed -e 's/[a-zA-Z\ \.\(\)\-]//g'`"
    if test $AC -lt 100 ; then
      AC=`expr $AC \* 10`
    fi
    if [ `expr $AC` -ge $ACLOCAL_MIN ]; then
      aclocal_ok=yes
    fi
  else
    echo
    echo "**Error**: You must have \`automake' >= $AUTOMAKE_MIN installed to" 
    echo "           compile $PROG. Download the appropriate package"
    echo "           for your distribution or source from ftp.gnu.org."
    exit 1
  fi
}

run_aclocal () {

  if test x"$aclocal_ok" != x"yes"; then
    echo
    echo "**Warning**: Version of aclocal is less than $AUTOMAKE_MIN."
    echo "             Some warning message might occur from aclocal"
    echo
  fi
  
  echo $_echo_n " + Running aclocal: $_echo_c"

  aclocal -I m4
  echo "done." 
}

#---------------
# MAIN
#---------------
detect_configure_ac
detect_autoconf
detect_libtool
detect_automake
detect_aclocal


#   help: print out usage message
#   *) run aclocal, autoheader, automake, autoconf
case "$1" in
  aclocal)
    run_aclocal
    ;;
  autoheader)
    run_autoheader
    ;;
  automake)
    run_aclocal
    run_automake
    ;;
  autoconf)
    run_aclocal
    run_autoconf
    ;;
  libtoolize)
    run_libtoolize
    ;;
  noconfig)
    run_aclocal
    run_libtoolize
    run_autoheader
    run_automake
    run_autoconf
    ;;
  *)
    run_aclocal
    run_libtoolize
    run_autoheader
    run_automake
    run_autoconf
    ;;
esac
