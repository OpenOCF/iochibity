CFLAGS = -O2 -g -Wall -Wextra -std=c11

CPPFLAGS = -I../src \
	-I../../resource/csdk/cbor \
	-I../../resource/csdk/cjson \
	-I../../resource/c_common \
	-I../../resource/csdk/connectivity/lib/libcoap-4.1.1 \
	-I../../resource/csdk/security \
	-I../../resource/csdk/security/ocsecurity \
	-I../../resource/csdk/connectivity/api \
	-I../../resource/csdk/stack \
	-I../../resource/csdk/stack/internal \
	-I../../resource/csdk/logger

VPATH = cbordump:../../build/darwin/x86_64/debug/csdk:../../build/darwin/x86_64/debug/security:../../build/darwin/x86_64/debug/c_common

LDFLAGS = -L../../sysroots/darwin/x86_64/debug/lib \
	-L../../build/darwin/x86_64/debug/c_common \
	-L../../build/darwin/x86_64/debug/logger \
	-L../../build/darwin/x86_64/debug/security \
	-lchibity -lconnectivity_abstraction

all: json2cbor cbordump

cbordump: cbordump.o cborparser.o cborerrorstrings.o cborpretty.o cbortojson.o open_memstream.o

json2cbor: cborencoder.o cborerrorstrings.o cborencoder_close_container_checked.o cJSON.o

# \
# 	ocrandom.o oic_time.o \
# 	srmresourcestrings.o srmutility.o doxmresource.o \
# 	../../build/linux/x86_64/debug/c_common/libc_common.a \
# 	../../build/linux/x86_64/debug/connectivity/coap/libcoap.a \
# 	../../build/linux/x86_64/debug/logger/liblogger.a \
# 	../../build/linux/x86_64/debug/security/libocsrm.a \
# 	../../build/linux/x86_64/debug/tinydtls/libtinydtls.a \
# 	../../build/linux/x86_64/debug/stack/libchibity.a



# $(CC) -L../../build/linux/x86_64/release/csdk -o $@ $^


#$(RM) $^

clean:
	rm json2cbor cbordump *.o
