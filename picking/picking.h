#ifndef PICKING_TEXTURE_H
#define PICKING_TEXTURE_H

#include <glad/glad.h>
#include <stdio.h>

class PickingTexture
{
public:
    PickingTexture() {}

    ~PickingTexture();

    void Init(unsigned int WindowWidth, unsigned int WindowHeight);

    void EnableWriting();

    void DisableWriting();

    void Clear();

    struct PixelInfo {
        unsigned int ObjectID = 0;
        unsigned int DrawID = 0;
        unsigned int PrimID = 0;

        void Print()
        {
            printf("Object %d draw %d prim %d\n", ObjectID, DrawID, PrimID);
        }
    };

    PixelInfo ReadPixel(unsigned int x, unsigned int y);

private:
    GLuint m_fbo = 0;
    GLuint m_pickingTexture = 0;
    GLuint m_depthTexture = 0;
};

#endif  /* PICKING_TEXTURE_H */
