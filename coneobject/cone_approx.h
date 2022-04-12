#ifndef KG3_CONE_APPROX_H
#define KG3_CONE_APPROX_H

#define uint unsigned int
#define PI 3.14159265358979323846f

#include <gl/glew.h>

template<uint N = 64>
class cone_approx
{
    inline static float side_vertices[6 * (N + 1)];
    inline static float bottom_vertices[6 * N];
    inline static uint side_indices[3 * N];

    inline static uint side_vbo = 0;
    inline static uint bottom_vbo = 0;
    inline static uint side_ebo = 0;

    uint side_vao, bottom_vao;

public:
    static void generate_vertices();

    cone_approx();

    ~cone_approx();

    void draw() const;
};

template<uint N>
cone_approx<N>::cone_approx()
{
    glGenVertexArrays(1, &side_vao);

    glBindVertexArray(side_vao);
    glBindBuffer(GL_ARRAY_BUFFER, side_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, side_ebo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glGenVertexArrays(1, &bottom_vao);

    glBindVertexArray(bottom_vao);
    glBindBuffer(GL_ARRAY_BUFFER, bottom_vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

template<uint N>
cone_approx<N>::~cone_approx<N>()
{
    glDeleteVertexArrays(1, &side_vao);
    glDeleteVertexArrays(1, &bottom_vao);

    glDeleteBuffers(1, &side_vbo);
    glDeleteBuffers(1, &bottom_vbo);

    glDeleteBuffers(1, &side_ebo);
}

template<uint N>
void cone_approx<N>::draw() const
{
    glBindVertexArray(bottom_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, N);

    glBindVertexArray(side_vao);
    glDrawElements(GL_TRIANGLES, 3 * N, GL_UNSIGNED_INT, nullptr);

    glBindVertexArray(0);
}

template<uint N>
void cone_approx<N>::generate_vertices()
{
    int i = 0;

    side_vertices[i++] = 0.0f;
    side_vertices[i++] = 1.0f;
    side_vertices[i++] = 0.0f;

    side_vertices[i++] = 0.0f;
    side_vertices[i++] = 0.0f;
    side_vertices[i++] = 0.0f;

    for (int j = 0; j < N; j++)
    {
        float angle = 2 * PI * j / N;
        float c = cosf(angle);
        float s = sinf(angle);
        side_vertices[i++] = c;
        side_vertices[i++] = 0.0f;
        side_vertices[i++] = s;

        side_vertices[i++] = c;
        side_vertices[i++] = 1.0f;
        side_vertices[i++] = s;
    }

    i = 0;
    float sign;
    for (int j = 0;  j < N; j++)
    {
        if (j % 2) sign = -1.0f;
        else sign = 1.0f;

        float angle = sign * ((j + 1) / 2) * 2 * PI / N;
        float c = cosf(angle);
        float s = sinf(angle);
        bottom_vertices[i++] = c;
        bottom_vertices[i++] = 0.0f;
        bottom_vertices[i++] = s;

        bottom_vertices[i++] = 0.0f;
        bottom_vertices[i++] = -1.0f;
        bottom_vertices[i++] = 0.0f;
    }

    i = 0;
    for (int j = 1; j < N; j++)
    {
        side_indices[i++] = 0;
        side_indices[i++] = j;
        side_indices[i++] = j + 1;
    }
    side_indices[i++] = 0;
    side_indices[i++] = N;
    side_indices[i] = 1;

    glGenBuffers(1, &side_vbo);
    glGenBuffers(1, &bottom_vbo);
    glGenBuffers(1, &side_ebo);

    glBindBuffer(GL_ARRAY_BUFFER, side_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(side_vertices), side_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, bottom_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bottom_vertices), bottom_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, side_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(side_indices), side_indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

#endif //KG3_CONE_APPROX_H
