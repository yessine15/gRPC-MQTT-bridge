#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <chrono>  
#include <thread>  

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

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
ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using my_project::Mqtt_Service;
using my_project::Connect_Request;
using my_project::Connect_Reply;
using my_project::Publish_Request;
using my_project::Publish_Reply;
using my_project::Subscribe_Request;
using my_project::Subscribe_Reply;









/////////////////////////////////////////////////////////////////////////


class Mqtt_ServiceClient {
 public:
  Mqtt_ServiceClient(std::shared_ptr<Channel> channel)
      : stub_(Mqtt_Service::NewStub(channel)) {}


//////////so we have to impliment the RPC services /////////////////////////// 



////////////////////////////Connection_MQTT//////////////////////////////


  std::string Connection_MQTT(const std::string& IP_address) {

    Connect_Request request;
    request.set_address(IP_address);
    Connect_Reply reply;
    ClientContext context;

    Status status = stub_->Connection_MQTT(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }


//////////////////////////////Publish_MQTT/////////////////////////////

  std::string Publish_MQTT(const std::string& IP_address,const std::string& mqtt_topic,const std::string& msg) {
    Publish_Request request;
    
    request.set_address(IP_address);
    request.set_topic(mqtt_topic);
    request.set_message(msg);
    Publish_Reply reply;

    ClientContext context;

    Status status = stub_->Publish_MQTT(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }




/////////////////////////////Subscribe_MQTT//////////////////////////////
 std::string Subscribe_MQTT(const std::string& IP_address , const std::string& mqtt_topic) {

    Subscribe_Request request;
    request.set_address(IP_address);
    request.set_topic(mqtt_topic);
    Subscribe_Reply reply;
    ClientContext context;


    Status status = stub_->Subscribe_MQTT(&context, request, &reply);
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Mqtt_Service::Stub> stub_;
};



////////////////////////////////////////


int main(int argc, char** argv) {

  absl::ParseCommandLine(argc, argv);
  std::string target_str = absl::GetFlag(FLAGS_target);

  Mqtt_ServiceClient Mqtt_Ser(grpc::CreateChannel(target_str, grpc::InsecureChannelCredentials()));
  std::cout << "Connected to the gRPC server on the IP address : " << target_str << std::endl;

  std::chrono::milliseconds duration(2000);
  std::this_thread::sleep_for(duration);
  


  std::cout << "Select one of these options : "<< std::endl;
  std::cout << "Press ' 1 ' to Connect to the IP_address of the mqtt client "  << std::endl;
  std::cout << "Press ' 2 ' to Publish on a topic "<< std::endl;
  std::cout << "Press ' 3 ' to Subscribe for a topic "<< std::endl;


  int c;
  std::cin >> c ;
 /////////////////////////////////////////////
switch (c) {
    case 1:
       
	    {       std::cout << "Type the IP_address of the mqtt_client you want to connect to :" << std::endl;
        std::string IP_address;
        std::cin >> IP_address ;
        std::string reply = Mqtt_Ser.Connection_MQTT(IP_address);
        std::cout << reply << std::endl;
        break;
	    }	


    case 2:
	    {    
        std::cout << "Type the IP_address of the mqtt_client you want to connect to :"  << std::endl;
        std::string IP_address;
        std::cin >> IP_address ;
        std::cout << "Type the name of the topic you want to publish on : " << std::endl;
        std::string topic ;
        std::cin >> topic ;
        std::cout << "Type the message to send : " << std::endl;
        std::string msg_to_send ;
        std::cin >> msg_to_send;
        std::string reply = Mqtt_Ser.Publish_MQTT(IP_address,topic,msg_to_send);
       ///////////////////////we will wait the reply from the server /////////////////////////////////////
        std::cout << reply << std::endl;
        break;
	    }
    case 3:
	    {
        std::cout << "Type the IP_address of the mqtt_client you want to connect to :"  << std::endl;
        std::string IP_address;
        std::cin >> IP_address;
        std::cout << "Type the name of the topic you want to subscribe to : " << std::endl;
        std::string topic ;
        std::cin >> topic ;
        std::string reply = Mqtt_Ser.Subscribe_MQTT(IP_address,topic);
       ///////////////////////we will wait the reply from the server /////////////////////////////////////
        std::cout << reply << std::endl; 
        break;
	    }

   
    default:
	    {	std::cout << "Bad Option !" << std::endl;

         // Code to execute if expression doesn't match any case
        break; // Optional: Exit the switch block
	    }
}




  return 0;
}



















