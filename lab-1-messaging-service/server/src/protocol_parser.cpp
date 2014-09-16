#include "protocol_parser.h"
#include "db.h"

std::string ProtocolParser::put(std::vector<std::string> arguments) {
  // Check argument length
  if( arguments.size() != 4 ) {
    return Response::error("invalid arguments");
  }

  auto name = arguments.at(0);
  auto subject = arguments.at(1);
  auto message = arguments.at(3);

  // Create message object
  auto message_dto = message_t( subject, message );

  DB::get_instance()->add_message(name, message_dto);

  return Response::ok();
}

std::string ProtocolParser::list(std::string name) {
  // Validate name
  if( name.at( name.size()-1 ) != '\n' ) {
    return Response::error("invalid arguments");
  }

  // Validate name
  if( name.find(" ") != std::string::npos ) {
    return Response::error("invalid arguments");
  }

  auto subjects = DB::get_instance()->list( name );

  return Response::list( subjects );
}

std::string ProtocolParser::get(std::vector<std::string> arguments) {
  // Check argument length
  if( arguments.size() != 2 ) {
    return Response::error("invalid arguments");
  }

  auto name = arguments.at(0);
  int index = std::stoi( arguments.at(1) );

  auto message_vec = DB::get_instance()->get(name, index);

  if(message_vec.empty() && DB::get_instance()->is_user( name ) ) {
    return Response::error("no such user");
  }

  return Response::message( message_vec.front() );
}

std::string ProtocolParser::reset() {

  DB::get_instance()->delete_all();

  return Response::ok();
}
