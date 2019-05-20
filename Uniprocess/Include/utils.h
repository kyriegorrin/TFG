#include "OpenNI.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <fstream>
#include <bitset>

using namespace openni;

//Funció per marcar els pixels que estan per sobre o per sota dels boundaries especificats
//Input: mapa de profunditat en forma de vector, enters per marcar boundaries i especificacions
//Els pixels que sobrepassen, passen a tenir valor -1. Eix Z.
void cropAxisZ(uint16_t* data, int lowerBound, int upperBound, int height, int width){
	for(int i = 0; i < height*width; ++i){
		if(data[i] < lowerBound || data[i] > upperBound)
			data[i] = -1;
	}
}

//Funció per generar CSV de dades de profunditat a partir de matrius de dades.
void generateDepthCSV(const char* filename, uint16_t* dades, int height, int width){
	std::ofstream file;
	file.open(filename);

	for(int i = 0; i < height*width; ++i){
		if((i % width) == (width-1)) file << dades[i] << "\n";
		else file << dades[i] << ",";
	}
	file.close();
}

//Funció per generar CSV de dades RGB a partir de matrius de dades.
//Hi ha l'element R, G i B continus: R,G,B,R,G,B,...
void generateImageCSV(const char* filename, uint8_t* dades, int height, int width){
	std::ofstream file;
	file.open(filename);

	for(int i = 0; i < height*width*3; ++i){
		if((i % (width*3)) == (width-1)) file << unsigned(dades[i]) << "\n";
		else file << unsigned(dades[i]) << ",";
	}
	file.close();
}

//Funció per generar imatge .ppm a partir de la matriu de dades RGB.
void generateImagePPM(uint8_t* dades, int height, int width){
	std::ofstream file;
	file.open("image.ppm", std::ios::out | std::ios::binary);
	
	//Afegim capçaleres
	file << "P6 ";
	file << width << " " << height << " ";
	file << "255\n";

	for(int i = 0; i < height*width*3; ++i){
		file.write((char*)&dades[i], sizeof(char));
	}
	file.close();
}

//Funció per imprimir per pantalla les característiques d'un frame
void printFrameDetails(int height, int width, int sizeInBytes, int stride, 
		PixelFormat* pixelFormat, SensorType* sensorType){
	std::cout << "-------CARACTERÍSTIQUES DEL FRAME--------\n";
	
	if(*sensorType == SENSOR_IR) std::cout << "Tipus de sensor: Sensor IR\n";
	else if(*sensorType == SENSOR_COLOR) std::cout << "Tipus de sensor: Sensor de color\n";
	else std::cout << "Tipus de sensor: Sensor de profunditat\n";

	std::cout << "Format de pixel: ";
	if(*pixelFormat == 100) std::cout << "PIXEL_FORMAT_DEPTH_1_MM\n";
	else if(*pixelFormat == 101) std::cout << "PIXEL_FORMAT_DEPTH_100_UM\n";
	else if(*pixelFormat == 102) std::cout << "PIXEL_FORMAT_SHIFT_9_2\n";
	else if(*pixelFormat == 103) std::cout << "PIXEL_FORMAT_SHIFT_9_3\n";
	else if(*pixelFormat == 200) std::cout << "PIXEL_FORMAT_RGB888\n";
	else if(*pixelFormat == 201) std::cout << "PIXEL_FORMAT_YUV422\n";
	else if(*pixelFormat == 202) std::cout << "PIXEL_FORMAT_GRAY8\n";
	else if(*pixelFormat == 203) std::cout << "PIXEL_FORMAT_GRAY16\n";
	else if(*pixelFormat == 204) std::cout << "PIXEL_FORMAT_JPEG\n";
	else if(*pixelFormat == 205) std::cout << "PIXEL_FORMAT_YUYV\n";

	std::cout << "Altura: " << height << " pixels\n";
	std::cout << "Amplada: " << width << " pixels\n";
	std::cout << "Tamany del frame: " << sizeInBytes << " bytes\n";
	std::cout << "Tamany del stride (fila): " << stride << " bytes\n";
	std::cout << "Tamany de l'element: " << stride / width << " bytes\n";
	std::cout << "------------------------------------------\n\n";
}
