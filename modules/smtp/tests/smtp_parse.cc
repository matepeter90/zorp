#undef ENABLE_TRACE
#include "../smtpcmd.c"
#include <glib.h>

static void
test_smtp_parse_atom(void)
{
  gchar *end, *str;

#define OFFSET_TO_PTR(str, offset) (&str[offset])

  /* empty string is OK */
  str = "";
  g_assert(smtp_parse_atom(NULL, str, &end));
  g_assert(end == str);

  /* should stop at end of string */
  str = "valid";
  g_assert(smtp_parse_atom(NULL, str, &end));
  g_assert(end == OFFSET_TO_PTR(str, strlen(str)));

  /* special characters */
#define TEST_SPECIAL(chr) do {                  \
    str = chr;					\
    g_assert(smtp_parse_atom(NULL, str, &end));	\
    g_assert(end == str);			\
    str = "valid" chr "alsovalid";              \
    g_assert(smtp_parse_atom(NULL, str, &end)); \
    g_assert_cmpint(end - str, ==, strstr(str, chr) - str);     \
  } while (0)

  TEST_SPECIAL("(");
  TEST_SPECIAL(")");
  TEST_SPECIAL("<");
  TEST_SPECIAL(">");
  TEST_SPECIAL("@");
  TEST_SPECIAL(",");
  TEST_SPECIAL(";");
  TEST_SPECIAL(":");
  TEST_SPECIAL("\\");
  TEST_SPECIAL("\"");
  TEST_SPECIAL(".");
  TEST_SPECIAL("[");
  TEST_SPECIAL("]");
  TEST_SPECIAL(" ");

#undef TEST_SPECIAL
}

static void
test_smtp_parse_domain(void)
{
  gchar *end;

#define TEST_DOMAIN(str, expected) do {                  \
    gchar *_end;                                         \
    g_assert(smtp_parse_domain(NULL, str, &_end));       \
    g_assert_cmpint(_end - str, ==, strlen(expected));   \
  } while (0)

  /* must contain at least one valid character */
  g_assert(smtp_parse_domain(NULL, "", &end) == FALSE);
  g_assert(smtp_parse_domain(NULL, " invalid.example.com", &end) == FALSE);
  TEST_DOMAIN("a", "a");
  TEST_DOMAIN("a ", "a");

  /* parts may be separated by dots */
  TEST_DOMAIN("domain.name.example", "domain.name.example");
  TEST_DOMAIN("domain .name.example", "domain");
}

int
main(int argc, char *argv[])
{
  g_test_init(&argc, &argv, NULL);

  g_test_add_func("/smtp/parse/atom", test_smtp_parse_atom);
  g_test_add_func("/smtp/parse/domain", test_smtp_parse_domain);

  g_test_run();

  return 0;
}
