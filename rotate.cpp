#include<iostream>
#include<fstream>
#include<string>
#include<array>
#include<vector>
#include<cmath>

#define HEADER_SIZE  54
#define WIDTH_INDEX  18
#define HEIGHT_INDEX 22
#define PI 3.141592

using namespace std;

struct pixel {
	int R;
	int G;
	int B;
	unsigned char* r;
	unsigned char* g;
	unsigned char* b;
};

int value(unsigned char c);
void swap_char(unsigned char* c1, unsigned char *c2) {
	unsigned char tmp = *c1;
	*c1 = *c2;
	*c2 = tmp;
}

void swap_pixel(struct pixel *p1, struct pixel *p2) {
	swap_char(p1->r, p2->r);
	swap_char(p1->g, p2->g);
	swap_char(p1->b, p2->b);
}

int width, height;
void readBMP(struct pixel***, unsigned char**, int, int);

void rotate(struct pixel***, int, int, int);

void rotatePoint(int x, int y, int *new_x, int *new_y, int angle){
	double rad = angle * (PI/180);
	*new_x = cos(rad) * x - sin(rad) * y;
	*new_y = sin(rad) * x + cos(rad) * y;
};

int getRotatedIdx(int i, int j, int width, int height, int *new_row, int *new_col, int angle){
	int origin_x = width/2;
	int origin_y = height/2;

	int new_x, new_y;
	rotatePoint(j-origin_x, (i-origin_y)*-1, &new_x, &new_y, angle);
	*new_row = (new_y - origin_y) * -1;
	*new_col = new_x + origin_x;

	return false;
};

void writeFile(char *filename, unsigned char *header, unsigned char* data, int size) {
	FILE* f = fopen(filename, "w+t");
	fwrite(header, sizeof(unsigned char), HEADER_SIZE, f);
	fwrite(data, sizeof(unsigned char), size, f);
	fclose(f);
};


bool **rotated;

int main(void) {
	char filename[30] = {"lena_24_crop.bmp"};
	FILE* f = fopen(filename, "rb");
	unsigned char header[HEADER_SIZE];
	fread(header, sizeof(unsigned char), HEADER_SIZE, f);

	int width = *(int*) &header[WIDTH_INDEX];
	int height = *(int* ) &header[HEIGHT_INDEX];


	int row_padded = (width*3 + 3) & (~3);
	int size = row_padded * height;

	unsigned char* data = new unsigned char[size];
	fread(data, sizeof(unsigned char), size, f);
	fclose(f);

	struct pixel** pixels = new struct pixel*[height];
	for(int i=0;i<height;i++)
		pixels[i] = new struct pixel[row_padded/3];

	rotated = new bool*[height];
	for(int i=0;i<height;i++)
		rotated[i] = new bool[row_padded/3];

	readBMP(&pixels, &data, width, height);
	rotate(&pixels, width, height, 90);

	char newName[30] =  "lena_24_crop_converted.bmp";
	writeFile(newName, header, data, size);

	for(int i=0;i<height;i++)
		free(pixels[i]);
	free(pixels);
	return 0;
}

void rotate(struct pixel*** p, int width, int height, int angle) {

	struct pixel ** pixels = *p;
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			if (rotated[i][j])
				continue;
			struct pixel* p1 = &pixels[i][j];
			int new_row;
			int new_col;
			getRotatedIdx(i, j, width, height, &new_row, &new_col, angle);

			// validation
			if (new_row > 0 && new_col > 0
					&& new_row < height && new_col < width) {
				struct pixel* p2 = &pixels[new_row][new_col];
				swap_pixel(p1, p2);
				rotated[new_row][new_col] = true;
			} else {
				*p1->r=0x00;
				*p1->g=0x00;
				*p1->b=0x00;
			}

		}
	}
}


void readBMP(struct pixel*** p, unsigned char** d, int width, int height){
	struct pixel** pixels = *p;
	struct pixel* pixel;
	unsigned char* data = *d;

	int row_padded = (width*3 + 3) & (~3);
	int idx = 0;
	for (int i=0; i < height; i++) {
		for (int j=0; j<row_padded/3; j++, idx+=3) {

			pixel = &pixels[i][j];

			pixel->r = &data[idx];
			pixel->g = &data[idx+1];
			pixel->b = &data[idx+2];

			pixel->R= value(*pixel->r);
			pixel->G= value(*pixel->g);
			pixel->B= value(*pixel->b);
		}
	}


}


int value(unsigned char c) {
	return int(c & 0xff);
}
