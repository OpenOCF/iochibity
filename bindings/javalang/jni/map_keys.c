
#include "jni_init.h"
#include "map_keys.h"

jobject J_TRUE = NULL;
jobject J_FALSE = NULL;

/* We keep a bunch of key constants around globally for the moment. */
/* keep in sync with OpenOCF.java */
jobject KEY_ADDR;
jobject KEY_PORT;
jobject KEY_TRANSPORT;
jobject KEY_UDP;
jobject KEY_TCP;
jobject KEY_BLE;
jobject KEY_INDEX;
jobject KEY_SECURE;
jobject KEY_MCAST;
jobject KEY_DEVADDR;
jobject KEY_IPV4;
jobject KEY_IPV6;
jobject KEY_SCOPE;
jobject KEY_SCOPE_IF;
jobject KEY_SCOPE_LINK;
jobject KEY_SCOPE_REALM;
jobject KEY_SCOPE_ADMIN;
jobject KEY_SCOPE_SITE;
jobject KEY_SCOPE_ORG;
jobject KEY_SCOPE_GLOBAL;
jobject KEY_REMOTE_IDENTITY;
jobject KEY_REMOTE_RESULT;
jobject KEY_PAYLOAD_TYPE;
jobject KEY_PAYLOAD;
jobject KEY_COAP_OPTIONS;
jobject KEY_SERIAL;
jobject KEY_URI;

/* OCF standard property names */
jobject KEY_OCF_ADDR;
jobject KEY_OCF_PORT;
jobject KEY_OCF_DEVICE;		/* device */
jobject KEY_OCF_POLICY;		/* p */
jobject KEY_OCF_DI;		/* device id? */
jobject KEY_OCF_INSTANCE;	/* ins */
jobject KEY_OCF_RT;
jobject KEY_OCF_IF;
jobject KEY_OCF_NAME;		/* n */
jobject KEY_OCF_ID;
jobject KEY_OCF_TITLE;
jobject KEY_OCF_MEDIA_TYPE;
jobject KEY_OCF_ANCHOR;
jobject KEY_OCF_POLICY_BITMASK;	/* bm */
jobject KEY_OCF_SEC;		/* old security flag (OIC 1) */
jobject KEY_OCF_EP;
jobject KEY_OCF_EPS;
jobject KEY_OCF_PRIORITY;	/* pri */
jobject KEY_OCF_LINK;
jobject KEY_OCF_LINKS;
jobject KEY_OCF_HREF;
jobject KEY_OCF_LINK_RELATION;
jobject KEY_OCF_TPS;
jobject KEY_TRANSPORT_FLAGS;
jobject KEY_OCF_BURI;		/* base uri */
jobject KEY_OCF_PI;		/* platform id */
jobject KEY_TCP_PORT;
jobject KEY_DISCOVERABLE;
jobject KEY_OBSERVABLE;
jobject KEY_CONN_TYPE;
jobject KEY_TRANSPORT_ADAPTER;

void init_map_keys(JNIEnv* env)
{
    jobject j_true = (*env)->GetStaticObjectField(env, K_BOOLEAN, FID_BOOL_TRUE);
    J_TRUE = (*env)->NewGlobalRef(env, j_true);
    (*env)->DeleteLocalRef(env, j_true);

    jobject j_false = (*env)->GetStaticObjectField(env, K_BOOLEAN, FID_BOOL_FALSE);
    J_FALSE = (*env)->NewGlobalRef(env, j_false);
    (*env)->DeleteLocalRef(env, j_false);

    jobject key_addr = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 1);
    KEY_ADDR = (*env)->NewGlobalRef(env, key_addr);
    (*env)->DeleteLocalRef(env, key_addr);

    jobject key_port = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 2);
    KEY_PORT = (*env)->NewGlobalRef(env, key_port);
    (*env)->DeleteLocalRef(env, key_port);

    jobject key_transport = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 3);
    KEY_TRANSPORT =(*env)->NewGlobalRef(env, key_transport);
    (*env)->DeleteLocalRef(env, key_transport);

    jobject key_index = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 4);
    KEY_INDEX = (*env)->NewGlobalRef(env, key_index);
    (*env)->DeleteLocalRef(env, key_index);

    jobject key_secure = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 5);
    KEY_SECURE = (*env)->NewGlobalRef(env, key_secure);
    (*env)->DeleteLocalRef(env, key_secure);

    jobject key_mcast = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 6);
    KEY_MCAST = (*env)->NewGlobalRef(env, key_mcast);
    (*env)->DeleteLocalRef(env, key_mcast);

    jobject key_devaddr = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 7);
    KEY_DEVADDR = (*env)->NewGlobalRef(env, key_devaddr);
    (*env)->DeleteLocalRef(env, key_devaddr);

    jobject key_ipv4 = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 8);
    KEY_IPV4 = (*env)->NewGlobalRef(env, key_ipv4);
    (*env)->DeleteLocalRef(env, key_ipv4);

    jobject key_ipv6 = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 9);
    KEY_IPV6 = (*env)->NewGlobalRef(env, key_ipv6);
    (*env)->DeleteLocalRef(env, key_ipv6);

    jobject key_scope = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 10);
    KEY_SCOPE = (*env)->NewGlobalRef(env, key_scope);
    (*env)->DeleteLocalRef(env, key_scope);

    jobject key_scope_if = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 11);
    KEY_SCOPE_IF = (*env)->NewGlobalRef(env, key_scope_if);
    (*env)->DeleteLocalRef(env, key_scope_if);

    jobject key_scope_link = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 12);
    KEY_SCOPE_LINK = (*env)->NewGlobalRef(env, key_scope_link);
    (*env)->DeleteLocalRef(env, key_scope_link);

    jobject key_scope_realm = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 13);
    KEY_SCOPE_REALM = (*env)->NewGlobalRef(env, key_scope_realm);
    (*env)->DeleteLocalRef(env, key_scope_realm);

    jobject key_scope_admin = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 14);
    KEY_SCOPE_ADMIN = (*env)->NewGlobalRef(env, key_scope_admin);
    (*env)->DeleteLocalRef(env, key_scope_admin);

    jobject key_scope_site = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 15);
    KEY_SCOPE_SITE = (*env)->NewGlobalRef(env, key_scope_site);
    (*env)->DeleteLocalRef(env, key_scope_site);

    jobject key_scope_org = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 16);
    KEY_SCOPE_ORG = (*env)->NewGlobalRef(env, key_scope_org);
    (*env)->DeleteLocalRef(env, key_scope_org);

    jobject key_scope_global = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 17);
    KEY_SCOPE_GLOBAL = (*env)->NewGlobalRef(env, key_scope_global);
    (*env)->DeleteLocalRef(env, key_scope_global);

    jobject key_udp = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 18);
    KEY_UDP = (*env)->NewGlobalRef(env, key_udp);
    (*env)->DeleteLocalRef(env, key_udp);

    jobject key_tcp = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 19);
    KEY_TCP = (*env)->NewGlobalRef(env, key_tcp);
    (*env)->DeleteLocalRef(env, key_tcp);

    jobject key_identity = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 20);
    KEY_REMOTE_IDENTITY = (*env)->NewGlobalRef(env, key_identity);
    (*env)->DeleteLocalRef(env, key_identity);

    jobject key_result = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 21);
    KEY_REMOTE_RESULT = (*env)->NewGlobalRef(env, key_result);
    (*env)->DeleteLocalRef(env, key_result);

    jobject key_payload_type = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 22);
    KEY_PAYLOAD_TYPE = (*env)->NewGlobalRef(env, key_payload_type);
    (*env)->DeleteLocalRef(env, key_payload_type);

    jobject key_payload = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 23);
    KEY_PAYLOAD = (*env)->NewGlobalRef(env, key_payload);
    (*env)->DeleteLocalRef(env, key_payload);

    jobject key_coap_options = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 24);
    KEY_COAP_OPTIONS = (*env)->NewGlobalRef(env, key_coap_options);
    (*env)->DeleteLocalRef(env, key_coap_options);

    jobject key_serial = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 25);
    KEY_SERIAL = (*env)->NewGlobalRef(env, key_serial);
    (*env)->DeleteLocalRef(env, key_serial);

    jobject key_uri = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 26);
    KEY_URI = (*env)->NewGlobalRef(env, key_uri);
    (*env)->DeleteLocalRef(env, key_uri);

    jobject key_ocf_addr = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 27);
    KEY_OCF_ADDR = (*env)->NewGlobalRef(env, key_ocf_addr);
    (*env)->DeleteLocalRef(env, key_ocf_addr);

    jobject key_ocf_port = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 28);
    KEY_OCF_PORT = (*env)->NewGlobalRef(env, key_ocf_port);
    (*env)->DeleteLocalRef(env, key_ocf_port);

    jobject key_ocf_d = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 29);
    KEY_OCF_DEVICE = (*env)->NewGlobalRef(env, key_ocf_d);
    (*env)->DeleteLocalRef(env, key_ocf_d);

    jobject key_ocf_policy = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 30);
    KEY_OCF_POLICY = (*env)->NewGlobalRef(env, key_ocf_policy);
    (*env)->DeleteLocalRef(env, key_ocf_policy);

    jobject key_ocf_di = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 31);
    KEY_OCF_DI = (*env)->NewGlobalRef(env, key_ocf_di);
    (*env)->DeleteLocalRef(env, key_ocf_di);

    jobject key_ocf_instance = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 32);
    KEY_OCF_INSTANCE = (*env)->NewGlobalRef(env, key_ocf_instance);
    (*env)->DeleteLocalRef(env, key_ocf_instance);

    jobject key_ocf_rt = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 33);
    KEY_OCF_RT = (*env)->NewGlobalRef(env, key_ocf_rt);
    (*env)->DeleteLocalRef(env, key_ocf_rt);

    jobject key_ocf_if = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 34);
    KEY_OCF_IF = (*env)->NewGlobalRef(env, key_ocf_if);
    (*env)->DeleteLocalRef(env, key_ocf_if);

    jobject key_ocf_name = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 35);
    KEY_OCF_NAME = (*env)->NewGlobalRef(env, key_ocf_name);
    (*env)->DeleteLocalRef(env, key_ocf_name);

    jobject key_ocf_id = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 36);
    KEY_OCF_ID = (*env)->NewGlobalRef(env, key_ocf_id);
    (*env)->DeleteLocalRef(env, key_ocf_id);

    jobject key_ocf_title = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 37);
    KEY_OCF_TITLE = (*env)->NewGlobalRef(env, key_ocf_title);
    (*env)->DeleteLocalRef(env, key_ocf_title);

    jobject key_ocf_media_type = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 38);
    KEY_OCF_MEDIA_TYPE = (*env)->NewGlobalRef(env, key_ocf_media_type);
    (*env)->DeleteLocalRef(env, key_ocf_media_type);

    jobject key_ocf_anchor = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 39);
    KEY_OCF_ANCHOR = (*env)->NewGlobalRef(env, key_ocf_anchor);
    (*env)->DeleteLocalRef(env, key_ocf_anchor);

    jobject key_ocf_policy_bitmask = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 40);
    KEY_OCF_POLICY_BITMASK = (*env)->NewGlobalRef(env, key_ocf_policy_bitmask);
    (*env)->DeleteLocalRef(env, key_ocf_policy_bitmask);

    jobject key_ocf_sec = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 41);
    KEY_OCF_SEC = (*env)->NewGlobalRef(env, key_ocf_sec);
    (*env)->DeleteLocalRef(env, key_ocf_sec);

    jobject key_ocf_ep = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 42);
    KEY_OCF_EP = (*env)->NewGlobalRef(env, key_ocf_ep);
    (*env)->DeleteLocalRef(env, key_ocf_ep);

    jobject key_ocf_eps = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 43);
    KEY_OCF_EPS = (*env)->NewGlobalRef(env, key_ocf_eps);
    (*env)->DeleteLocalRef(env, key_ocf_eps);

    jobject key_ocf_priority = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 44);
    KEY_OCF_PRIORITY = (*env)->NewGlobalRef(env, key_ocf_priority);
    (*env)->DeleteLocalRef(env, key_ocf_priority);

    jobject key_ocf_link = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 45);
    KEY_OCF_LINK = (*env)->NewGlobalRef(env, key_ocf_link);
    (*env)->DeleteLocalRef(env, key_ocf_link);

    jobject key_ocf_links = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 46);
    KEY_OCF_LINKS = (*env)->NewGlobalRef(env, key_ocf_links);
    (*env)->DeleteLocalRef(env, key_ocf_links);

    jobject key_ocf_href = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 47);
    KEY_OCF_HREF = (*env)->NewGlobalRef(env, key_ocf_href);
    (*env)->DeleteLocalRef(env, key_ocf_href);

    jobject key_ocf_link_relation = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 48);
    KEY_OCF_LINK_RELATION = (*env)->NewGlobalRef(env, key_ocf_link_relation);
    (*env)->DeleteLocalRef(env, key_ocf_link_relation);

    jobject key_ocf_tps = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 49);
    KEY_OCF_TPS = (*env)->NewGlobalRef(env, key_ocf_tps);
    (*env)->DeleteLocalRef(env, key_ocf_tps);

    jobject key_transport_flags = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF,
								     50);
    KEY_TRANSPORT_FLAGS = (*env)->NewGlobalRef(env, key_transport_flags);
    (*env)->DeleteLocalRef(env, key_transport_flags);

    jobject key_ocf_buri = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 51);
    KEY_OCF_BURI = (*env)->NewGlobalRef(env, key_ocf_buri);
    (*env)->DeleteLocalRef(env, key_ocf_buri);

    jobject key_ocf_pi = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 52);
    KEY_OCF_PI = (*env)->NewGlobalRef(env, key_ocf_pi);
    (*env)->DeleteLocalRef(env, key_ocf_pi);

    jobject key_tcp_port = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 53);
    KEY_TCP_PORT = (*env)->NewGlobalRef(env, key_tcp_port);
    (*env)->DeleteLocalRef(env, key_tcp_port);

    jobject key_discoverable = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 54);
    KEY_DISCOVERABLE = (*env)->NewGlobalRef(env, key_discoverable);
    (*env)->DeleteLocalRef(env, key_discoverable);

    jobject key_observable = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 55);
    KEY_OBSERVABLE = (*env)->NewGlobalRef(env, key_observable);
    (*env)->DeleteLocalRef(env, key_observable);

    jobject key_conn_type = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 56);
    KEY_CONN_TYPE = (*env)->NewGlobalRef(env, key_conn_type);
    (*env)->DeleteLocalRef(env, key_conn_type);

    jobject key_transport_adapter = (*env)->CallStaticObjectMethod(env, K_INTEGER, MID_INT_VALUE_OF, 57);
    KEY_TRANSPORT_ADAPTER = (*env)->NewGlobalRef(env, key_transport_adapter);
    (*env)->DeleteLocalRef(env, key_transport_adapter);

}
