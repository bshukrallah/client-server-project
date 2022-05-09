#include <iostream>
#include <chrono>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

std::vector<char> vBuffer(1 * 1024);

void getData(asio::ip::tcp::socket& socket) 
{
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::cout << "\n\nRead " << length << " bytes\n\n";

				for (int i = 0; i < length; i++)
					std::cout << vBuffer[i];

				getData(socket);
			}
		}
	);
}

int main(){
	asio::error_code ec;

	//create context
	asio::io_context context;

	asio::io_context::work idlework(context);

	std::thread thrContext = std::thread([&]() { context.run(); });


	//Get the address to connect to
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	//create a socket, which delivers the implementation
	asio::ip::tcp::socket socket(context);

	socket.connect(endpoint, ec);

	if (!ec) { std::cout << "Connected!" << std::endl; }
	else { std::cout << "Failed to connect to address:\n" << ec.message() << std::endl; }

	if (socket.is_open()) 
	{
		getData(socket);

		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(2000ms);

		context.stop();
		if (thrContext.joinable()) thrContext.join();
	}

	return 0;
}