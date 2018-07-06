#ifdef _MSC_VER
#pragma warning(disable:4100 4127 4267 4996 4503)
#endif

#include "InputLine.h"
#include <atomic>

// The ASIO_STANDALONE define is necessary to use the standalone version of Asio.
// Remove if you are using Boost Asio.
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#define _WEBSOCKETPP_CPP11_THREAD_
#define _WEBSOCKETPP_CPP11_FUNCTIONAL_
#define _WEBSOCKETPP_CPP11_SYSTEM_ERROR_
#define _WEBSOCKETPP_CPP11_RANDOM_DEVICE_
#define _WEBSOCKETPP_CPP11_MEMORY_

#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

// This header pulls in the WebSocket++ abstracted thread support that will
// select between boost::thread and std::thread based on how the build system
// is configured.
#include <websocketpp/common/thread.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::lib::lock_guard<websocketpp::lib::mutex> scoped_lock;


/**
* The telemetry client connects to a WebSocket server and sends a message every
* second containing an integer count. This example can be used as the basis for
* programs where a client connects and pushes data for logging, stress/load
* testing, etc.
*/
class telemetry_client 
{
public:

	telemetry_client() : m_open(false), m_done(false) {
		// set up access channels to only log interesting things
		m_client.clear_access_channels(websocketpp::log::alevel::all);
		m_client.set_access_channels(websocketpp::log::alevel::connect);
		m_client.set_access_channels(websocketpp::log::alevel::disconnect);
		m_client.set_access_channels(websocketpp::log::alevel::app);

		// Initialize the Asio transport policy
		m_client.init_asio();

		// Bind the handlers we are using
		using websocketpp::lib::placeholders::_1;
		using websocketpp::lib::placeholders::_2;
		using websocketpp::lib::bind;
		m_client.set_open_handler(bind(&telemetry_client::on_open, this, _1));
		m_client.set_close_handler(bind(&telemetry_client::on_close, this, _1));
		m_client.set_fail_handler(bind(&telemetry_client::on_fail, this, _1));

		m_client.set_message_handler(bind(&telemetry_client::on_message, this, _1, _2));


	}



	// This method will block until the connection is complete
	void run(const std::string & uri) 
	{
		// Create a new connection to the given URI
		websocketpp::lib::error_code ec;
		client::connection_ptr con = m_client.get_connection(uri, ec);
		if (ec) 
		{
			m_client.get_alog().write(websocketpp::log::alevel::app,"Get Connection Error: " + ec.message());
			return;
		}

		// Grab a handle for this connection so we can talk to it in a thread
		// safe manor after the event loop starts.
		m_hdl = con->get_handle();

		// Queue the connection. No DNS queries or network connections will be
		// made until the io_service event loop is run.
		m_client.connect(con);

		// Create a thread to run the ASIO io_service event loop
		websocketpp::lib::thread asio_thread(&client::run, &m_client);

		// Create a thread to run the telemetry loop
		websocketpp::lib::thread telemetry_thread(&telemetry_client::telemetry_loop, this);

		asio_thread.join();
		telemetry_thread.join();
	}

	// The open handler will signal that we are ready to start sending telemetry
	void on_open(websocketpp::connection_hdl) 
	{
		m_client.get_alog().write(websocketpp::log::alevel::app,"Connection opened, starting telemetry!");
		m_open = true;
	}

	// The close handler will signal that we should stop sending telemetry
	void on_close(websocketpp::connection_hdl) 
	{
		m_client.get_alog().write(websocketpp::log::alevel::app,"Connection closed, stopping telemetry!");
		m_done = true;
	}

	// The fail handler will signal that we should stop sending telemetry
	void on_fail(websocketpp::connection_hdl) 
	{
		m_client.get_alog().write(websocketpp::log::alevel::app,"Connection failed, stopping telemetry!");
		m_done = true;
	}

	void telemetry_loop() 
	{
		std::stringstream val;
		websocketpp::lib::error_code ec;

		while ( !m_done )
		{

			InputMode ret = getInputLine(mBuffer,sizeof(mBuffer), mLen);
			if (ret == InputMode::ENTER)
			{
				m_client.get_alog().write(websocketpp::log::alevel::app, val.str());
				m_client.send(m_hdl, mBuffer, websocketpp::frame::opcode::text, ec);
				mLen = 0;
				mBuffer[0] = 0;
			}
			else if (ret == InputMode::ESCAPE)
			{
				m_client.close(m_hdl, 0, "ESCAPE");
			}
			// The most likely error that we will get is that the connection is
			// not in the right state. Usually this means we tried to send a
			// message to a connection that was closed or in the process of
			// closing. While many errors here can be easily recovered from,
			// in this simple example, we'll stop the telemetry loop.
			if (ec) 
			{
				m_client.get_alog().write(websocketpp::log::alevel::app,"Send Error: " + ec.message());
				break;
			}
		}
	}

	void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg)
	{
		printf("Received %s\r\n", msg->get_payload().c_str());
	}


private:

	client						m_client;
	websocketpp::connection_hdl m_hdl;
	bool						m_open;
	std::atomic<bool>			m_done{ false };
	char						mBuffer[512];
	uint32_t					mLen{ 0 };
};

int main(int argc, char* argv[]) 
{
	telemetry_client c;

	std::string uri = "ws://localhost:9002";

	if (argc == 2) 
	{
		uri = argv[1];
	}

	c.run(uri);
}
