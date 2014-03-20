#include <assert.h>

#include <zmq.h>

#include "jzmq.hpp"
#include "util.hpp"
#include "org_zeromq_ZMQ_Event.h"

static jmethodID constructor;
static jstring empty_addr;

JNIEXPORT void JNICALL
Java_org_zeromq_ZMQ_00024Event_nativeInit (JNIEnv *env, jclass cls)
{
    constructor = env->GetMethodID(cls, "<init>", "(I;I;Ljava/lang/String)V");
    assert (constructor);
    empty_addr = env->NewStringUTF("");
    assert (empty_addr);
}

JNIEXPORT jobject JNICALL
Java_org_zeromq_ZMQ_00024Event_read (JNIEnv *env, jclass cls, jlong socket, jint flags)
{
#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(4,0,0)
    zmq_event_t event;
    char addr[1025];

    zmq_msg_t event_msg;  // binary part
    zmq_msg_init (event_msg);

    zmq_msg_t addr_msg;  //  address part
    zmq_msg_init (&addr_msg);

    rc = zmq_msg_recv (&event_msg, s, 0);
    if (rc < 0) {
        raise_exception(env, zmq_errno());
        return;
    }
    assert (zmq_msg_more(&event_msg) != 0);
    rc = zmq_msg_recv (&addr_msg, s, 0);
    if (rc < 0) {
        raise_exception(env, zmq_errno());
        return;
    }
    assert (rc != -1);
    assert (zmq_msg_more(&addr_msg) == 0);
    // copy binary data to event struct
    memcpy (&event, zmq_msg_data (&event_msg), sizeof (event));
    // copy address part
    const size_t len = zmq_msg_size(&msg2) ;
    ep = memcpy(ep, zmq_msg_data(&msg2), len);
    *(ep + len) = 0 ;
#elif ZMQ_VERSION >= ZMQ_MAKE_VERSION(3,2,2)
    zmq_event_t event;

    zmq_msg_t msg;
    zmq_msg_init (&msg);
    rc = zmq_recvmsg ((void *) socket, &msg, flags);
    if (rc < 0) {
        raise_exception(env, zmq_errno());
        return;
    }
    memcpy (&event, zmq_msg_data (&msg), sizeof (event));
    switch (event.event) {
    case ZMQ_EVENT_CONNECTED:
        return env->NewObject(cls, constructor, event.event, event.data.connected.fd, empty_addr);
    case ZMQ_EVENT_CONNECT_DELAYED:
        return env->NewObject(cls, constructor, event.event, event.data.connect_delayed.err, empty_addr);
    case ZMQ_EVENT_CONNECT_RETRIED:
        return env->NewObject(cls, constructor, event.event, event.data.connect_retried.interval, empty_addr);
    case ZMQ_EVENT_LISTENING:
        return env->NewObject(cls, constructor, event.event, event.data.listening.fd, empty_addr);
    case ZMQ_EVENT_BIND_FAILED:
        return env->NewObject(cls, constructor, event.event, event.data.bind_failed.err, empty_addr);
    case ZMQ_EVENT_ACCEPTED:
        return env->NewObject(cls, constructor, event.event, event.data.accepted.fd, empty_addr);
    case ZMQ_EVENT_ACCEPT_FAILED:
        return env->NewObject(cls, constructor, event.event, event.data.accept_failed.err, empty_addr);
    case ZMQ_EVENT_CLOSED:
        return env->NewObject(cls, constructor, event.event, event.data.closed.fd, empty_addr);
    case ZMQ_EVENT_CLOSE_FAILED:
        return env->NewObject(cls, constructor, event.event, event.data.close_failed.err, empty_addr);
    case ZMQ_EVENT_DISCONNECTED:
        return env->NewObject(cls, constructor, event.event, event.data.disconnected.fd, empty_addr);
    }
#endif
    return;
}
