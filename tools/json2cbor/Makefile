CFLAGS = -O2 -g -Wall -Wextra
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

VPATH = cbordump:../../build/darwin/x86_64/debug/csdk
LDFLAGS = -L../../sysroots/darwin/x86_64/debug/lib \
	-L../../build/darwin/x86_64/debug/csdk \
	-L../../build/darwin/x86_64/debug/security \
	-lchibity

all: json2cbor cbordump

cbordump: cbordump.o cborparser.o cborerrorstrings.o cborpretty.o cbortojson.o open_memstream.o

json2cbor: cborencoder.o cborerrorstrings.o cborencoder_close_container_checked.o cJSON.o

# $(CC) -L../../build/darwin/x86_64/debug/csdk -o $@ $^


#$(RM) $^

clean:
	rm json2cbor cbordump *.o
