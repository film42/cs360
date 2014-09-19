#include "client_parser.h"
#include "repl.h"
#include "client.h"
#include "templates.h"

void ClientParser::read(std::vector<std::string> commands) {

  if( commands.size() != 2 ) {
    std::cout << "Error: Bad read params" << std::endl;
    return;
  }

  // Get User
  auto user = commands.front();

  // Get Index
  int index = safe_stoi( commands.at(1) );

  if( index < 1 ) {
    std::cout << "Error: Bad read params" << std::endl;
    return;
  }


  // Send Request
  auto client = Client::get_instance();

  bool status = client->send_request( Request::get( user, index ) );

  if( !status ) {
    std::cout << "Error: Request could not send to the server!";
  }

  // Read Request
  auto response = client->read_until('\n');

  if( is_error(response) ) {
    std::cout << response;
    return;
  }

  std::cout << split( response ).at(1) << std::endl;

  int byte_count = std::stoi( split( response ).back() );

  std::cout << client->read_for( byte_count ) << std::endl;

}

void ClientParser::send(std::vector<std::string> commands) {

  if( commands.size() != 2 ) {
    std::cout << "Error: Bad send params" << std::endl;
    return;
  }

  // Get User
  auto user = commands.front();

  // Get Subject
  auto subject = commands.at(1);

  // Get Message
  std::cout << "- Type your message. End with a blank line -" << std::endl;

  std::string message;

  while(true) {
    std::string buffer;
    getline(std::cin, buffer);

    if( buffer.empty() ) {
      break;
    }

    message += buffer += "\n";
  }

  // Send Request
  auto client = Client::get_instance();

  bool status = client->send_request( Request::put(user, subject, message) );

  if( !status ) {
    std::cout << "Error: Request could not send to the server!";
  }

  // Read Request
  auto response = client->read_until('\n');

  if( is_error(response) ) {
    std::cout << response;
    return;
  }

}

void ClientParser::list(std::vector<std::string> commands) {

  if( commands.size() != 1 ) {
    std::cout << "Error: Bad list params" << std::endl;
    return;
  }

  // Get User
  auto user = commands.front();

  // Get Request
  auto client = Client::get_instance();

  bool status = client->send_request( Request::list( user ) );

  if( !status ) {
    std::cout << "Error: Request could not send to the server!";
  }

  // Print Response
  auto response = client->read_until('\n');

  if( is_error(response) ) {
    std::cout << response;
    return;
  }

  int list_size = std::stoi( split( response ).back() );

  for( int i = 0; i < list_size; ++i ) {
    std::cout << client->read_until('\n');
  }

}

void ClientParser::quit() {
  REPL::release();
}