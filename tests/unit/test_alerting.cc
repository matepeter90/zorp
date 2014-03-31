#include <stdio.h>

#include <zorp/alerting.h>

#include <zorp/audit.h>
#include <zorp/zorp.h>
#include <zorp/policy.h>
#include <zorp/pyaudit.h>
#include <zorp/log.h>
#include <glib.h>

/* *******************************
 * Auxiliaries for the test cases:
*/

extern ZAuditGlobalParams audit_params;
extern AuditEventDetectorLayer audit_event_detector;

static ADPSession * const my_adp_session = (ADPSession *)0xaa55;
static int adp_session_audit_start_is_called = FALSE;
static int adp_session_audit_data_is_called = FALSE;
static guchar *data_forwarded_to_adp = NULL;
static int adp_session_destroy_is_called = FALSE;
static int adp_session_init_is_called = FALSE;
static int adp_set_event_handler_is_called = FALSE;

ADPSession *
z_alerting_session_new_mock(const EventDetectorUserData *user_data G_GNUC_UNUSED, const gchar *alerting_config G_GNUC_UNUSED)
{
  return my_adp_session;
}

static ADPSession *
adp_session_new_mock()
{
  return (ADPSession *)my_adp_session;
}

static ADPSession *
adp_session_new_error_mock()
{
  return NULL;
}

static void adp_session_init_mock(ADPSession *session G_GNUC_UNUSED, const ADPSettings *settings G_GNUC_UNUSED, AlertingErrorEvent **error G_GNUC_UNUSED)
{
  adp_session_init_is_called = TRUE;
}

static void adp_session_init_error_mock(ADPSession *session G_GNUC_UNUSED, const ADPSettings *settings G_GNUC_UNUSED, AlertingErrorEvent **error)
{
  *error = adp_alerting_error_event_new("test_error", 12345, "Test error message");
  adp_session_init_is_called = TRUE;
}

void
adp_set_event_handler_mock(ADPSession *session G_GNUC_UNUSED, ADPEventHandlerCallback event_handler_callback G_GNUC_UNUSED, void *user_data G_GNUC_UNUSED)
{
  adp_set_event_handler_is_called = TRUE;
}

ADPSession *
z_alerting_session_new_error_mock(const EventDetectorUserData *user_data G_GNUC_UNUSED, const gchar *alerting_config G_GNUC_UNUSED)
{
  return NULL;
}

static void
adp_session_audit_start_mock(ADPSession *session G_GNUC_UNUSED, const void *data G_GNUC_UNUSED, int size G_GNUC_UNUSED, unsigned long channel_id G_GNUC_UNUSED)
{
  adp_session_audit_start_is_called = TRUE;
}

static void
adp_session_audit_data_mock(ADPSession *session G_GNUC_UNUSED, const void *data, int size G_GNUC_UNUSED)
{
  data_forwarded_to_adp = (guchar*)data;
  adp_session_audit_data_is_called = TRUE;
}

static void
adp_session_destroy_mock(ADPSession *session G_GNUC_UNUSED)
{
  adp_session_destroy_is_called = TRUE;
}

/* *******************************
 * Test cases:
*/

static void
test_alerting_is_initialized(void)
{
  audit_event_detector.create = z_alerting_session_new_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL
  };

  ZAuditSession z_audit_session = {
    .trail = 0
  };

  ZATSourceType zat_source_type = ZA_SOURCE_STREAM;

  ZAuditSessionParams z_audit_session_params = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = FALSE,
    .alerting_config = NULL,
    .event_detector_user_data = g_new0(EventDetectorUserData, 1)
  };

  const gchar *name = "test_name";
  const gchar *value = "test_value";
  g_assert(!z_audit_stream_init(&z_audit_stream, &z_audit_session, zat_source_type, &z_audit_session_params, name, NULL));
  g_assert(z_audit_stream.alerting == FALSE);
  g_assert(adp_session_audit_start_is_called == FALSE);

  z_audit_session_params.alerting = TRUE;
  z_audit_stream.alerting = FALSE;
  adp_session_audit_start_is_called = FALSE;

  g_assert(z_audit_stream_init(&z_audit_stream, &z_audit_session, zat_source_type, &z_audit_session_params, name, value, NULL));
  g_assert(z_audit_stream.alerting == TRUE);
  g_assert(z_audit_stream.alerting_session == my_adp_session);
  g_assert(adp_session_audit_start_is_called == TRUE);
}

static void
test_alerting_is_off_when_no_user_data_in_session_params(void)
{
  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL
  };

  ZAuditSession z_audit_session = {
    .trail = 0
  };

  ZATSourceType zat_source_type = ZA_SOURCE_STREAM;

  ZAuditSessionParams z_audit_session_params = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .event_detector_user_data = NULL
  };

  // Audit method needs initialized session and gthread.
  g_thread_init(NULL);
  z_audit_session_init(&z_audit_session, "test");
  const gchar *name = "test_name";
  const gchar *value = "test_value";

  g_assert(!z_audit_stream_init(&z_audit_stream, &z_audit_session, zat_source_type, &z_audit_session_params, name, value, NULL));
  g_assert(z_audit_stream.alerting == FALSE);

  z_audit_session_params.audit = TRUE;

  g_assert(z_audit_stream_init(&z_audit_stream, &z_audit_session, zat_source_type, &z_audit_session_params, name, value, NULL));
  g_assert(z_audit_stream.alerting == FALSE);
}

static void
test_alerting_is_initialized_with_error(void)
{
  audit_event_detector.create = z_alerting_session_new_error_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL
  };

  ZAuditSession z_audit_session = {
    .trail = 0
  };

  ZATSourceType zat_source_type = ZA_SOURCE_STREAM;

  ZAuditSessionParams z_audit_session_params = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = FALSE
  };

  const gchar *name = "test_name";
  const gchar *value = "test_value";

  z_audit_session_params.alerting = TRUE;
  z_audit_stream.alerting = FALSE;
  adp_session_audit_start_is_called = FALSE;

  //adp mock send back an error, so it should return false
  g_assert(!z_audit_stream_init(&z_audit_stream, &z_audit_session, zat_source_type, &z_audit_session_params, name, value, NULL));
  //the alerting service should be turned off
  g_assert(z_audit_stream.alerting == FALSE);
  //the alerting session must be NULL
  g_assert(z_audit_stream.alerting_session == NULL);
}

static void
test_alerting_session_creation_with_failed_adp_construction(void)
{
  audit_event_detector.session_new = adp_session_new_error_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  adp_session_init_is_called = FALSE;

  const EventDetectorUserData *user_data = NULL;
  const gchar *alerting_config = "";

  g_assert(z_alerting_session_new(user_data, alerting_config) == NULL);
  g_assert(adp_session_init_is_called == FALSE);
}

static void
test_alerting_session_creation_with_failed_initialization(void)
{
  audit_event_detector.session_new = adp_session_new_mock;
  audit_event_detector.session_init = adp_session_init_error_mock;
  audit_event_detector.session_destroy = adp_session_destroy_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  adp_session_init_is_called = FALSE;

  const EventDetectorUserData *user_data = NULL;
  const gchar *alerting_config = "";

  g_assert(z_alerting_session_new(user_data, alerting_config) == NULL);
  g_assert(adp_session_init_is_called == TRUE);
}

static void
test_alerting_session_creation(void)
{
  audit_event_detector.session_new = adp_session_new_mock;
  audit_event_detector.session_init = adp_session_init_mock;
  audit_event_detector.set_event_handler = adp_set_event_handler_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  adp_session_init_is_called = FALSE;
  adp_set_event_handler_is_called = FALSE;

  const EventDetectorUserData *user_data = NULL;
  const gchar *alerting_config = "";

  g_assert(z_alerting_session_new(user_data, alerting_config) == my_adp_session);
  g_assert(adp_session_init_is_called == TRUE);
  g_assert(adp_set_event_handler_is_called == TRUE);
}

static void
test_alerting_data_recvd(void)
{
  audit_event_detector.process_audit_recv = adp_session_audit_data_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL,
    .session = g_new0(ZAuditSession, 1),
    .event_detector_user_data = NULL
  };

  guint8 flags = 0;
  const guchar data[] = { 0x55, 0xaa, 0x05, 0xaf, 0xa0, 0xf5 };
  adp_session_audit_data_is_called = FALSE;

  g_assert(z_audit_stream_data_recvd(&z_audit_stream, flags, data, sizeof(data)));
  g_assert(adp_session_audit_data_is_called == TRUE);
  g_assert(memcmp(data, data_forwarded_to_adp, sizeof(data)) == 0);
}

static void
test_alerting_data_sent(void)
{
  audit_event_detector.process_audit_sent = adp_session_audit_data_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL,
    .session = g_new0(ZAuditSession, 1),
    .event_detector_user_data = NULL
  };

  guint8 flags = 0;
  const guchar data[] = { 0x55, 0xaa, 0x05, 0xaf, 0xa0, 0xf5 };
  adp_session_audit_data_is_called = FALSE;

  g_assert(z_audit_stream_data_sent(&z_audit_stream, flags, data, sizeof(data)));
  g_assert(adp_session_audit_data_is_called == TRUE);
  g_assert(memcmp(data, data_forwarded_to_adp, sizeof(data)) == 0);
}

static void
test_alerting_meta(void)
{
  audit_event_detector.process_audit_meta = adp_session_audit_data_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL,
    .session = g_new0(ZAuditSession, 1),
    .event_detector_user_data = NULL
  };

  guint direction = 0;
  ZPktBuf *buf = z_pktbuf_new();
  adp_session_audit_data_is_called = FALSE;

  g_assert(z_audit_stream_commit_meta(&z_audit_stream, direction, buf));
  g_assert(adp_session_audit_data_is_called == TRUE);
}

static void
test_return_false_on_terminate_connection(void)
{
  audit_event_detector.process_audit_recv = adp_session_audit_data_mock;
  audit_event_detector.process_audit_sent = adp_session_audit_data_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = NULL,
    .session = g_new0(ZAuditSession, 1),
    .event_detector_user_data = NULL
  };
  guint direction = 0;
  const guchar data[] = {};

  z_audit_stream.session->terminate_connection = FALSE;
  g_assert(z_audit_stream_data_recvd(&z_audit_stream, direction, data, 0) == TRUE);
  g_assert(z_audit_stream_data_sent(&z_audit_stream, direction, data, 0) == TRUE);

  z_audit_stream.session->terminate_connection = TRUE;
  g_assert(z_audit_stream_data_recvd(&z_audit_stream, direction, data, 0) == FALSE);
  g_assert(z_audit_stream_data_sent(&z_audit_stream, direction, data, 0) == FALSE);
}

static void
test_alerting_is_deinitialized(void)
{
  audit_event_detector.session_destroy = adp_session_destroy_mock;
  audit_event_detector.process_audit_start = adp_session_audit_start_mock;

  ZAuditStream z_audit_stream = {
    .audit = FALSE,
    .ids = FALSE,
    .alerting = TRUE,
    .alerting_session = my_adp_session,
    .stream_id = 1
  };
  adp_session_destroy_is_called = FALSE;

  z_audit_stream_deinit(&z_audit_stream);

  g_assert(adp_session_destroy_is_called == TRUE);
  g_assert(z_audit_stream.alerting_session == NULL);
}

static void
test_alerting_configuration_with_null_json()
{
  Py_Initialize();

  //give a NULL object
  PyObject *object = NULL;
  GError *error = NULL;
  char *result = create_string_from_dict(object, &error);
  g_assert(result == NULL);
  g_assert(error != NULL);
  g_assert(strlen(error->message) > 0);
}

static void
test_alerting_configuration_with_pystring_object()
{
  Py_Initialize();
  //create a string object
  GError *error = NULL;
  PyObject *object = PyString_FromString("not a json object");
  char *result = create_string_from_dict(object, &error);
  g_assert(result == NULL);
  g_assert(error != NULL);
  g_assert(strlen(error->message) > 0);

  //create a string object 2
  error = NULL;
  object = PyString_FromString("{\"not a\": \"json object\"}");
  result = create_string_from_dict(object, &error);
  g_assert(result == NULL);
  g_assert(error != NULL);
  g_assert(strlen(error->message) > 0);
}

static void
test_alerting_configuration_correct_json_object()
{
  Py_Initialize();
  //correct json object
  GError *error = NULL;
  PyObject *json_module = PyImport_ImportModule( "json" );
  PyObject *json_object = PyObject_CallMethod( json_module, "loads", "s", "{\"json value\": \"json object\"}" );
  char *result = create_string_from_dict(json_object, &error);
  g_assert(result != NULL);
  g_assert(!strcmp(result, "{\"json value\": \"json object\"}"));
  g_assert(error == NULL);
}

static void
test_alerting_configuration_syntax_error_json_object()
{
  Py_Initialize();
  //syntax error json
  GError *error = NULL;
  PyObject *json_module = PyImport_ImportModule( "json" );
  PyObject *json_object = PyObject_CallMethod( json_module, "loads", "s", "sytax error json object" );
  char *result = create_string_from_dict(json_object, &error);
  g_assert(result == NULL);
  g_assert(error != NULL);
  g_assert(strlen(error->message) > 0);

  //syntax error json 2
  error = NULL;
  json_object = PyObject_CallMethod( json_module, "loads", "s", "{\"json value\": \"json object\"" );
  result = create_string_from_dict(json_object, &error);
  g_assert(result == NULL);
  g_assert(error != NULL);
  g_assert(strlen(error->message) > 0);

}

static void
test_alerting_configuration_empty_dict_object()
{
  Py_Initialize();
  //the empty dict object is an empty json
  GError *error = NULL;
  PyObject *dict = PyDict_New();
  char *result = create_string_from_dict(dict, &error);
  g_assert(result != NULL);
  g_assert(!strcmp(result, "{}"));
  g_assert(error == NULL);
}

static void
test_alerting_configuration_correct_dict_object()
{
  Py_Initialize();
  //the dict object is a json
  PyObject *dict = PyDict_New();
  GError *error = NULL;
  PyDict_SetItemString(dict, "teszt_key", PyString_FromString("teszt_value"));
  char *result = create_string_from_dict(dict, &error);
  g_assert(result != NULL);
  g_assert(!strcmp(result, "{\"teszt_key\": \"teszt_value\"}"));
  g_assert(error == NULL);
}

/* ***********************************************
 * Must have main function calling the test cases:
*/

int
main(int argc, char *argv[])
{
  g_test_init(&argc, &argv, NULL);
  audit_params.per_session = TRUE;

  g_test_add_func("/alerting/initialized", test_alerting_is_initialized);
  g_test_add_func("/alerting/data_recvd", test_alerting_data_recvd);
  g_test_add_func("/alerting/data_send", test_alerting_data_sent);
  g_test_add_func("/alerting/meta", test_alerting_meta);
  g_test_add_func("/alerting/return_false_on_terminate_connection", test_return_false_on_terminate_connection);
  g_test_add_func("/alerting/deinitialized", test_alerting_is_deinitialized);
  g_test_add_func("/alerting/it_is_off_when_no_user_data_in_session_params", test_alerting_is_off_when_no_user_data_in_session_params);
  g_test_add_func("/alerting/initialized_with_error", test_alerting_is_initialized_with_error);
  g_test_add_func("/alerting/session_creation_with_failed_construction", test_alerting_session_creation_with_failed_adp_construction);
  g_test_add_func("/alerting/session_creation_with_failed_initialization", test_alerting_session_creation_with_failed_initialization);
  g_test_add_func("/alerting/session_creation", test_alerting_session_creation);
  g_test_add_func("/alerting/alerting_configuration_parsing_with_null_json", test_alerting_configuration_with_null_json);
  g_test_add_func("/alerting/alerting_configuration_parsing_with_pystring_object", test_alerting_configuration_with_pystring_object);
  g_test_add_func("/alerting/alerting_configuration_parsing_correct_json_object", test_alerting_configuration_correct_json_object);
  g_test_add_func("/alerting/alerting_configuration_parsing_syntax_error_json_object", test_alerting_configuration_syntax_error_json_object);
  g_test_add_func("/alerting/alerting_configuration_parsing_empty_dict_object", test_alerting_configuration_empty_dict_object);
  g_test_add_func("/alerting/alerting_configuration_parsing_correct_dict_object", test_alerting_configuration_correct_dict_object);

  g_test_run();

  return 0;
}
