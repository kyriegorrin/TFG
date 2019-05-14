#include "OpenNI.h"
#include "utils.h"
#include "minilzo.h"
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <fstream>
#include <bitset>
#include <unistd.h>

using namespace openni;

int main(int argc, char *argv[]){
//************************ TASK 1 - DATA RETRIEVAL**************************************//
	
	//..............OPENNI INITIALIZATION...................//
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
	int heightDepth, widthDepth, sizeInBytesDepth, strideDepth;
	int heightImage, widthImage, sizeInBytesImage, strideImage;

	//PROFUNDITAT
	heightDepth = frame.getHeight();
	widthDepth = frame.getWidth();
	sizeInBytesDepth = frame.getDataSize();
	strideDepth = frame.getStrideInBytes();

	//Obtenim matriu d'elements i la guardem en un format CSV
	//Les dades són de 2 bytes, si fos un altre s'ha dutilitzar el uintX_t equivalen
	uint16_t* depthData = (uint16_t*)frame.getData();

	//IMATGE
	heightImage = frameImage.getHeight();
	widthImage = frameImage.getWidth();
	sizeInBytesImage = frameImage.getDataSize();
	strideImage = frameImage.getStrideInBytes();

	//Obtenim matriu d'elements RGB i la guardem en CSV i PPM.
	//Cada element Són 3 bytes (un per cada canal RGB).
	uint8_t* imageData = (uint8_t*)frameImage.getData();

//**************************** TASK 2 - DEPTH FRAME FILTERING *************************//
			
	//Fem cropping en l'eix Z si s'ha especificat als arguments del programa
	if(argc >= 3){
		cropAxisZ(depthData, atoi(argv[1]), atoi(argv[2]), heightDepth, widthDepth);
	}
	
	//TODO: reducció de soroll (convolució gaussiana?)

//******************************* TASK 3 - COMPRESSING ********************************//

	//OPCIONAL? Potser ficar opció per activar-ho o no,
	//depenent de les condicions de bandwith	

	
//***********************TASK 4 - SENDING AND SOCKET MANAGEMENT************************//

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
