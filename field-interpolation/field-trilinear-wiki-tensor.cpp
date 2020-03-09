#include <cmath>
#include <cstdlib>
#include <iostream>
#include <vector>
using std::vector;

// https://en.wikipedia.org/wiki/Trilinear_interpolation
template <class F>
inline F trilinear(F xd, F yd, F zd,                //
                   F c000, F c001, F c010, F c011,  //
                   F c100, F c101, F c110, F c111) {
  F c00 = c000 * (1 - xd) + c100 * xd;
  F c01 = c001 * (1 - xd) + c101 * xd;
  F c10 = c010 * (1 - xd) + c110 * xd;
  F c11 = c011 * (1 - xd) + c111 * xd;
  F c0 = c00 * (1 - yd) + c10 * yd;
  F c1 = c01 * (1 - yd) + c11 * yd;
  return c0 * (1 - zd) + c1 * zd;
}

// index into a dense cube of data
inline int index(int M, int x, int y, int z) { return x + y * M + z * M * M; }

template <class F>
void interpolate(F* result, size_t N, const F* data, size_t M, F x, F y, F z) {
  F x0, y0, z0;
  F xd = modf(x, &x0);
  F yd = modf(y, &y0);
  F zd = modf(z, &z0);
  int xi = static_cast<int>(x0) % M;
  int yi = static_cast<int>(y0) % M;
  int zi = static_cast<int>(z0) % M;

  int i000 = N * index(M, xi + 0, yi + 0, zi + 0);
  int i001 = N * index(M, xi + 0, yi + 0, zi + 1);
  int i010 = N * index(M, xi + 0, yi + 1, zi + 0);
  int i011 = N * index(M, xi + 0, yi + 1, zi + 1);
  int i100 = N * index(M, xi + 1, yi + 0, zi + 0);
  int i101 = N * index(M, xi + 1, yi + 0, zi + 1);
  int i110 = N * index(M, xi + 1, yi + 1, zi + 0);
  int i111 = N * index(M, xi + 1, yi + 1, zi + 1);

  for (int i = 0; i < N; i++)
    result[i] = trilinear(xd, yd, zd, data[i + i000], data[i + i001],
                          data[i + i010], data[i + i011], data[i + i100],
                          data[i + i101], data[i + i110], data[i + i111]);
}

const int M = 100;
const int N = 5;
int main() {
  vector<float> field;
  field.resize(N * M * M * M);
  for (int i = 0; i < N * M * M * M; i++)  //
    field[i] = 1.0f * i / (N * M * M * M);
  vector<float> result;
  result.resize(N);
  interpolate(result.data(), N, field.data(), M, 0.1f, 3.3f, 9.5f);
  for (int i = 0; i < N; i++)  //
    printf("%f\n", result[i]);
}
