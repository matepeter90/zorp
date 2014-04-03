LIBS     = @MODULES_LIBS@
LDADD = ../libtelnet.la -lboost_unit_test_framework
CPPFLAGS = @MODULES_CPPFLAGS@ -DBOOST_TEST_DYN_LINK=1

check_PROGRAMS = test_telnet
check_SCRIPTS =

TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)

EXTRA_DIST = ${check_SCRIPTS}
TESTS = $(check_SCRIPTS) $(check_PROGRAMS)

test_telnet_SOURCES = test_telnet.cc
