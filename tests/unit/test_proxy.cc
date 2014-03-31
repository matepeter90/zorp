#include <glib.h>
#include "helpers/zproxy.h"
#include <zorp/proxy.h>
#include <zorp/audit.h>

#ifdef ALERTING_ENABLED
  #include <adp/Alerting/event.h>
#endif

#ifdef ALERTING_ENABLED

struct _Event
{
  char *name;               /**< Name of the event. For the format of a name, see \ref message_name_format. */
  double timestamp;         /**< Timestamp of the event (seconds since epoch). May contain fractions of a second. */

  int data_size;            /**< Raw event data size. */
  void *data;               /**< Raw event data, format depends on name. */
};

static Event
create_event()
{
  Event e =
  {
    .name = "Name",
    .data = "DATA",
    .data_size = 4
  };
  return e;
}

static gboolean event_processing_result;
#endif

#ifdef ALERTING_ENABLED

static void
test_z_proxy_should_have_alerting_interface()
{
  gboolean (*callback_func)(ZProxy*, const Event*, guint32 channel_id);
  callback_func = NULL;
  callback_func = &z_proxy_handle_alerting_event_method;
  g_assert(callback_func);
}

static void
test_alerting_returns_false_when_policy_is_wrong()
{
  ZProxy* proxy = get_proxy_with_policy(
    "class TestProxy(object):\n"
    "  def no_such_method(self, type, event):\n"
    "    pass\n");

  Event event = create_event();
  event_processing_result = Z_FUNCS_CALL(proxy, ZProxy, handle_alerting_event, &event, 1);

  g_assert(event_processing_result == FALSE);

  leave_zproxy_test();
}

static void
test_alerting_should_return_true_if_no_terminate_required()
{
  ZProxy* proxy = get_proxy_with_policy(
    "class TestProxy(object):\n"
    "  def handleAlertingEvent(self, channel_id, type, event):\n"
    "    return True\n");

  Event event = create_event();

  event_processing_result = Z_FUNCS_CALL(proxy, ZProxy, handle_alerting_event, &event, 1);
  g_assert(event_processing_result == TRUE);

  leave_zproxy_test();
}

static void
test_alerting_should_return_false_if_terminate_required()
{
  ZProxy* proxy = get_proxy_with_policy(
    "class TestProxy(object):\n"
    "  def handleAlertingEvent(self, channel_id, type, event):\n"
    "    return False\n");

  Event event = create_event();

  event_processing_result = Z_FUNCS_CALL(proxy, ZProxy, handle_alerting_event, &event, 1);
  g_assert(event_processing_result == FALSE);

  leave_zproxy_test();
}

static void
test_alerting_called_python_layer_with_received_event()
{
  ZProxy* proxy = get_proxy_with_policy(
    "class TestProxy(object):\n"
    "  def handleAlertingEvent(self, channel_id, type, event):\n"
    "    self.type = type\n"
    "    self.event = event\n"
    "    return True\n");

  Event event = create_event();
  event.name = "Test Event";
  event.data = "TEST_DATA";
  event.data_size = 9;

  event_processing_result = Z_FUNCS_CALL(proxy, ZProxy, handle_alerting_event, &event, 1);

  g_assert_cmpstr(event.name, ==, fetch_policy_attribute_as_string("type"));
  g_assert_cmpstr(event.data, ==, fetch_policy_attribute_as_string("event"));

  leave_zproxy_test();
}

static gboolean handle_alerting_event_mock_called = FALSE;

static gboolean
handle_alerting_event_mock(ZProxy *self G_GNUC_UNUSED, const Event *event G_GNUC_UNUSED, guint32 channel_id G_GNUC_UNUSED)
{
  handle_alerting_event_mock_called = TRUE;
  return TRUE;
}

static void
test_audit_layer_should_call_the_proxy_when_event_happened()
{
  typedef struct _TestProxy {
    ZProxy *super;
  } TestProxy;

  ZProxyFuncs test_funcs = {
    {
      Z_FUNCS_COUNT (ZProxy),
      z_proxy_free_method
    },
    .handle_alerting_event = &handle_alerting_event_mock
  };

  Z_CLASS_DEF(TestProxy, ZProxy, test_funcs);

  TestProxy* proxy = Z_NEW(TestProxy);

  EventDetectorUserData user_data;
  user_data.proxy = (ZProxy*)proxy;
  user_data.channel_id = 1;

  Event event = create_event();
  handle_alerting_event_mock_called = FALSE;
  z_audit_event_detector_callback(&event, &user_data);
  g_assert(handle_alerting_event_mock_called == TRUE);
}
#endif

int
main(int argc, char *argv[])
{
  g_test_init(&argc, &argv, NULL);

  init_environment();

#ifdef ALERTING_ENABLED
  g_test_add_func("/zproxy_should_have_alerting_interface",
    test_z_proxy_should_have_alerting_interface);
  g_test_add_func("/alerting_returns_false_when_policy_is_wrong",
    test_alerting_returns_false_when_policy_is_wrong);
  g_test_add_func("/alerting_should_return_true_if_no_terminate_required",
    test_alerting_should_return_true_if_no_terminate_required);
  g_test_add_func("/alerting_should_return_false_if_terminate_required",
    test_alerting_should_return_false_if_terminate_required);
  g_test_add_func("/alerting_called_python_layer_with_received_event",
    test_alerting_called_python_layer_with_received_event);
  g_test_add_func("/test_audit_layer_should_call_the_proxy_when_event_happened",
    test_audit_layer_should_call_the_proxy_when_event_happened);
#endif

  g_test_run();
  return 0;
}
