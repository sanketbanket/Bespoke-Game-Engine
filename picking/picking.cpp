#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "picking.h"
#include "technique.h"
//#include "ogldev_util.h"
#include <stb/stb_image_write.h>

PickingTexture::~PickingTexture()
{
    if (m_fbo != 0) {
        glDeleteFramebuffers(1, &m_fbo);
    }

    if (m_pickingTexture != 0) {
        glDeleteTextures(1, &m_pickingTexture);
    }

    if (m_depthTexture != 0) {
        glDeleteTextures(1, &m_depthTexture);
    }
}


void PickingTexture::Init(unsigned int WindowWidth, unsigned int WindowHeight)
{
    // Create the FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Create the texture object for the primitive information buffer
    glGenTextures(1, &m_pickingTexture);
    glBindTexture(GL_TEXTURE_2D, m_pickingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, WindowWidth, WindowHeight, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pickingTexture, 0);

    // Create the texture object for the depth buffer
    glGenTextures(1, &m_depthTexture);
    glBindTexture(GL_TEXTURE_2D, m_depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, WindowWidth, WindowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture, 0);

    // Verify that the FBO is correct
    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE) {
        printf("FB error, status: 0x%x\n", Status);
        exit(1);
    }

    // Restore the default framebuffer
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void PickingTexture::EnableWriting()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}


void PickingTexture::DisableWriting()
{
    // Bind back the default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}


PickingTexture::PixelInfo PickingTexture::ReadPixel(unsigned int x, unsigned int y)
{
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);

    glReadBuffer(GL_COLOR_ATTACHMENT0);

    PixelInfo Pixel;
    glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &Pixel);

    //std::vector<GLubyte> imageData(1000 * 1000 * 4);
    //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData.data());

    GLubyte* pixels = new GLubyte[1000 * 1000 * 3]; //debug
    glReadPixels(0, 0, 1000, 1000, GL_RGB, GL_UNSIGNED_BYTE,pixels);   //debug
    GLubyte* rgbaData = new GLubyte[1000 * 1000 * 4];
    for (int i = 0; i < 1000 * 1000; ++i) {
        rgbaData[i * 4] = pixels[i * 3];     // Red channel
        rgbaData[i * 4 + 1] = pixels[i * 3 + 1]; // Green channel
        rgbaData[i * 4 + 2] = pixels[i * 3 + 2]; // Blue channel
        rgbaData[i * 4 + 3] = 255;            // Alpha channel (fully opaque)
    }
    int result = stbi_write_png("output.png", 1000, 1000, 4, rgbaData,1000*4);
    for (int i = 0;i < 1000000;++i) {
        int index = i * 3;  // Each pixel has 4 components (RGBA)
        printf("Pixel %d: R=%u, G=%u, B=%u\n", i,
            pixels[index], pixels[index + 1], pixels[index + 2]);

    }

    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    return Pixel;
}