#include "zproxy.h"

static ZPolicy  *policy;
static PyObject *policy_instance;

void
init_environment()
{
  z_proxy_hash_init();
  z_registry_init();
}

ZProxy*
get_proxy_with_policy(const char* policy_source)
{
  // Reset policy instance to occurre to use the previous state if
  // any error happaned...
  policy_instance = NULL;
  policy = NULL;

  // Initialize the python interpreter
  z_thread_init();
  if (!z_python_init())
    {
      fprintf(stderr, "Python initialization failed\n");
      exit(1);
    }

  // Create the main
  policy = z_policy_new("");
  z_policy_boot(policy);
  z_policy_thread_acquire(policy->main_thread);

  // Parse and run the source with Python interpreter
  PyRun_SimpleString(policy_source);

  // Create policy instance that can be used by ZProxy
  PyObject* sys_mod_dict = PyImport_GetModuleDict();
  PyObject* main_mod = PyMapping_GetItemString(sys_mod_dict, "__main__");
  policy_instance = PyObject_CallMethod(main_mod, "TestProxy", "");

  // Create an ssh proxy with the policy
  ZProxyParams params;
  params.session_id = "ssh/ssh";
  params.pyclient = NULL;
  params.client = NULL;
  params.handler = policy_instance;
  params.parent = NULL;

  ZProxy *proxy;
  Py_BEGIN_ALLOW_THREADS;
  proxy = z_proxy_new(Z_CLASS(ZProxy), &params);
  Py_END_ALLOW_THREADS;
  z_policy_thread_ready(proxy->thread);

  z_policy_thread_release(policy->main_thread);

  return proxy;
}

void
leave_zproxy_test()
{
  z_python_destroy();
}

static PyObject*
fetch_policy_attribute_nolock(const char *attribute_name)
{
  return PyObject_GetAttrString(policy_instance, attribute_name);
}

PyObject*
fetch_policy_attribute(const char* attribute_name)
{
  z_policy_thread_acquire(policy->main_thread);
  PyObject *obj = fetch_policy_attribute_nolock(attribute_name);
  z_policy_thread_release(policy->main_thread);
  return obj;
}

char*
fetch_policy_attribute_as_string(const char* attribute_name)
{
  z_policy_thread_acquire(policy->main_thread);
  PyObject* temp_obj = fetch_policy_attribute_nolock(attribute_name);
  char *string = PyString_AsString(temp_obj);
  z_policy_thread_release(policy->main_thread);
  return string;
}
