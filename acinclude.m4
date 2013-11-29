AC_DEFUN([COMPILER_FLAGS], [
	if (test "${CFLAGS}" = ""); then
		CFLAGS="-Wall"
	fi
	if (test "$USE_MAINTAINER_MODE" = "yes"); then
		CFLAGS+=" -Werror -Wextra"
		CFLAGS+=" -Wno-unused-parameter"
		CFLAGS+=" -Wno-missing-field-initializers"
		CFLAGS+=" -Wdeclaration-after-statement"
		CFLAGS+=" -Wmissing-declarations"
		CFLAGS+=" -Wredundant-decls"
		CFLAGS+=" -Wcast-align"
	fi
])

AC_DEFUN([AC_INIT_BLESTACK], [
])

AC_DEFUN([AC_ARG_BLESTACK], [
	debug_enable=no
	optimization_enable=yes

	AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug], [enable compiling with debugging information]), [
		debug_enable=${enableval}
	])

	AC_ARG_ENABLE(optimization, AC_HELP_STRING([--disable-optimization], [disable code optimization]), [
		optimization_enable=${enableval}
	])

	AC_ARG_WITH([sdkdir], AC_HELP_STRING([--with-sdkdir=DIR],
			[path to NRF SDK directory]), [path_sdkdir=${withval}])

	if (test -z "${path_sdkdir}"); then
		AC_MSG_ERROR([NRF SDK directory is required])
	fi

	AC_SUBST(SDK_DIR, [${path_sdkdir}])

	AC_ARG_WITH([builddir], AC_HELP_STRING([--with-builddir=DIR],
			[path to build directory]), [path_builddir=${withval}])

	if (test "${path_builddir}"); then
		AC_SUBST(BUILD_DIR, [${path_builddir}])
	else
		AC_SUBST(BUILD_DIR, "build")
	fi

	AC_ARG_WITH([platform], AC_HELP_STRING([--with-platform=nrf|ubertooth],
			[Select target platform]), [platform_val=${withval}])

	if (test "${platform_val}" == "nrf"); then
		AM_CONDITIONAL(PLATFORM_NRF, true)
		AM_CONDITIONAL(PLATFORM_UBERTOOTH, false)
	elif (test "${platform_val}" == "ubertooth"); then
		AM_CONDITIONAL(PLATFORM_NRF, false)
		AM_CONDITIONAL(PLATFORM_UBERTOOTH, true)
	else
		AC_MSG_ERROR([Platform is required])
	fi

	if (test "${debug_enable}" = "yes" && test "${ac_cv_prog_cc_g}" = "yes"); then
		CFLAGS="-g $CFLAGS"
	fi

	if (test "${optimization_enable}" = "no"); then
		CFLAGS="-O0 $CFLAGS"
	else
		CFLAGS="-O2 $CFLAGS"
	fi
])
