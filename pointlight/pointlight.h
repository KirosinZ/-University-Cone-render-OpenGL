//
// Created by Kiril on 05.04.2022.
//

#ifndef KG3_POINTLIGHT_H
#define KG3_POINTLIGHT_H

#define uint unsigned int

class pointlight
{
    float coords[3] = {
            0.0f, 0.0f, 0.0f
    };
    uint vao, vbo;

public:
    float Size;

    pointlight(float size = 1.0f);

    ~pointlight();

    void draw() const;
};


#endif //KG3_POINTLIGHT_H
