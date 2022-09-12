#include "octahedronball.h"
#include "logger.h"

OctahedronBall::OctahedronBall(int recursions, float radius)
    : m_rekursjoner(recursions), m_indeks(0)
{
    mLogger = Logger::getInstance();

    mVertices.reserve(3*8*pow(4, m_rekursjoner));

    size = radius;
    /* Size of ball */
    QVector3D v0{    0,     0,   size};
    QVector3D v1{ size,     0,      0};
    QVector3D v2{    0,  size,      0};
    QVector3D v3{-size,     0,      0};
    QVector3D v4{    0, -size,      0};
    QVector3D v5{    0,     0,  -size};

    /* Subdivide ball */
    subDivide(v0, v1, v2, m_rekursjoner);
    subDivide(v0, v2, v3, m_rekursjoner);
    subDivide(v0, v3, v4, m_rekursjoner);
    subDivide(v0, v4, v1, m_rekursjoner);
    subDivide(v5, v2, v1, m_rekursjoner);
    subDivide(v5, v3, v2, m_rekursjoner);
    subDivide(v5, v4, v3, m_rekursjoner);
    subDivide(v5, v1, v4, m_rekursjoner);

    CalculateNormals();
}

void OctahedronBall::CalculatePhysics(float DeltaTime)
{
//    GetSurfaceInfo();
//    CalculateAcceleration(QVector3D SurfaceNormal);
//    UpdateVelocity(QVector3D Acceleration, float DeltaTime);
    //    UpdatePosition();
}

void OctahedronBall::GetSurfaceInfo(Bakke* bakken, float DeltaTime/* hakk metode atm */)
{
    QVector3D SurfaceHeight;
    QVector3D SurfaceNormal;

    Position = mMatrix.column(3).toVector3D();
    for (unsigned int i{1}; i <= bakken->mTriangleIndex; i++)
    {
        const bool b = bakken->BarySentricCoordinate( Position, SurfaceHeight, SurfaceNormal, i );

        if (b){ break; }
    }

    /* Test for z posisjonen */
    mMatrix.translate( 0, 0, SurfaceHeight.z() - Position.z() );

    CalculateAcceleration(SurfaceNormal);
    UpdateVelocity(DeltaTime);
    /* Sjekker hvor ballen befinner seg neste frame
     * Om det er innenfor en ny trekant så:
     *  1. Finn ut når den kommer over til en ny trekant
     *  2. Regn ut ny velocity basert på når den kommer over til den nye trekanten
     */
    UpdatePosition();
}

void OctahedronBall::CalculateAcceleration(QVector3D SurfaceNormal)
{
    qDebug() << "SurfaceNormal: " << SurfaceNormal;

    float nx{ SurfaceNormal.x() };
    float ny{ SurfaceNormal.y() };
    float nz{ SurfaceNormal.z() };
    Acceleration = QVector3D{nx * nz * Gravity, ny * nz * Gravity, (nz * nz * Gravity) - Gravity};
    qDebug() << " Acceleration: " << Acceleration;
}

void OctahedronBall::UpdateVelocity(float DeltaTime)
{
    Velocity += Acceleration * DeltaTime * 0.5f;
}

void OctahedronBall::UpdatePosition()
{
    mMatrix.translate(Velocity.x(), Velocity.y());
    Position = mMatrix.column(3).toVector3D();

    //mMatrix = mScale * mRotation * mPosition;
}

void OctahedronBall::subDivide(const QVector3D &a, const QVector3D &b, const QVector3D &c, int n)
{
    if (n>0)
    {
        QVector3D v1 = a+b; v1.normalize(); v1 *= size;
        QVector3D v2 = a+c; v2.normalize(); v2 *= size;
        QVector3D v3 = c+b; v3.normalize(); v3 *= size;
        subDivide(a, v1, v2, n-1);
        subDivide(c, v2, v3, n-1);
        subDivide(b, v3, v1, n-1);
        subDivide(v3, v2, v1, n-1);
    }
    else
    {
        lagTriangel(a,b,c);
    }
}

void OctahedronBall::CalculateNormals()
{
    for (unsigned int it{0}; it < mVertices.size(); it+=3)
    {
        QVector3D v1{ mVertices[it+1].getPosition() - mVertices[it].getPosition() };
        QVector3D v2{ mVertices[it+2].getPosition() - mVertices[it].getPosition() };
        QVector3D normal = QVector3D::crossProduct(v1, v2);
        normal.normalize();

        mVertices[it].setNormal(normal);
        mVertices[it+1].setNormal(normal);
        mVertices[it+2].setNormal(normal);
    }
}

void OctahedronBall::lagTriangel(const QVector3D &v1, const QVector3D &v2, const QVector3D &v3)
{
//    QVector3D color{20, 20, 200};
    mVertices.push_back(Vertex(v1));
    mVertices.push_back(Vertex(v2));
    mVertices.push_back(Vertex(v3));

    int m = mVertices.size();
    mIndices.push_back(m-3);
    mIndices.push_back(m-2);
    mIndices.push_back(m-1);
}

void OctahedronBall::init()
{
    //must call this to use OpenGL functions
    initializeOpenGLFunctions();

    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );

    //Vertex Buffer Object to hold vertices - VBO
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    // 1st attribute buffer : vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(0 )  );          // array buffer offset
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof(Vertex),  reinterpret_cast<GLvoid*>(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3rd attribute buffer : uvs
    glVertexAttribPointer(2, 2,  GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(6 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    glGenBuffers ( 1, &mEAB );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEAB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(GLuint), mIndices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void OctahedronBall::draw()
{
    m_shader->setUniformMatrix("mMatrix", mMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray( mVAO );
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
//    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

        glBindVertexArray(0);
}

void OctahedronBall::draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix)
{
    glUseProgram(m_shader->getProgram());
    m_shader->setUniformMatrix("mMatrix", mMatrix);
    m_shader->setUniformMatrix("pMatrix", projectionMatrix);
    m_shader->setUniformMatrix("vMatrix", viewMatrix);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
//    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    glBindVertexArray(0);
}
