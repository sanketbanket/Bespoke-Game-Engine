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

//#include "ogldev/ogldev_util.h"
#include "technique.h"

PickingTechnique::PickingTechnique(Shader shader)
{
    ID = shader.ID;
}

bool PickingTechnique::Init()
{
    

    m_WVPLocation = glGetUniformLocation(ID,"gWVP");
    m_objectIndexLocation = glGetUniformLocation(ID, "gObjectIndex");
    m_drawIndexLocation = glGetUniformLocation(ID, "gDrawIndex");

    return true;
}


void PickingTechnique::SetWVP(const glm::mat4 WVP)
{
    glUniformMatrix4fv(m_WVPLocation, 1, GL_TRUE, glm::value_ptr(WVP));
}


void PickingTechnique::DrawStartCB(unsigned int DrawIndex)
{
    glUniform1ui(m_drawIndexLocation, DrawIndex);
}


void PickingTechnique::SetObjectIndex(unsigned int ObjectIndex)
{
    glUniform1ui(m_objectIndexLocation, ObjectIndex);
}

void PickingTechnique::Enable()
{
    glUseProgram(ID);
}