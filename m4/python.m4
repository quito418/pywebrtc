AC_DEFUN([AX_PYTHON_VERSION], [
    AC_MSG_CHECKING([for python version $1 or higher])
    AX_PROG_PYTHON_VERSION([$1], [], [
        exit -1
    ])
    AS_IF([test $? -eq 0], [], [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([You need at least version $1 of Python module.])
    ])
])

AC_DEFUN([AX_COMPARE_VERSION], [
  AC_REQUIRE([AC_PROG_AWK])

  # Used to indicate true or false condition
  ax_compare_version=false

  # Convert the two version strings to be compared into a format that
  # allows a simple string comparison.  The end result is that a version
  # string of the form 1.12.5-r617 will be converted to the form
  # 0001001200050617.  In other words, each number is zero padded to four
  # digits, and non digits are removed.
  AS_VAR_PUSHDEF([A],[ax_compare_version_A])
  A=`echo "$1" | sed -e 's/\([[0-9]]*\)/Z\1Z/g' \
                     -e 's/Z\([[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/[[^0-9]]//g'`

  AS_VAR_PUSHDEF([B],[ax_compare_version_B])
  B=`echo "$3" | sed -e 's/\([[0-9]]*\)/Z\1Z/g' \
                     -e 's/Z\([[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/Z\([[0-9]][[0-9]][[0-9]]\)Z/Z0\1Z/g' \
                     -e 's/[[^0-9]]//g'`

  dnl # In the case of le, ge, lt, and gt, the strings are sorted as necessary
  dnl # then the first line is used to determine if the condition is true.
  dnl # The sed right after the echo is to remove any indented white space.
  m4_case(m4_tolower($2),
  [lt],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort -r | sed "s/x${A}/false/;s/x${B}/true/;1q"`
  ],
  [gt],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort | sed "s/x${A}/false/;s/x${B}/true/;1q"`
  ],
  [le],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort | sed "s/x${A}/true/;s/x${B}/false/;1q"`
  ],
  [ge],[
    ax_compare_version=`echo "x$A
x$B" | sed 's/^ *//' | sort -r | sed "s/x${A}/true/;s/x${B}/false/;1q"`
  ],[
    dnl Split the operator from the subversion count if present.
    m4_bmatch(m4_substr($2,2),
    [0],[
      # A count of zero means use the length of the shorter version.
      # Determine the number of characters in A and B.
      ax_compare_version_len_A=`echo "$A" | $AWK '{print(length)}'`
      ax_compare_version_len_B=`echo "$B" | $AWK '{print(length)}'`

      # Set A to no more than B's length and B to no more than A's length.
      A=`echo "$A" | sed "s/\(.\{$ax_compare_version_len_B\}\).*/\1/"`
      B=`echo "$B" | sed "s/\(.\{$ax_compare_version_len_A\}\).*/\1/"`
    ],
    [[0-9]+],[
      # A count greater than zero means use only that many subversions
      A=`echo "$A" | sed "s/\(\([[0-9]]\{4\}\)\{m4_substr($2,2)\}\).*/\1/"`
      B=`echo "$B" | sed "s/\(\([[0-9]]\{4\}\)\{m4_substr($2,2)\}\).*/\1/"`
    ],
    [.+],[
      AC_WARNING(
        [invalid OP numeric parameter: $2])
    ],[])

    # Pad zeros at end of numbers to make same length.
    ax_compare_version_tmp_A="$A`echo $B | sed 's/./0/g'`"
    B="$B`echo $A | sed 's/./0/g'`"
    A="$ax_compare_version_tmp_A"

    # Check for equality or inequality as necessary.
    m4_case(m4_tolower(m4_substr($2,0,2)),
    [eq],[
      test "x$A" = "x$B" && ax_compare_version=true
    ],
    [ne],[
      test "x$A" != "x$B" && ax_compare_version=true
    ],[
      AC_WARNING([invalid OP parameter: $2])
    ])
  ])

  AS_VAR_POPDEF([A])dnl
  AS_VAR_POPDEF([B])dnl

  dnl # Execute ACTION-IF-TRUE / ACTION-IF-FALSE.
  if test "$ax_compare_version" = "true" ; then
    m4_ifvaln([$4],[$4],[:])dnl
    m4_ifvaln([$5],[else $5])dnl
  fi
])

AC_DEFUN([AX_PROG_PYTHON_VERSION],[
    AC_REQUIRE([AC_PROG_SED])
    AC_REQUIRE([AC_PROG_GREP])

    AS_IF([test -n "$PYTHON"],[
        ax_python_version="$1"

        changequote(<<,>>)
        python_version=`$PYTHON -V 2>&1 | $GREP "^Python " | $SED -e 's/^.* \([0-9]*\.[0-9]*\.[0-9]*\)/\1/'`
        changequote([,])
        AC_MSG_RESULT($python_version found)

	AC_SUBST([PYTHON_VERSION],[$python_version])

        AX_COMPARE_VERSION([$ax_python_version],[le],[$python_version],[
	    :
            $2
        ],[
	    :
            $3
        ])
    ],[
        AC_MSG_WARN([could not find the python interpreter])
        $3
    ])
])

AC_DEFUN([AX_PYTHON_MODULE],[
    if test -z $PYTHON;
    then
        if test -z "$3";
        then
            PYTHON="python3"
        else
            PYTHON="$3"
        fi
    fi
    PYTHON_NAME=`basename $PYTHON`
    AC_MSG_CHECKING($PYTHON_NAME module: $1)
    $PYTHON -c "import $1" 2>/dev/null
    if test $? -eq 0;
    then
        AC_MSG_RESULT(yes)
        eval AS_TR_CPP(HAVE_PYMOD_$1)=yes
    else
        AC_MSG_RESULT(no)
        eval AS_TR_CPP(HAVE_PYMOD_$1)=no
        #
        if test -n "$2"
        then
            AC_MSG_ERROR(failed to find required module $1)
            exit 1
        fi
    fi
])

AC_DEFUN([AX_PYTHON_MODULE_VERSION], [
    AX_PYTHON_MODULE([$1], [required], [$3])
    AC_MSG_CHECKING([for version $2 or higher of $1])
    $PYTHON -c "import sys, $1; from distutils.version import StrictVersion; sys.exit(StrictVersion($1.__version__) < StrictVersion('$2'))" 2> /dev/null
    AS_IF([test $? -eq 0], [], [
        AC_MSG_RESULT([no])
        AC_MSG_ERROR([You need at least version $2 of the $1 Python module.])
    ])
    AC_MSG_RESULT([yes])
])
