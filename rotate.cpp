#include<iostream>
#include<fstream>
#include<string>
#include<array>
#include<vector>

#define HEADER_SIZE  54
#define WIDTH_INDEX  18
#define HEIGHT_INDEX 22

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
int main(void) {
	char filename[30] = {"lena_24_crop.bmp"};
	FILE* f = fopen(filename, "rb");
	unsigned char header[HEADER_SIZE];
	fread(header, sizeof(unsigned char), HEADER_SIZE, f);

	int width = *(int*) &header[WIDTH_INDEX];
	int height = *(int* ) &header[HEIGHT_INDEX];

	int row_padded = (width*3 + 3) & (~3);
	int size = row_padded * height;

	cout << size << endl;

	unsigned char* data = new unsigned char[size];
	fread(data, sizeof(unsigned char), size, f);
	fclose(f);

	struct pixel** pixels = new struct pixel*[height];
	for(int i=0;i<height;i++)
		pixels[i] = new struct pixel[row_padded/3];

	struct pixel* pixel;
	int idx = 0;
	for (int i=0; i < height; i++) {
		for (int j=0; j<row_padded/3; j++, idx+=3) {

			//			swap_char(&data[idx], &data[idx+1]);

			pixel = &pixels[i][j];

			pixel->r = &data[idx];
			pixel->g = &data[idx+1];
			pixel->b = &data[idx+2];

			pixel->R= value(*pixel->r);
			pixel->G= value(*pixel->g);
			pixel->B= value(*pixel->b);
		}
	}

	for (int i=0; i<height; i++) {
		for (int j=0; j<width/2; j++) {
			struct pixel* p1 = &pixels[i][j];
			struct pixel* p2 = &pixels[i][width-j-1];
			swap_pixel(p1, p2);
		}
	}

	char new_filename[30] = {"lena_24_crop_converted.bmp"};
	FILE* new_f = fopen(new_filename, "w+t");
	fwrite(header, sizeof(unsigned char), HEADER_SIZE, f);
	fwrite(data, sizeof(unsigned char), size, f);
	fclose(f);

	for(int i=0;i<height;i++)
		free(pixels[i]);
	free(pixels);


	return 0;
}

int value(unsigned char c) {
	return int(c & 0xff);
}
