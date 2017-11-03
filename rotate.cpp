#include<iostream>
#include<fstream>
#include<string>
#include<array>
#include<vector>
#include<cmath>

#define HEADER_SIZE  54
#define WIDTH_INDEX  18
#define HEIGHT_INDEX 22
#define pi 3.141592653589793238462643383279

using namespace std;

struct pixel {
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

void writeFile(char *filename, unsigned char *header, unsigned char* data, int size);
void loadFile(char *filename, unsigned char **header, unsigned char **data, int *height, int *width, int *row_padded, int *size);
struct pixel **rotate(struct pixel** p, int width, int height, int angle);
struct pixel **readBMP(unsigned char *, int, int);
void writePixel(unsigned char *, int *, struct pixel);

int main(void) {
	char fname[30] = {"lena_24_crop.bmp"};
	char fname2[30] = {"lena_24_crop_converted.bmp"};
	
	unsigned char* header, *data;
	int width, height, row_padded, size;
	loadFile(fname, &header, &data, &height, &width, &row_padded, &size);

	struct pixel **pixels = readBMP(data, width, height);
	struct pixel **out = rotate(pixels, width, height, 45);

	for (int y=0, idx=0; y<height; y++) {
		for (int x=0; x<row_padded/3; x++) {
			writePixel(data, &idx, out[y][x]);
		}
	}

	writeFile(fname2, header, data, size);
	for(int i=0;i<height;i++)
		free(pixels[i]);
	free(pixels);
	return 0;
}

void writePixel(unsigned char *data, int *idx, struct pixel pixel) {
	data[(*idx)++] = pixel.r;
	data[(*idx)++] = pixel.g;
	data[(*idx)++] = pixel.b;
}

struct pixel **rotate(struct pixel** pixels, int width, int height, int angle) {
	int new_row;
	int new_col;
	
	double radian = angle*pi/180.0;
	double x0 = (double) width/2.0;
	double y0 = (double) height/2.0;

	struct pixel **out = new struct pixel*[height];
	for(int i=0;i<height;i++)
		out[i] = new struct pixel[width];

	for (int y=0; y<height; y++) {
		for (int x=0; x<width; x++) {
			new_col = ( y - y0 ) * sin(radian) + ( x - x0 ) * cos(radian) + x0;
			new_row = ( y - y0 ) * cos(radian) - ( x - x0 ) * sin(radian) + y0;
			if (new_row < 0 || new_col < 0 || new_row >= height || new_col >= width) {
				out[y][x].r = 0x00;
				out[y][x].g = 0x00;
				out[y][x].b = 0x00;
			}else {
				out[y][x] = pixels[new_row][new_col];
			}
		}
	}
	return out;
}

struct pixel **readBMP(unsigned char* d, int width, int height){
	struct pixel **pixels = new struct pixel*[height];
	for(int i=0;i<height;i++)
		pixels[i] = new struct pixel[width];
	int row_padded = (width*3 + 3) & (~3);

	int idx = 0;
	for (int i=0; i < height; i++) {
		for (int j=0; j<row_padded/3; j++, idx+=3) {
			if (j < width) {
				struct pixel* pixel = &pixels[i][j];
				pixel->r = d[idx];
				pixel->g = d[idx+1];
				pixel->b = d[idx+2];
			}
		}
	}
	return pixels;
}

void writeFile(char *filename, unsigned char *header, unsigned char* data, int size) {
	FILE* f = fopen(filename, "w+t");
	fwrite(header, sizeof(unsigned char), HEADER_SIZE, f);
	fwrite(data, sizeof(unsigned char), size, f);
	fclose(f);
};

void loadFile(char *filename, unsigned char **header, unsigned char **data, int *height, int *width, int *row_padded, int *size) {
	FILE* f = fopen(filename, "rb");
	*header = new unsigned char[HEADER_SIZE];
	fread(*header, sizeof(unsigned char), HEADER_SIZE, f);

	*width = *(int*) &(*header)[WIDTH_INDEX];
	*height = *(int* ) &(*header)[HEIGHT_INDEX];

	*row_padded = (*width*3 + 3) & (~3);
	*size = *row_padded * *height;

	*data = new unsigned char[*size];
	fread(*data, sizeof(unsigned char), *size, f);
	fclose(f);


}

