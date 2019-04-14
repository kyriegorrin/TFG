#include "OpenNI.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <fstream>

using namespace openni;

int main(){
	char buffer[320];

	for (int i = 0; i < 320; ++i) buffer[i] = '*';
	
	//......................INICIALITZAR...................//
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
	
	//......................FRAME PROCESSING...................//
	//Capturem un sol frame d'informació 3D
	VideoFrameRef frame;
	rc = depth.readFrame(&frame);

	if(rc != STATUS_OK) std::cout << "No es pot fer la lectura del frame\n\n" << OpenNI::getExtendedError();
	else std::cout << "Frame capturat\n\n";

	//Mirem característiques del frame
	int height, width, sizeInBytes, stride;
	SensorType sensorType;

	height = frame.getHeight();
	width = frame.getWidth();
	sizeInBytes = frame.getDataSize();
	stride = frame.getStrideInBytes();
	
	sensorType = frame.getSensorType();

	std::cout << "-------CARACTERÍSTIQUES DEL FRAME--------\n";
	if(sensorType == SENSOR_IR) std::cout << "Tipus de sensor: Sensor IR\n";
	else if(sensorType == SENSOR_COLOR) std::cout << "Tipus de sensor: Sensor de color\n";
	else std::cout << "Tipus de sensor: Sensor de profunditat\n";
	std::cout << "Altura: " << height << " pixels\n";
	std::cout << "Amplada: " << width << " pixels\n";
	std::cout << "Tamany del frame: " << sizeInBytes << " bytes\n";
	std::cout << "Tamany del stride (fila): " << stride << " bytes\n";
	std::cout << "Tamany de l'element: " << stride / width << " bytes\n";
	std::cout << "------------------------------------------\n\n";

	//Obtenim matriu d'elements i la guardem en un format CSV
	//Les dades són de 2 bytes, si fos un altre s'ha dutilitzar el uintX_t equivalen
	uint16_t* dades = (uint16_t*)frame.getData();
	
	std::ofstream file;
	file.open("frame.csv");

	for(int i = 0; i < height*width; ++i){
		if((i % width) == (width-1)) file << dades[i] << "\n";
		else file << dades[i] << ",";
	}
	file.close();

	//......................SHUTDOWN...................//
	//Tanquem dispositius i fem shutdown
	std::cout << "Fent release del frame\n\n";
	frame.release();
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
