#ifndef FLOOR_H
#define FLOOR_H

#include "object.h"

class Floor : public Object
{
public:
    // Initialize textures, vertices, etc.
    virtual bool initialize()
    {
        // Texture coordinates for each vertex
        GLfloat verts[] =
        {
            //Positions             //Normals               //Texture Coords
            // ------------------------------------------------------
            // Front Face          //Negative Z Normal
            -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, -1.0f,      0.0f, 0.0f,
            0.5f, -0.5f, 0.0f,       0.0f, 0.0f, -1.0f,      1.0f, 0.0f,
            0.5f,  0.5f, 0.0f,       0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
            0.5f,  0.5f, 0.0f,       0.0f, 0.0f, -1.0f,      1.0f, 1.0f,
            -0.5f,  0.5f, 0.0f,      0.0f, 0.0f, -1.0f,      0.0f, 1.0f,
            -0.5f, -0.5f, 0.0f,      0.0f, 0.0f, -1.0f,      0.0f, 0.0f
        };

        // Create mesh
        GLMesh mesh;
        createMesh(verts, sizeof(verts), mesh);
        m_Meshes.push_back(mesh);

        GLuint textureId;
        if (!createTexture("./textures/wood.png", textureId))
        {
            return false;
        }
        m_Textures.push_back(textureId);

        return true;
    }

    // Draw
    virtual void draw(GLint modelHandle)
    {
        glm::mat4 scale = glm::scale(glm::vec3(m_Scale.x, m_Scale.y, m_Scale.z));
        glm::mat4 rotation = glm::rotate(m_Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)) *
                             glm::rotate(m_Rotation.y, glm::vec3(1.0f, 0.0f, 0.0f)) *
                             glm::rotate(m_Rotation.x, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 translation = glm::translate(glm::vec3(m_Position.x, m_Position.y, m_Position.z));
        // Activate the VBOs contained within the mesh's VAO
        glBindVertexArray(m_Meshes[0].vao);

        // Bind the face color
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Textures[0]);

        translation = glm::translate(glm::vec3(0.0f, -0.5f, 0.0f));
        rotation = glm::rotate(glm::radians(90.0f), glm::vec3(1.0, 0.0f, 0.0f));
        scale = glm::scale(glm::vec3(5.0f, 5.0f, 1.0f));
        glm::mat4 model = translation * rotation * scale;
        glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));

        // Draws the triangles
        glDrawArrays(GL_TRIANGLES, 0, m_Meshes[0].vertices);
    }

    // Update based on fps
    virtual void update(float elapsed)
    {

    }
};

#endif // FLOOR_H
