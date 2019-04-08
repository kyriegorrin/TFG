#include "OpenNI.h"
#include <iostream>

using namespace openni;

int main(){
	char buffer[320];

	for (int i = 0; i < 320; ++i) buffer[i] = '*';
	
	//Inicialitzem dispositiu
	Status rc = OpenNI::initialize();
	if(rc != STATUS_OK) std::cout << "Error d'inicialitzacio\n\n" << OpenNI::getExtendedError();
	else std::cout << "Inicialitzat correctament\n\n";

	//Obrim dispositiu
	Device device;
	rc = device.open(ANY_DEVICE);
	if(rc != STATUS_OK) std::cout << "Error obrint dispositiu\n\n" << OpenNI::getExtendedError();
	else std::cout << "Device obert correctament\n\n";

	//Tanquem dispositius i fem shutdown
	device.close();
	OpenNI::shutdown();
	return 0;
}
