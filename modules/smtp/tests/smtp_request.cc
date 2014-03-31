#undef ENABLE_TRACE
#include "../smtp.c"
#include <glib.h>

static SmtpProxy *self;

/**
 * @brief Test smtp_request_parse() with various input vectors
 */
static void
test_request_parse(void)
{
#define TEST_REQUEST_FAILS(_line) do {                                 \
    g_assert(smtp_parse_request(self, _line, strlen(_line)) == FALSE); \
  } while (0)

  TEST_REQUEST_FAILS("");
  TEST_REQUEST_FAILS(" HELO");

#define TEST_REQUEST(_line, _request, _param) do {                    \
    g_assert(smtp_parse_request(self, _line, strlen(_line)));         \
    g_assert_cmpstr(self->request->str, ==, _request);                \
    g_assert_cmpstr(self->request_param->str, ==, _param);            \
  } while (0)

  TEST_REQUEST("HELO", "HELO", "");
  TEST_REQUEST("HELO ", "HELO", "");
  TEST_REQUEST("HELO  ", "HELO", "");
  TEST_REQUEST("EHLO example.domain.com", "EHLO", "example.domain.com");
  TEST_REQUEST("EHLO  example.domain.com", "EHLO", "example.domain.com");

#undef TEST_REQUEST
#undef TEST_REQUEST_FAILS
}

int
main(int argc, char *argv[])
{
  g_test_init(&argc, &argv, NULL);

  z_thread_init();
  z_log_init("smtp_request", ZLF_STDERR);
  z_python_init();

  self = Z_NEW_COMPAT(Z_CLASS(SmtpProxy), SmtpProxy);
  g_strlcpy(self->super.session_id, "test/telnet:1", sizeof(self->super.session_id));
  self->request = g_string_new("");
  self->request_param = g_string_new("");

  g_test_add_func("/smtp/request/parse", test_request_parse);

  g_test_run();

  return 0;
}
