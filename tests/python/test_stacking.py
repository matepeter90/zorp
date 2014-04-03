# vim: ts=8 sts=4 expandtab autoindent
from Zorp.Core import *
from Zorp.Session import MasterSession, StackedSession, ClientInfo
from Zorp.AnyPy import AnyPyProxy
from socket import *
import Zorp.Globals
import Zorp.Zone

import threading
import unittest
import collections
import time

config.options.kzorp_enabled = FALSE

class TestProxy(AnyPyProxy):

    def notifyParent(self):
        self.session.proxy_event.set()

    def log(self, *args):
        proxyLog(self, "anypy.debug", 6, *args)

    def copyClientToServer(self):
        self.log("Copying one chunk of data from client to server")
        data = self.client_stream.read(1024)
        self.server_stream.write(data)

    def copyServerToClient(self):
        self.log("Copying one chunk of data from server to client")
        data = self.server_stream.read(1024)
        self.client_stream.write(data)


def createStreamPair(name_prefix):
    streams = streamPair(AF_UNIX, SOCK_STREAM)

    pair = collections.namedtuple('StreamPair', 'downstream upstream')
    pair.downstream = streams[0]
    pair.upstream = streams[1]

    pair.downstream.name = name_prefix + "/downstream"
    pair.upstream.name = name_prefix + "/upstream"

    return pair


class ProxyTestCase(unittest.TestCase):

    def setUp(self, proxy_class):
        InetZone('internet', ('0.0.0.0/0'))
        self.service = Service("test", proxy_class)
        client_info = ClientInfo(client_stream=None, client_local=None, client_listen=None, client_address=SockAddrInet('127.0.0.1', 1234))
        self.mastersession = MasterSession(ZD_PROTO_TCP, self.service, client_info, instance_id=1)
        self.session = StackedSession(self.mastersession)

        self.session.proxy_event = threading.Event()

    def tearDown(self):
        if self.client_stream:
            self.client_stream.close()
        if self.server_stream:
            self.server_stream.close()

        del self.session
        del self.mastersession
        del self.service
        Zorp.Globals.services.clear()
        Zorp.Globals.zones.clear()
        Zorp.Zone.root_zone = Zorp.Zone.Zone("root")
        Zorp.Zone.inet_root_zone = Zorp.Zone.Zone("inet_root")

    def _setUpStreams(self):
        client_streams = createStreamPair("client")
        server_streams = createStreamPair("server")
        self.session.client_stream = client_streams.upstream
        self.session.server_stream = server_streams.upstream
        self.client_stream = client_streams.downstream
        self.server_stream = server_streams.downstream

    def waitForProxyNotify(self):
        self.session.proxy_event.wait()
        self.session.proxy_event.clear()

    def startProxy(self):
        self._setUpStreams()

        proxy = self.service.proxy_class(self.session)
        self.assertTrue(ProxyGroup(1).start(proxy))
        self.waitForProxyNotify()

        return proxy

    def doTransferClientToServer(self):
        testdata = "almafa"
        self.client_stream.write(testdata)
        output = self.server_stream.read(1024)
        self.assertEqual(output, testdata)

    def doTransferServerToClient(self):
        testdata = "almafa"
        self.server_stream.write(testdata)
        output = self.client_stream.read(1024)
        self.assertEqual(output, testdata)

    def doTransferTest(self):
        """Pump data through the proxy in both directions"""
        proxy = self.startProxy()

        self.doTransferClientToServer()
        self.doTransferServerToClient()

        self.waitForProxyNotify()

class CopyingTestProxy(TestProxy):

    def proxyThread(self):
        self.log("Copying proxy started; name='%s'", self.__class__)
        self.notifyParent()

        self.copyClientToServer()
        self.copyServerToClient()

        self.notifyParent()


class TestProxyStartup(ProxyTestCase):

    def setUp(self):
        super(TestProxyStartup, self).setUp(CopyingTestProxy)

    def test_proxy_startup(self):
        """Check that we can send an arbitrary string through the proxy"""
        self.doTransferTest()

class StackingTestProxy(TestProxy):

    def proxyThread(self):
        self.log("Proxy started; name='%s'", self.__class__)
        self.notifyParent()

        self.log("Starting stacked proxy")

        client_streams = createStreamPair("client")
        server_streams = createStreamPair("server")
        session = StackedSession(self.session)
        session.client_stream = client_streams.upstream
        session.server_stream = server_streams.upstream
        session.session_id = "svc/stacked:1"
        session.proxy_event = threading.Event()

        self.stack(Z_STACK_PROXY_IN_SESSION, CopyingTestProxy, session, client_streams.downstream, server_streams.downstream)

        self.log("Waiting for stacked proxy to start up")
        session.proxy_event.wait()
        session.proxy_event.clear()

        client_streams.downstream.nonblocking = FALSE
        server_streams.downstream.nonblocking = FALSE

        self.log("Copying from client to server through stacked proxy")
        line = self.client_stream.read(1024)
        client_streams.downstream.write(line)
        output = server_streams.downstream.read(1024)
        self.server_stream.write(output)

        self.log("Copying from server to client through stacked proxy")
        line = self.server_stream.read(1024)
        server_streams.downstream.write(line)
        output = client_streams.downstream.read(1024)
        self.client_stream.write(output)

        self.log("Waiting for stacked proxy to finish")
        session.proxy_event.wait()
        session.proxy_event.clear()

        self.log("Closing downstream streams")

        self.notifyParent()


class TestProxyStacking(ProxyTestCase):

    def setUp(self):
        super(TestProxyStacking, self).setUp(StackingTestProxy)

    def test_stack(self):
        """Stacking a child proxy and transferring data through it"""
        self.doTransferTest()

def init(name, virtual_name, is_master):
    unittest.main(argv=('/',))

# Local Variables:
# mode: python
# indent-tabs-mode: nil
# python-indent: 4
# End:
