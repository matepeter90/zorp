pkglibdir=$(libdir)/zorp/tests
pkglibexecdir=$(libdir)/zorp/tests

check_PROGRAMS = get_kzorp_result
check_SCRIPTS = test_zone_handler.py

PYTHONPATH=$(top_srcdir)/pylib:$(top_srcdir)/pylib/kzorp:$(top_builddir)/pylib/Zorp
TEST_EXTENSIONS = .py
PY_LOG_COMPILER = $(PYTHON)

TESTS = $(check_SCRIPTS)

pkglibexec_PROGRAMS = get_kzorp_result

get_kzorp_result_SOURCES = get_kzorp_result.cc

EXTRA_DIST = $(check_SCRIPTS) test_kzorp_sockopt.py test_kzorp.py
