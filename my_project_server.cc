#include <iostream>
#include <memory>
#include <string>
#include <stdio.h>
#include <mosquitto.h>


#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "mqtt_protocol.h"


#ifdef BAZEL_BUILD
#include "examples/protos/my_project.grpc.pb.h"
#else
#include "my_project.grpc.pb.h"
#endif
#include "my_project.grpc.pb.h"
#include "my_project.pb.h"
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using my_project::Mqtt_Service;
using my_project::Connect_Request;
using my_project::Connect_Reply;
using my_project::Publish_Request;
using my_project::Publish_Reply;
using my_project::Subscribe_Request;
using my_project::Subscribe_Reply;





ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");
void on_message(struct mosquitto*, void*, const struct mosquitto_message*);



// Logic and data behind the server's behavior.
class Mqtt_ServiceServiceImpl final : public Mqtt_Service::Service {




///////////////////////////////////////////////////


  Status Connection_MQTT(ServerContext* context, const Connect_Request* request,
                  Connect_Reply* reply) override {

    int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("publisher-test", true, NULL);


    std::string IP_address(request->address());
    size_t colonPos = IP_address.find(':');
    std::string IPadd = IP_address.substr(0, colonPos);



	rc = mosquitto_connect(mosq,IPadd.c_str(), 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
	
	}

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();

    reply->set_message("You can connect to the broker! (ONLINE) \n");
    return Status::OK;
  }

///////////////////////////////////////////////////


  Status Publish_MQTT(ServerContext* context, const Publish_Request* request,
                  Publish_Reply* reply) override {
    

    int rc;
	struct mosquitto * mosq;

	mosquitto_lib_init();

	mosq = mosquitto_new("publisher-test", true, NULL);


    std::string IP_address(request->address());
    size_t colonPos = IP_address.find(':');
    std::string IPadd = IP_address.substr(0, colonPos);



	rc = mosquitto_connect(mosq,IPadd.c_str(), 1883, 60);
	if(rc != 0){
		printf("Client could not connect to broker! Error Code: %d\n", rc);
		mosquitto_destroy(mosq);
	}
   
   	
	printf("We are now connected to the broker!\n");

	mosquitto_publish(mosq, NULL, request->topic().c_str(), 6, request->message().c_str() , 0, false);

    mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);

	mosquitto_lib_cleanup();
   


    reply->set_message("Message published Succesfully !! \n");
    return Status::OK;
  }


///////////////////////////////////////////////////


  Status Subscribe_MQTT(ServerContext* context, const Subscribe_Request* request,
                  Subscribe_Reply* reply) override {

	int rc;

	mosquitto_lib_init();

	struct mosquitto *mosq;

	mosq = mosquitto_new("subscribe-test", true,NULL);
    
	std::string IP_address(request->address());
    size_t colonPos = IP_address.find(':');
    std::string IPadd = IP_address.substr(0, colonPos);
	rc = mosquitto_connect(mosq,IPadd.c_str(), 1883, 60);

	if(rc) {
		printf("Could not connect to Broker with return code %d\n", rc);
		
	}
    mosquitto_subscribe(mosq, NULL,request->topic().c_str(), 0);

    mosquitto_message_callback_set(mosq, on_message);
	mosquitto_loop_start(mosq);



	printf("Press Enter to quit...\n");
	getchar();
	mosquitto_loop_stop(mosq, true);

	mosquitto_disconnect(mosq);
	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
        reply->set_message("Subscribed Succesfully !! \n");



    return Status::OK;
  }




};


void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
	printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
}








void RunServer(uint16_t port) {

  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  Mqtt_ServiceServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}

