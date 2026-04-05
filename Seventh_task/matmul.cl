__kernel void matMul(
    const int M, const int K, const int N,
    __global const float* firstSrc,
    __global const float* secondSrc,
    __global float* result)
{
    int col = get_global_id(0);
    int row = get_global_id(1);

    if (row < M && col < N) {
        float sum = 0.0f;
        for (int k = 0; k < K; ++k) {
            sum += firstSrc[row * K + k] * secondSrc[k * N + col];
        }

        result[row * N + col] = sum;
    }
}