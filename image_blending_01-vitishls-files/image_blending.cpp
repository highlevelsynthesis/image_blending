#include "image_blending.h"


void read_images(
		char *inImage1,
		char *inImage2,
		hls::stream<char> &inImage1_fifo,
		hls::stream<char> &inImage2_fifo,
		int n, int m)
{

	for (int i = 0; i < n*m; i++) {
		inImage1_fifo << inImage1[i];
		inImage2_fifo << inImage2[i];
	}
}


void blending(
		hls::stream<char> &inImage1_fifo,
		hls::stream<char> &inImage2_fifo,
		hls::stream<char> &outImage_fifo,
		float alpha,
		int n, int m)
{
	for (int i = 0; i < n*m; i++) {
		char inPixel1 = inImage1_fifo.read();
		char inPixel2 = inImage2_fifo.read();
		char outPixel = alpha*inPixel1+(1-alpha)*inPixel2;
		outImage_fifo << outPixel;
	}
}

void write_images(
		char               *outImage,
		hls::stream<char>  &outImage_fifo,
		int                 n,
		int                 m)
{
	for (int i = 0; i < n*m; i++) {
		outImage[i] = outImage_fifo.read();
	}
}

void image_blending(
		char *inImage1,
		char *inImage2,
		char *outImage,
		float alpha,
		int n,
		int m)
{
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=inImage1
#pragma HLS INTERFACE mode=m_axi bundle=gmem1 port=inImage2
#pragma HLS INTERFACE mode=m_axi bundle=gmem0 port=outImage

	hls::stream<char> inImage1_fifo;
	hls::stream<char> inImage2_fifo;
	hls::stream<char> outImage_fifo;
	read_images(inImage1, inImage2, inImage1_fifo, inImage2_fifo, n, m);
	blending(inImage1_fifo, inImage2_fifo, outImage_fifo, alpha, n, m);
	write_images(outImage, outImage_fifo, n, m);
}
