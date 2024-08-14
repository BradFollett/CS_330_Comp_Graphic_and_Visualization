#ifndef PENCIL_H
#define PENCIL_H

#include "object.h"

class Pencil : public Object
{
public:
    // Initialize textures, vertices, etc.
    virtual bool initialize()
    {
        // Create body mesh
        m_Meshes.push_back(makeCylinder(0.1, 3, 6));

        // Create eraser mesh
        m_Meshes.push_back(makeCylinder(0.1, 0.2));

        // Create point mesh
        m_Meshes.push_back(makeCone(0.1, 0.1));

        GLuint textureId;
        if (!createTexture("./textures/pencil_color.png", textureId))
        {
            return false;
        }
        m_Textures.push_back(textureId);

        if (!createTexture("./textures/pencil_top.png", textureId))
        {
            return false;
        }
        m_Textures.push_back(textureId);

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
        glm::mat4 rotation = glm::rotate(glm::radians(m_Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                             glm::rotate(glm::radians(m_Rotation.y), glm::vec3(1.0f, 0.0f, 0.0f)) *
                             glm::rotate(glm::radians(m_Rotation.x), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 translation = glm::translate(glm::vec3(m_Position.x, m_Position.y, m_Position.z));

        glm::mat4 model = translation * rotation  * scale;
        glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));

        // Bind VAO
        glBindVertexArray(m_Meshes[0].vao);

        // Bind the face color
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Textures[0]);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, m_Meshes[0].vertices);

        // Draw eraser
        float pencilTopZ = 3.0 / 2 + 0.2 / 2;
        glm::vec4 transform = rotation * glm::vec4(0, 0, pencilTopZ, 0);
        glm::mat4 t2 = translation * glm::translate(glm::vec3(transform.x, transform.y, transform.z));

        model = t2 * rotation * scale;
        glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));

        // Bind VAO
        glBindVertexArray(m_Meshes[1].vao);

        // Bind the face color
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Textures[1]);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, m_Meshes[1].vertices);

        // Draw point
        pencilTopZ = 3.0 / 2 + 0.1 / 2;
        transform = rotation * glm::vec4(0, 0, -pencilTopZ, 0);
        glm::mat4 t3 = translation * glm::translate(glm::vec3(transform.x, transform.y, transform.z));

        model = t3 * rotation * scale;
        glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));

        // Bind VAO
        glBindVertexArray(m_Meshes[2].vao);

        // Bind the face color
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_Textures[2]);

        // Draw
        glDrawArrays(GL_TRIANGLES, 0, m_Meshes[2].vertices);
    }

    // Update based on fps
    virtual void update(float elapsed)
    {

    }

};

#endif // PENCIL_H
