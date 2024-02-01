/*
        Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PICKING_TECHNIQUE_H
#define PICKING_TECHNIQUE_H

#include <glad/glad.h>
#include "../headers/shaderClass.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

//#include "ogldev_types.h"

class PickingTechnique
{
public:

    PickingTechnique(Shader shader);

    virtual bool Init();

    void SetWVP(const glm::mat4 WVP);

    void SetObjectIndex(unsigned int ObjectIndex);

    void DrawStartCB(unsigned int DrawIndex);

    void Enable();

private:
    unsigned int ID;
    GLuint m_WVPLocation;
    GLuint m_drawIndexLocation;
    GLuint m_objectIndexLocation;
    
};

#endif  /* PICKING_TECHNIQUE_H */
