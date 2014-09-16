#include <Python/Python.h>
#include "protocol_parser.h"
#include "db.h"
#include "server.h"

ProtocolParser::ProtocolParser(int client, Server *server)
    : m_client(client), m_server(server) {}


std::string ProtocolParser::evaluate() {

  std::cout << "Entering evaluate" << std::endl;

  auto input = m_server->read_until( m_client, '\n' );

  std::cout << "Received data" << std::endl;

  if( !_is_valid(input) ) {
    return Response::error("Error! Could not parse request");
  }

  auto input_vec = utils::split( input );

  // PUT REQUEST
  if( input_vec.front() == "put" ) {

    // Remove first item
    input_vec.erase( input_vec.begin());

    return put( input_vec );
  }

  // LIST REQUEST
  if( input_vec.front() == "list" ) {
    if( input_vec.size() > 2) {
      return Response::error("too many list arguments");
    }

    return list( input_vec.back() );
  }

  // GET REQUEST
  if( input_vec.front() == "get" ) {

    // Remove first item
    input_vec.erase( input_vec.begin());

    return get( input_vec );
  }

  // RESET REQUEST
  if( input_vec.front() == "reset" ) {
    if( input_vec.size() > 1) {
      return Response::error("too many reset arguments");
    }

    return reset();
  }

  return Response::error("Error! Could not parse request");
}

//
// Request Methods
//
std::string ProtocolParser::put(std::vector<std::string> arguments) {
  // Check argument length
  if( arguments.size() != 4 ) {
    return Response::error("invalid arguments");
  }

  auto name = arguments.at(0);
  auto subject = arguments.at(1);

  auto byte_count = utils::safe_stoi( arguments.back() );

  std::cout << byte_count << std::endl;

  auto message = m_server->read_for( m_client , byte_count );

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
  int index = utils::safe_stoi( arguments.at(1) );

  auto message_vec = DB::get_instance()->get(name, index);

  if(DB::get_instance()->is_user( name ) ) {
    return Response::error("no such user");
  }
  else if(message_vec.empty()) {
    return Response::error("invalid index");
  }

  return Response::message( message_vec.front() );
}

std::string ProtocolParser::reset() {

  DB::get_instance()->delete_all();

  return Response::ok();
}
