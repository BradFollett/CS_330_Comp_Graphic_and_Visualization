#ifndef SPHERE_H
#define SPHERE_H

#include "object.h"
#include <string>
#include <stdexcept>

class Sphere : public Object
{
public:
    // Constructor that accepts a std::string
    Sphere(const std::string& texturePath)
    {
        initializeSphere(texturePath.c_str());
    }

    // Constructor that accepts a const char*
    Sphere(const char* texturePath)
    {
        initializeSphere(texturePath);
    }

    // Initialize textures, vertices, etc.
    virtual bool initialize() override
    {
        // Initialization code if needed
        return true;
    }

    // Draw
    virtual void draw(GLint modelHandle) override
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

        glm::mat4 model = translation * rotation * scale;
        glUniformMatrix4fv(modelHandle, 1, GL_FALSE, glm::value_ptr(model));

        // Draw the triangles
        glDrawArrays(GL_TRIANGLES, 0, m_Meshes[0].vertices);
    }

    // Update based on fps
    virtual void update(float elapsed) override
    {
        // Update code if needed
    }

private:
    // Helper function to initialize the sphere
    void initializeSphere(const char* texturePath)
    {
        // Create mesh
        m_Meshes.push_back(makeSphere(0.5, 200, 200));

        GLuint textureId;
        if (!createTexture(texturePath, textureId))
        {
            // Handle texture loading error
            throw std::runtime_error(std::string("Failed to load texture: ") + texturePath);
        }
        m_Textures.push_back(textureId);
    }
};

#endif // SPHERE_H
