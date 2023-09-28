
HOST_SYSTEM = $(shell uname | cut -f 1 -d_)
SYSTEM ?= $(HOST_SYSTEM)
CPPFLAGS += `pkg-config --cflags protobuf grpc `
CXXFLAGS += -std=c++17
ifeq ($(SYSTEM),vboxuser)
LDFLAGS += -L/usr/local/lib `pkg-config --libs --static protobuf grpc++`\
           -lgrpc++_reflection\
           -ldl
else
LDFLAGS += -L/recipe-sysroot/usr/bin `pkg-config --libs --static protobuf grpc++`\
           -Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed\
           -ldl
endif
PROTOC = protoc
GRPC_CPP_PLUGIN = grpc_cpp_plugin
GRPC_CPP_PLUGIN_PATH ?= `which $(GRPC_CPP_PLUGIN)`

PROTOS_PATH = ./

vpath %.proto $(PROTOS_PATH)

all: my_project_client my_project_server

my_project_client: my_project.pb.o my_project.grpc.pb.o my_project_client.o
	$(CXX) $^ $(LDFLAGS) -o $@

my_project_server: my_project.pb.o my_project.grpc.pb.o my_project_server.o
	$(CXX) $^ $(LDFLAGS) -o $@


.PRECIOUS: %.grpc.pb.cc
%.grpc.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --grpc_out=. --plugin=protoc-gen-grpc=$(GRPC_CPP_PLUGIN_PATH) $<

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	$(PROTOC) -I $(PROTOS_PATH) --cpp_out=. $<

clean:
	rm -f *.o *.pb.cc *.pb.h my_project_client my_project_server
