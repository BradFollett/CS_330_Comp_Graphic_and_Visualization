#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // Image loading Utility functions

// GLM Math Header inclusions
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static float k_PI = std::acos(-1.0);

struct GLMesh
{
    GLuint vao;         // Handle for the vertex array object
    GLuint vbo;         // Handle for the vertex buffer object
    GLuint vertices;    // Number of vertices of the mesh
    GLuint indices;     // Number of indices of the mesh (optional)
};

class Object
{
public:
    virtual ~Object()
    {
        for (auto mesh : m_Meshes)
        {
            glDeleteVertexArrays(1, &mesh.vao);
            glDeleteBuffers(1, &mesh.vbo);
        }

        for (auto textureId : m_Textures)
        {
            glGenTextures(1, &textureId);
        }
    }

    // Initialize textures, vertices, etc.
    virtual bool initialize() = 0;

    // Draw
    virtual void draw(GLint modelHandle) = 0;

    // Update based on fps
    virtual void update(float elapsed) = 0;

    // Move object
    virtual void move(float x, float y, float z)
    {
        m_Position = { x, y, z };
    }

    // Rotate object
    virtual void rotate(float yaw, float pitch, float roll)
    {
        m_Rotation = { yaw, pitch, roll };
    }

    // Scale object
    virtual void scale(float x, float y, float z)
    {
        m_Scale = { x, y, z };
    }

    // Create simple mesh from vertices
    static void createMesh(GLfloat verts[], size_t size, GLMesh& mesh)
    {
        const GLuint floatsPerVertex = 3;
        const GLuint floatsPerNormal = 3;
        const GLuint floatsPerUV = 2;

        mesh.vertices = size / (sizeof(GLfloat) * (floatsPerVertex + floatsPerNormal + floatsPerUV));

        glGenVertexArrays(1, &mesh.vao); // we can also generate multiple VAOs or buffers at the same time
        glBindVertexArray(mesh.vao);

        // Create 2 buffers: first one for the vertex data; second one for the indices
        glGenBuffers(1, &mesh.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo); // Activates the buffer
        glBufferData(GL_ARRAY_BUFFER, size, verts, GL_STATIC_DRAW); // Sends vertex or coordinate data to the GPU

        // Strides between vertex coordinates is 6 (x, y, z, r, g, b, a). A tightly packed stride is 0.
        GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);// The number of floats before each

        // Create Vertex Attribute Pointers
        glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
        glEnableVertexAttribArray(2);
    }

    /*Generate and load the texture*/
    static bool createTexture(const char* filename, GLuint& textureId)
    {
        int width, height, channels;
        unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
        if (image)
        {
            flipImageVertically(image, width, height, channels);

            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            if (channels == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            else if (channels == 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            else
            {
                std::cout << "Not implemented to handle image with " << channels << " channels" << std::endl;
                return false;
            }

            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(image);
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

            return true;
        }

        // Error loading the image
        return false;
    }

    static GLMesh makeCone(float radius, float height, std::uint32_t numSectors = 16)
    {
        // All vertex data
        std::vector<float> vertices;

        // Create circle
        std::vector<float> unitVertices;
        float sectorStep = 2 * k_PI / (float)numSectors;
        float sectorAngle;
        for (int i = 0; i <= numSectors; ++i)
        {
            sectorAngle = i * sectorStep;
            unitVertices.push_back(cos(sectorAngle)); // x
            unitVertices.push_back(sin(sectorAngle)); // y
            unitVertices.push_back(0);                // z
        }

        // Sides of cone
        float h = height / 2.0f;
        for(int j = 0, k = 0; j <= numSectors; ++j)
        {
            // Top 2 points
            for (int w = 0; w < 2; ++w, k += 3)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                float uz = unitVertices[k+2];
                // position vector
                vertices.push_back(ux * radius);             // x
                vertices.push_back(uy * radius);             // y
                vertices.push_back(h);                       // z
                // normal vector
                vertices.push_back(ux);                       // nx
                vertices.push_back(uy);                       // ny
                vertices.push_back(uz);                       // nz
                // texture coordinate
                vertices.push_back(0);      // s
                vertices.push_back(1 * w);      // t
            }

            // Start at first point
            k -= 6;

            // Last point and start of next triangle
            for (int i = 0; i < 2; ++i)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                float uz = unitVertices[k+2];
                // position vector
                vertices.push_back(ux * 0);             // x
                vertices.push_back(uy * 0);             // y
                vertices.push_back(-h);                       // z
                // normal vector
                vertices.push_back(ux);                       // nx
                vertices.push_back(uy);                       // ny
                vertices.push_back(uz);                       // nz
                // texture coordinate
                vertices.push_back(1);      // s
                vertices.push_back(i * 1);      // t
            }

            // Start at top point
            k += 3;

            // Bottom right and top right
            for (int i = 0; i < 2; ++i)
            {
                float h = height / 2.0f + i * -height;
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                float uz = unitVertices[k+2];
                // position vector
                vertices.push_back(ux * radius * i);             // x
                vertices.push_back(uy * radius * i);             // y
                vertices.push_back(-h);                       // z
                // normal vector
                vertices.push_back(ux);                       // nx
                vertices.push_back(uy);                       // ny
                vertices.push_back(uz);                       // nz
                // texture coordinate
                vertices.push_back(1 + i * -1);      // s
                vertices.push_back(1*i);      // t
            }
        }

        h = height / 2.0f;
        float nz = 1.0;
        for(int j = 0, k = 0; j < numSectors; ++j)
        {
            for (int w = 0; w < 2; ++w, k += 3)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                // position vector
                vertices.push_back(ux * radius);             // vx
                vertices.push_back(uy * radius);             // vy
                vertices.push_back(h);                       // vz
                // normal vector
                vertices.push_back(0);                        // nx
                vertices.push_back(0);                        // ny
                vertices.push_back(nz);                       // nz
                // texture coordinate
                vertices.push_back(-ux * 0.5f + 0.5f);      // s
                vertices.push_back(-uy * 0.5f + 0.5f);      // t
            }

            // Start at last point
            k -= 3;

            // Center vertex
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(h);
            vertices.push_back(0);
            vertices.push_back(0);
            vertices.push_back(nz);
            vertices.push_back(0.5f);
            vertices.push_back(0.5f);
        }

        GLMesh mesh;
        createMesh(vertices.data(), vertices.size() * sizeof(float), mesh);
        return mesh;
    }

    static GLMesh makeCylinder(float radius, float height, std::uint32_t numSectors = 16)
    {
        // All vertex data
        std::vector<float> vertices;

        // Create circle
        std::vector<float> unitVertices;
        float sectorStep = 2 * k_PI / (float)numSectors;
        float sectorAngle;
        for (int i = 0; i <= numSectors; ++i)
        {
            sectorAngle = i * sectorStep;
            unitVertices.push_back(cos(sectorAngle)); // x
            unitVertices.push_back(sin(sectorAngle)); // y
            unitVertices.push_back(0);                // z
        }

        // Sides of cylinder
        float h = -height / 2.0f;
        for(int j = 0, k = 0; j <= numSectors; ++j)
        {
            // Top 2 points
            for (int w = 0; w < 2; ++w, k += 3)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                float uz = unitVertices[k+2];
                // position vector
                vertices.push_back(ux * radius);             // x
                vertices.push_back(uy * radius);             // y
                vertices.push_back(h);                       // z
                // normal vector
                vertices.push_back(ux);                       // nx
                vertices.push_back(uy);                       // ny
                vertices.push_back(uz);                       // nz
                // texture coordinate
                vertices.push_back(0);      // s
                vertices.push_back(1 * w);      // t
            }

            // Start at first point
            k -= 6;

            // Last point and start of next triangle
            for (int i = 0; i < 2; ++i)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                float uz = unitVertices[k+2];
                // position vector
                vertices.push_back(ux * radius);             // x
                vertices.push_back(uy * radius);             // y
                vertices.push_back(-h);                       // z
                // normal vector
                vertices.push_back(ux);                       // nx
                vertices.push_back(uy);                       // ny
                vertices.push_back(uz);                       // nz
                // texture coordinate
                vertices.push_back(1);      // s
                vertices.push_back(i * 1);      // t
            }

            // Start at top point
            k += 3;

            // Bottom right and top right
            for (int i = 0; i < 2; ++i)
            {
                float h = -height / 2.0f + i * height;
                float ux = unitVertices[k];
                float uy = unitVertices[k+1];
                float uz = unitVertices[k+2];
                // position vector
                vertices.push_back(ux * radius);             // x
                vertices.push_back(uy * radius);             // y
                vertices.push_back(-h);                       // z
                // normal vector
                vertices.push_back(ux);                       // nx
                vertices.push_back(uy);                       // ny
                vertices.push_back(uz);                       // nz
                // texture coordinate
                vertices.push_back(1 + i * -1);      // s
                vertices.push_back(1*i);      // t
            }
        }

        for (int i = 0; i < 2; ++i)
        {
            float h = -height / 2.0f + i * height;
            float nz = -1.0 * i;
            for(int j = 0, k = 0; j < numSectors; ++j)
            {
                for (int w = 0; w < 2; ++w, k += 3)
                {
                    float ux = unitVertices[k];
                    float uy = unitVertices[k+1];
                    // position vector
                    vertices.push_back(ux * radius);             // vx
                    vertices.push_back(uy * radius);             // vy
                    vertices.push_back(h);                       // vz
                    // normal vector
                    vertices.push_back(0);                        // nx
                    vertices.push_back(0);                        // ny
                    vertices.push_back(nz);                       // nz
                    // texture coordinate
                    vertices.push_back(-ux * 0.5f + 0.5f);      // s
                    vertices.push_back(-uy * 0.5f + 0.5f);      // t
                }

                // Start at last point
                k -= 3;

                // Center vertex
                vertices.push_back(0);
                vertices.push_back(0);
                vertices.push_back(h);
                vertices.push_back(0);
                vertices.push_back(0);
                vertices.push_back(nz);
                vertices.push_back(0.5f);
                vertices.push_back(0.5f);
            }
        }

        GLMesh mesh;
        createMesh(vertices.data(), vertices.size() * sizeof(float), mesh);
        return mesh;
    }

    static GLMesh makeSphere(float radius, std::uint32_t stacks, std::uint32_t sectors)
    {
        std::vector<float> vertices;
        float lengthInv = 1.0f / radius;
        float sectorStep = 2 * k_PI / sectors;
        float stackStep = k_PI / stacks;

        for (int i = 0; i <= stacks; ++i)
        {
            float stackAngle = k_PI / 2 - i * stackStep;
            float xy = radius * std::cosf(stackAngle);
            float z = radius * std::sinf(stackAngle);

            for(int j = 0; j <= sectors; ++j)
            {
                float sectorAngle = j * sectorStep;

                // Position
                float x = xy * std::cosf(sectorAngle);
                float y = xy * std::sinf(sectorAngle);
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Normals
                vertices.push_back(x * lengthInv);
                vertices.push_back(y * lengthInv);
                vertices.push_back(z * lengthInv);

                // Textutre UV
                vertices.push_back((float)j / sectors);
                vertices.push_back((float)i / stacks);
            }
        }

        // Create indices
        std::vector<float> fullVertexData;
        int k1, k2;
        for (int i = 0; i < stacks; ++i)
        {
            k1 = i * (sectors + 1);     // beginning of current stack
            k2 = k1 + sectors + 1;      // beginning of next stack

            for(int j = 0; j < sectors; ++j, ++k1, ++k2)
            {
                if(i != 0)
                {
                    for (int w = 0; w < 8; ++w) fullVertexData.push_back(vertices[k1 * 8 + w]);
                    for (int w = 0; w < 8; ++w) fullVertexData.push_back(vertices[k2 * 8 + w]);
                    for (int w = 0; w < 8; ++w) fullVertexData.push_back(vertices[(k1 + 1) * 8 + w]);
                }

                if(i != (stacks-1))
                {
                    for (int w = 0; w < 8; ++w) fullVertexData.push_back(vertices[(k1 + 1) * 8 + w]);
                    for (int w = 0; w < 8; ++w) fullVertexData.push_back(vertices[k2 * 8 + w]);
                    for (int w = 0; w < 8; ++w) fullVertexData.push_back(vertices[(k2 + 1) * 8 + w]);
                }
            }
        }

        GLMesh mesh;
        createMesh(fullVertexData.data(), fullVertexData.size() * sizeof(float), mesh);
        return mesh;
    }

    // Images are loaded with Y axis going down, but OpenGL's Y axis goes up, so let's flip it
    static void flipImageVertically(unsigned char* image, int width, int height, int channels)
    {
        for (int j = 0; j < height / 2; ++j)
        {
            int index1 = j * width * channels;
            int index2 = (height - 1 - j) * width * channels;

            for (int i = width * channels; i > 0; --i)
            {
                unsigned char tmp = image[index1];
                image[index1] = image[index2];
                image[index2] = tmp;
                ++index1;
                ++index2;
            }
        }
    }

protected:
    Object() { }

    std::vector<GLMesh> m_Meshes;
    std::vector<GLuint> m_Textures;
    glm::vec3 m_Position = { 0, 0, 0 };
    glm::vec3 m_Rotation = { 0, 0, 0 };
    glm::vec3 m_Scale = { 1.0f, 1.0f, 1.0f };

};

#endif // OBJECT_H
