#ifndef VISUALOBJECT_H
#define VISUALOBJECT_H

#include <QOpenGLFunctions_4_1_Core>
#include <QMatrix4x4>
#include <vector>
#include <string>
#include "vertex.h"
#include "Shaders/shader.h"
#include "logger.h"


typedef std::pair<float, float> Position2D;

class VisualObject : public QOpenGLFunctions_4_1_Core
{
public:
    VisualObject();
    ~VisualObject();

    /* Vil kanskje fortsatt ta inn GLuint matrixUniform for at den ikke skal måtte settes i renderwindow
     * for hvert objekt, men det blir til senere. */
    virtual void init(/*GLuint matrixUniform*/);
    virtual void draw();
    virtual void draw(QMatrix4x4& projectionMatrix, QMatrix4x4& viewMatrix);
    virtual void drawLines();

    Position2D getPosition2D();
    QVector2D getPositionVector2D();
    QVector3D getPositionVector3D();

    void setPositionRotation(QMatrix4x4 matrix);

    void setShader(Shader* shader){ m_shader = shader; }
    Shader* m_shader{nullptr};  // Tar denne ut av private for øyeblikket

    void setTexture(GLint texture){ m_texture = texture; }
    uint32_t getTexture(){ return m_texture; }
//    class Shader* shader;

    QMatrix4x4 mMatrix;
    QVector3D mPositionVector;

    QMatrix4x4 mPosition;
    QMatrix4x4 mRotation;
    QMatrix4x4 mScale;

    std::string m_name{""};

    class Quadtree* m_quad{nullptr};

//    virtual bool sphereCollision(VisualObject* collisionObject);
    float getSphereColliderRadius(){ return sphereColliderRadius; }

//    virtual bool onCollision(VisualObject* collisionObject){ return false; }
    bool destroy{false};

    std::vector<Vertex> getVertices(){ return mVertices; }
    std::vector<GLuint> getIndices(){ return mIndices; }

//    virtual void update(float deltaTime){}

protected:
    std::vector<Vertex> mVertices;
    std::vector<GLuint> mIndices;

    GLuint mVAO{0};
    GLuint mVBO{0};
    GLuint mEAB{0}; // Holds the indices - Element Array Buffer - EAB
//    GLuint MatrixUniform{0};


    float sphereColliderRadius{ 0.3f };

    uint32_t m_texture{ 0 };

    Logger* mLogger{nullptr};
};

#endif // VISUALOBJECT_H
