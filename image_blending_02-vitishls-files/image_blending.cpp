#include "image_blending.h"


void read_images(
		char16 *inImage1,
		char16 *inImage2,
		hls::stream<char16> &inImage1_fifo,
		hls::stream<char16> &inImage2_fifo,
		int n, int m)
{

	for (int i = 0; i < n*m/CHUNK_SIZE; i++) {
		inImage1_fifo << inImage1[i];
		inImage2_fifo << inImage2[i];
	}
}
void rearange_input_images(
		hls::stream<char16> &inImage1_16_fifo,
		hls::stream<char16> &inImage2_16_fifo,
		hls::stream<char>   *inImage1_fifo,
		hls::stream<char>   *inImage2_fifo,
		int n, int m)
{

	for (int i = 0; i < n*m/CHUNK_SIZE; i++) {
		char16 p1 = inImage1_16_fifo.read();
		char16 p2 = inImage2_16_fifo.read();
		for (int j = 0; j < CHUNK_SIZE; j++) {
			inImage1_fifo[j] << p1.pixels[j];
			inImage2_fifo[j] << p2.pixels[j];
		}
	}
}

void blending(
		hls::stream<char> &inImage1_fifo,
		hls::stream<char> &inImage2_fifo,
		hls::stream<char> &outImage_fifo,
		float alpha,
		int n, int m)
{
	for (int i = 0; i < n*m/CHUNK_SIZE; i++) {
		char inPixel1 = inImage1_fifo.read();
		char inPixel2 = inImage2_fifo.read();
		char outPixel = alpha*inPixel1+(1-alpha)*inPixel2;
		outImage_fifo << outPixel;
	}
}

void rearange_output_images(
		hls::stream<char>    *outImage_fifo,
		hls::stream<char16>  &outImage_16_fifo,
		int n, int m)
{
	for (int i = 0; i < n*m/CHUNK_SIZE; i++) {
		char16 p;
		for (int j = 0; j < CHUNK_SIZE; j++) {
			p.pixels[j] = outImage_fifo[j].read();
		}
		outImage_16_fifo << p;
	}
}
void write_images(
		char16               *outImage,
		hls::stream<char16>  &outImage_fifo,
		int                 n,
		int                 m)
{
	for (int i = 0; i < n*m/CHUNK_SIZE; i++) {
		char16 p = outImage_fifo.read();
		outImage[i] = p;
	}
}

void image_blending(
		char16 *inImage1,
		char16 *inImage2,
		char16 *outImage,
		float alpha,
		int n,
		int m)
{
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=inImage1 offset=slave
#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=inImage2 offset=slave
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=outImage offset=slave

#pragma HLS DATAFLOW
	hls::stream<char16> inImage1_16_fifo;
	hls::stream<char16> inImage2_16_fifo;

	hls::stream<char>   inImage1_fifo[CHUNK_SIZE];
	hls::stream<char>   inImage2_fifo[CHUNK_SIZE];
	hls::stream<char>   outImage_fifo[CHUNK_SIZE];

	hls::stream<char16> outImage_16_fifo;
	read_images(inImage1, inImage2, inImage1_16_fifo, inImage2_16_fifo, n, m);
	rearange_input_images(inImage1_16_fifo, inImage2_16_fifo, inImage1_fifo, inImage2_fifo, n, m);
	for (int i = 0; i < CHUNK_SIZE; i++) {
#pragma HLS UNROLL
		blending(inImage1_fifo[i], inImage2_fifo[i], outImage_fifo[i], alpha, n, m);
	}
	rearange_output_images(outImage_fifo, outImage_16_fifo, n, m);
	write_images(outImage, outImage_16_fifo, n, m);
}
