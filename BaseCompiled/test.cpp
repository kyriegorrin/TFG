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

	//Creem video stream de profunditat
	VideoStream depth;

	if(device.getSensorInfo(SENSOR_DEPTH) != NULL){
		rc = depth.create(device, SENSOR_DEPTH);
		if(rc != STATUS_OK) std::cout << "No es pot crear el stream de profunditat\n\n" << OpenNI::getExtendedError();
		else std::cout << "Stream de profunditat creat\n\n";
	}
	
	//Comencem stream
	rc = depth.start();
	if(rc != STATUS_OK) std::cout << "No es pot començar el stream\n\n" << OpenNI::getExtendedError();
	else std::cout << "Comencem stream\n\n";



	//Tanquem dispositius i fem shutdown
	std::cout << "Parant stream\n\n";
	depth.stop();
	std::cout << "Eliminant stream object\n\n";
	depth.destroy();
	std::cout << "Tancant dispositiu\n\n";
	device.close();
	std::cout << "Fent shutdown\n\n";
	OpenNI::shutdown();
	std::cout << "FINALITZAT\n\n";

	return 0;
}
