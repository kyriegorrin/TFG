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
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//Server address and port to use for the socket
#define SERVER_IP "192.168.1.3"
#define PORT 8080

//Tamany del buffer a utilitzar per al socket
#define SOCK_BUFF_SIZE 1000

using namespace openni;

//Vector temporal de sortida de lzo, ~1MB
static unsigned char __LZO_MMODEL out [1000000];

//Memòria de treball per a la compressió
#define HEAP_ALLOC(var,size) \
	lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

int main(int argc, char *argv[]){
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//-------------SETUP DELS SENSORS DE PROFUNDITAT I IMATGE-------------//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	
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
	
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//-------------SETUP DEL SOCKET PER A TRANSMISSIO DE DADES------------//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

	int socket_fd;
	struct sockaddr_in serv_addr;
	char *message = "Test message";
	char buffer[1000] = {0};
	
	//Inicialitzem socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_fd < 0){
		std::cout << "Creació de socket fallida\n";
		return -1;
	}

	//Configurem struct de serv_addr
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if(inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0){
		std::cout << "Direcció IP destí no valida o no suportada\n";
		return -2;
	}

	//Connexió amb el socket destí
	if(connect(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		std::cout << "Connexió fallida\n";
		return -3;
	}

	std::cout << "Connexió a servidor satisfactoria\n\n";

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//--------------INICI DE LOOP DE TRACTAMENT I TRANSMISSIO-------------//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//

	//Definició de variables d'ús per a tot el loop	
	VideoFrameRef frame, frameImage;
	int heightDepth, widthDepth, sizeInBytesDepth, strideDepth;
	int heightImage, widthImage, sizeInBytesImage, strideImage;
	
	lzo_uint inLength, outLength, newLength;
	uint16_t* depthData;
	uint8_t* imageData;

	int lzo_status;

	int sentBytes= 0;
	int networkDepthSize;

	//Counter temporal per a que acabi el loop
	int counter = 0;

	//Comencem loop
	while(counter != 3000){
		//......................TASK 1 - FRAME PROCESSING...................//
		//Capturem un sol frame d'informació 3D i imatge
		std::cout << "--------------------------------------\n";

		rc = depth.readFrame(&frame);
		if(rc != STATUS_OK) std::cout << "No es pot fer la lectura del frame de profunditat\n\n" << OpenNI::getExtendedError();
		else std::cout << "Frame de profunditat capturat\n\n";

		rc = image.readFrame(&frameImage);
		if(rc != STATUS_OK) std::cout << "No es pot fer la lectura del frame d'imatge\n\n" << OpenNI::getExtendedError();
		else std::cout << "Frame d'imatge capturat\n\n";

		//PROFUNDITAT
		heightDepth = frame.getHeight();
		widthDepth = frame.getWidth();
		sizeInBytesDepth = frame.getDataSize();
		strideDepth = frame.getStrideInBytes();

		//Obtenim matriu d'elements i la guardem en un format CSV
		//Les dades són de 2 bytes, si fos un altre s'ha dutilitzar el uintX_t equivalen
		depthData = (uint16_t*)frame.getData();

		//IMATGE
		heightImage = frameImage.getHeight();
		widthImage = frameImage.getWidth();
		sizeInBytesImage = frameImage.getDataSize();
		strideImage = frameImage.getStrideInBytes();

		//Obtenim matriu d'elements RGB i la guardem en CSV i PPM.
		//Cada element Són 3 bytes (un per cada canal RGB).
		imageData = (uint8_t*)frameImage.getData();

		//**************************** TASK 2 - DEPTH FRAME FILTERING *************************//
				
		//Fem cropping en l'eix Z si s'ha especificat als arguments del programa
		if(argc >= 3){
			cropAxisZ(depthData, atoi(argv[1]), atoi(argv[2]), heightDepth, widthDepth);
		}
		
		//TODO: reducció de soroll (convolució gaussiana?) i potser tunejar
		//com agafem paràmetres per la funció, molt guarro pel moment

		//******************************* TASK 3 - COMPRESSING ********************************//

		//OPCIONAL? Potser ficar opció per activar-ho o no,
		//depenent de les condicions de bandwith	
		
		//Inicialitzar minilzo
		if(lzo_init() != LZO_E_OK){
			std::cout << "Error inicialitzant minilzo\n\n";
		}
		
		std::cout << "Iniciant compressió amb LZO " << lzo_version_string() << "\n\n";

		//Comprimir amb les dades obtingudes
		inLength = sizeInBytesDepth;
		lzo_status = lzo1x_1_compress((const unsigned char*) depthData, inLength, out, &outLength, wrkmem);
		if (lzo_status == LZO_E_OK) 
			std::cout << "Compressió de " << inLength << " bytes a " << outLength << " bytes\n\n";
		else{
			std::cout << "ERROR: compressió fallida\n\n";
			return 2;
		}

		//***********************TASK 4 - SENDING AND SOCKET MANAGEMENT************************//
		
		//Enviament del tamany del frame en bytes, en network long
		networkDepthSize = htonl(sizeInBytesDepth);
		send(socket_fd, &networkDepthSize, sizeof(networkDepthSize), 0);

		//Enviament del frame NO COMPRIMIT
		while(sentBytes < sizeInBytesDepth && sentBytes != -1){
			sentBytes = send(socket_fd, (char *) depthData, sizeInBytesDepth, 0);
			std::cout << "Enviats " << sentBytes << " bytes\n";
		} 

		if(sentBytes < 0) perror("Error enviant dades: ");
		std::cout << "Frame " << counter << " enviat\n";
		sentBytes = 0;
		++counter;
	
	}
	std::cout << "--------------------------------------\n\n";


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//-----------------------SHUTDOWN DEL PROGRAMA------------------------//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
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
