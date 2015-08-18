
#include "../include/network_io.h"
#include <iostream>
#include <exception>

extern "C" int sc_main( int argc, char **argv );

int sc_main( int argc, char **argv )
{
	using namespace soclib::common;

	try {
		NetworkIo ni(argv[1], argv[2], "");
		NetworkIo no(argv[3], "", argv[4]);
		while (ni.has_packet()) {
			NetworkPacket *p = ni.get_packet();
			if ( !p )
				continue;
			std::cout << "Packet: " << *p << std::endl;
			no.put_packet(p);
			delete p;
		}
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
	return 0;
}
