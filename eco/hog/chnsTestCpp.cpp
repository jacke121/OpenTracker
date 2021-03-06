/*******************************************************************************
* Piotr's Computer Vision Matlab Toolbox      Version 3.00
* Copyright 2014 Piotr Dollar.  [pdollar-at-gmail.com]
* Licensed under the Simplified BSD License [see external/bsd.txt]
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "rgbConvertMex.hpp"
#include "imPadMex.hpp"
#include "convConst.hpp"
#include "imResampleMex.hpp"
#include "gradientMex.hpp"

#include <iostream>
#include <cstdio>
#include <ctime>

#define debug(a, args...) printf("%s(%s:%d) " a "\n", __func__, __FILE__, __LINE__, ##args)
// compile and test standalone channels source code
int main(int argc, const char *argv[])
{
  int h, w, d, binSize, nOrients, softBin, cell_size, nDim;
  h = 150;
  w = 150;
  d = 3;
  binSize = 6;
  nOrients = 9;
  softBin = -1;
  cell_size = 6;
  nDim = 31;
  float clip = 0.2f;
  float *I, *M, *O, *H;
  I = (float *)wrCalloc(h * w * d, sizeof(float));
  M = (float *)wrCalloc(h * w * d, sizeof(float));
  O = (float *)wrCalloc(h * w * d, sizeof(float));
  H = (float *)wrCalloc(h * w * d, sizeof(float));
  //H = (float *)wrCalloc((int)std::floor(h / cell_size) * (int)std::floor(w / cell_size) * (nDim + 1), sizeof(float));
  std::clock_t start;
  double duration;
  start = std::clock();
  gradMag(I, M, O, h, w, d, 1);
  duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
  std::cout << "printf: " << duration << '\n';

  start = std::clock();
  fhog(M, O, H, h, w, binSize, nOrients, softBin, clip);
  duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
  std::cout << "printf: " << duration << '\n';

  for (int i = 0; i < h; i++)
  {
    printf("%f ", M[i]);
  }
  printf("\nM end\n");
  for (int i = 0; i < h; i++)
  {
    printf("%f ", O[i]);
  }
  printf("\nO end\n");
  for (int i = 0; i < std::floor(h / cell_size); i++)
  {
    printf("%f ", H[i]);
  }
  printf("\nH end\n");
  /*
  // initialize test array (misalign controls memory mis-alignment)
  const int h = 150, w = 150, misalign = 1;
  int x, y, d;
  float I[h * w * 3 + misalign], *I0 = I + misalign;
  for (x = 0; x < h * w * 3; x++)
    I0[x] = 0;
  for (d = 0; d < 3; d++)
    I0[int(h * w / 2 + h / 2) + d * h * w] = 1;

  // initialize memory for results with given misalignment
  const int pad = 2, rad = 2, sf = sizeof(float);
  d = 3;
  const int h1 = h + 2 * pad, w1 = w + 2 * pad, h2 = h1 / 2, w2 = w1 / 2, h3 = h2 / 4, w3 = w2 / 4;
  float *I1, *I2, *I3, *I4, *Gx, *Gy, *M, *O, *H, *G;
  I1 = (float *)wrCalloc(h1 * w1 * d + misalign, sf) + misalign;
  I3 = (float *)wrCalloc(h1 * w1 * d + misalign, sf) + misalign;
  I4 = (float *)wrCalloc(h2 * w2 * d + misalign, sf) + misalign;
  Gx = (float *)wrCalloc(h2 * w2 * d + misalign, sf) + misalign;
  Gy = (float *)wrCalloc(h2 * w2 * d + misalign, sf) + misalign;
  M = (float *)wrCalloc(h2 * w2 * d + misalign, sf) + misalign;
  O = (float *)wrCalloc(h2 * w2 * d + misalign, sf) + misalign;
  H = (float *)wrCalloc(h3 * w3 * d * 6 + misalign, sf) + misalign;
  G = (float *)wrCalloc(h3 * w3 * d * 24 + misalign, sf) + misalign;

  std::clock_t start;
  double duration;
  start = std::clock();

  // perform tests of imPad, rgbConvert, convConst, resample and gradient
  imPad(I0, I1, h, w, d, pad, pad, pad, pad, 0, 0.0f);
  I2 = rgbConvert(I1, h1 * w1, d, 0, 1.0f);
  d = 1;
  convTri(I2, I3, h1, w1, d, rad, 1);
  resample(I3, I4, h1, h2, w1, w2, d, 1.0f);
  grad2(I4, Gx, Gy, h2, w2, d);
  gradMag(I0, M, O, 150, 150, 3, 1);
  gradHist(M, O, H, h2, w2, 4, 6, 0, 1);

  duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
  std::cout << "printf: " << duration << '\n';
  start = std::clock();
  //hog(H, G, H, h2, w2, 4, 6, 1, 1, .2f);
  fhog(M, O, H, h2, w2, 6, 9, -1, .2f);

  duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
  std::cout << "printf: " << duration << '\n';

  // print some test arrays
  printf("---------------- M: ----------------\n");
  for (y = 0; y < h2; y++)
  {
    for (x = 0; x < w2; x++)
      printf("%.4f ", M[x * h2 + y]);
    printf("\n");
  }
  printf("---------------- O: ----------------\n");
  for (y = 0; y < h2; y++)
  {
    for (x = 0; x < w2; x++)
      printf("%.4f ", O[x * h2 + y]);
    printf("\n");
  }
  
  // free memory and return
  wrFree(I1 - misalign);
  wrFree(I2);
  wrFree(I3 - misalign);
  wrFree(I4 - misalign);
  wrFree(Gx - misalign);
  wrFree(Gy - misalign);
  wrFree(M - misalign);
  wrFree(O - misalign);
  wrFree(H - misalign);
  wrFree(G - misalign);
  */
  return 0;
}
