#include "OpenNI.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <fstream>

using namespace openni;

//Funció per generar CSV de dades de profunditat a partir de matrius de dades.
void generateDepthCSV(uint16_t* dades, int height, int width){
	std::ofstream file;
	file.open("frameDepth.csv");

	for(int i = 0; i < height*width; ++i){
		if((i % width) == (width-1)) file << dades[i] << "\n";
		else file << dades[i] << ",";
	}
	file.close();
}

//Funció per generar CSV de dades RGB a partir de matrius de dades.
void generateImageCSV(char* dades, int height, int width){
	std::ofstream file;
	file.open("frameImage.csv");

	for(int i = 0; i < height*width; ++i){
		if((i % width) == (width-1)) file << dades[i] << "\n";
		else file << dades[i] << ",";
	}
	file.close();
}

int main(){
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

	//Creem video stream d'imatge
	VideoStream image;

	if(device.getSensorInfo(SENSOR_COLOR) != NULL){
		rc = image.create(device, SENSOR_COLOR);
		if(rc != STATUS_OK) std::cout << "No es pot crear el stream d'imatge\n\n" << OpenNI::getExtendedError();
		else std::cout << "Stream d'imatge creat\n\n";
	}
	
	//Comencem streams
	rc = depth.start();
	if(rc != STATUS_OK) std::cout << "No es pot començar el stream de profunditat\n\n" << OpenNI::getExtendedError();
	else std::cout << "Comencem stream de profunditat\n\n";

	rc = image.start();
	if(rc != STATUS_OK) std::cout << "No es pot començar el stream d'imatge\n\n" << OpenNI::getExtendedError();
	else std::cout << "Comencem stream d'imatge\n\n";
	
	//......................FRAME PROCESSING...................//
	//Capturem un sol frame d'informació 3D i imatge
	VideoFrameRef frame, frameImage;

	rc = depth.readFrame(&frame);
	if(rc != STATUS_OK) std::cout << "No es pot fer la lectura del frame de profunditat\n\n" << OpenNI::getExtendedError();
	else std::cout << "Frame de profunditat capturat\n\n";

	rc = image.readFrame(&frameImage);
	if(rc != STATUS_OK) std::cout << "No es pot fer la lectura del frame d'imatge\n\n" << OpenNI::getExtendedError();
	else std::cout << "Frame d'imatge capturat\n\n";

	//Mirem característiques dels frames i càmera
	int height, width, sizeInBytes, stride;
	SensorType sensorType;

	height = frame.getHeight();
	width = frame.getWidth();
	sizeInBytes = frame.getDataSize();
	stride = frame.getStrideInBytes();
	sensorType = frame.getSensorType();

	//PROFUNDITAT
	std::cout << "-------CARACTERÍSTIQUES DEL FRAME DE PROFUNDITAT--------\n";
	if(sensorType == SENSOR_IR) std::cout << "Tipus de sensor: Sensor IR\n";
	else if(sensorType == SENSOR_COLOR) std::cout << "Tipus de sensor: Sensor de color\n";
	else std::cout << "Tipus de sensor: Sensor de profunditat\n";
	std::cout << "Altura: " << height << " pixels\n";
	std::cout << "Amplada: " << width << " pixels\n";
	std::cout << "Tamany del frame: " << sizeInBytes << " bytes\n";
	std::cout << "Tamany del stride (fila): " << stride << " bytes\n";
	std::cout << "Tamany de l'element: " << stride / width << " bytes\n";
	std::cout << "--------------------------------------------------------\n\n";

	//Obtenim matriu d'elements i la guardem en un format CSV
	//Les dades són de 2 bytes, si fos un altre s'ha dutilitzar el uintX_t equivalen
	uint16_t* dades = (uint16_t*)frame.getData();
	generateDepthCSV(dades, height, width);

	//IMATGE
	height = frameImage.getHeight();
	width = frameImage.getWidth();
	sizeInBytes = frameImage.getDataSize();
	stride = frameImage.getStrideInBytes();
	sensorType = frameImage.getSensorType();

	std::cout << "-------CARACTERÍSTIQUES DEL FRAME D'IMATGE--------------\n";
	if(sensorType == SENSOR_IR) std::cout << "Tipus de sensor: Sensor IR\n";
	else if(sensorType == SENSOR_COLOR) std::cout << "Tipus de sensor: Sensor de color\n";
	else std::cout << "Tipus de sensor: Sensor de profunditat\n";
	std::cout << "Altura: " << height << " pixels\n";
	std::cout << "Amplada: " << width << " pixels\n";
	std::cout << "Tamany del frame: " << sizeInBytes << " bytes\n";
	std::cout << "Tamany del stride (fila): " << stride << " bytes\n";
	std::cout << "Tamany de l'element: " << stride / width << " bytes\n";
	std::cout << "--------------------------------------------------------\n\n";


	//......................SHUTDOWN...................//
	//Tanquem dispositius i fem shutdown
	std::cout << "Fent release del frame de profunditat\n\n";
	frame.release();
	std::cout << "Fent release del frame d'imatge\n\n";
	frameImage.release();
	std::cout << "Parant stream de profunditat\n\n";
	depth.stop();
	std::cout << "Parant stream d'imatge\n\n";
	image.stop();
	std::cout << "Eliminant stream object de profunditat\n\n";
	depth.destroy();
	std::cout << "Eliminant stream object d'imatge\n\n";
	image.destroy();
	std::cout << "Tancant dispositiu\n\n";
	device.close();
	std::cout << "Fent shutdown\n\n";
	OpenNI::shutdown();
	std::cout << "FINALITZAT\n\n";

	return 0;
}
