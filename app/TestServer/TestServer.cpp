#ifdef _MSC_VER
#pragma warning(disable:4100 4127 4267 4996)
#endif


// The ASIO_STANDALONE define is necessary to use the standalone version of Asio.
// Remove if you are using Boost Asio.
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_FUNCTIONAL_
#define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_MEMORY_

#include "InputLine.h"
#include <atomic>
#include <iostream>
#include <map>
#include <exception>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/endpoint.hpp>

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


struct connection_data 
{
	int sessionid;
	std::string name;
};

typedef std::map<connection_hdl, connection_data, std::owner_less<connection_hdl>> con_list;


class print_server 
{
public:
	print_server() : m_next_sessionid(1) 
	{
		m_server.init_asio();
		m_server.set_open_handler(bind(&print_server::on_open, this, ::_1));
		m_server.set_close_handler(bind(&print_server::on_close, this, ::_1));
		m_server.set_message_handler(bind(&print_server::on_message, this, ::_1, ::_2));
	}

	void on_open(connection_hdl hdl) 
	{
		connection_data data;
		data.sessionid = m_next_sessionid++;
		data.name = "";
		m_connections[hdl] = data;
	}

	void on_close(connection_hdl hdl) 
	{
		connection_data& data = get_data_from_hdl(hdl);
		std::cout << "Closing connection " << data.name << " with sessionid " << data.sessionid << std::endl;
		m_connections.erase(hdl);
	}

	void on_message(connection_hdl hdl, server::message_ptr msg) 
	{
		connection_data& data = get_data_from_hdl(hdl);
		printf("Received[%d] %s\r\n", data.sessionid, msg->get_payload().c_str());

//		auto ptr = m_server.get_con_from_hdl(hdl);

		for (auto &i : m_connections)
		{
			connection_hdl dest = i.first;
//			auto pdest =  m_server.get_con_from_hdl(dest);
//			if ( ptr != pdest )
			{
				m_server.send(dest, msg);
			}
		}
	}

	connection_data& get_data_from_hdl(connection_hdl hdl) 
	{
		auto it = m_connections.find(hdl);
		if (it == m_connections.end()) 
		{
			// this connection is not in the list. This really shouldn't happen
			// and probably means something else is wrong.
			throw std::invalid_argument("No data avaliable for session");
		}

		return it->second;
	}

	void run(uint16_t port) 
	{
		m_server.listen(port);
		m_server.start_accept();
		m_server.run();
	}
private:

	int m_next_sessionid;
	server m_server;
	con_list m_connections;
};

int main() 
{
	print_server server;
	server.run(9002);
}