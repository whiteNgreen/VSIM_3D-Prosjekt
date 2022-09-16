#include "octahedronball.h"
#include "logger.h"

OctahedronBall::OctahedronBall(int recursions, float m_radius)
    : m_rekursjoner(recursions), m_indeks(0), radius(m_radius)
{
    mLogger = Logger::getInstance();

    mVertices.reserve(3*8*pow(4, m_rekursjoner));

    /* Size of ball */
    QVector3D v0{    0,     0,   radius};
    QVector3D v1{ radius,     0,      0};
    QVector3D v2{    0,  radius,      0};
    QVector3D v3{-radius,     0,      0};
    QVector3D v4{    0, -radius,      0};
    QVector3D v5{    0,     0,  -radius};

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

void OctahedronBall::Reset(const QVector3D& StartLocation)
{
    /* Move to start location */
    MoveTo(StartLocation);
//    QVector3D Move = StartLocation - mMatrix.column(3).toVector3D();
//    mMatrix.translate(Move);
//    Position = mMatrix.column(3).toVector3D();
//    Position_PreviousFrame = Position;

    /* Reset Movement Variables */
    Velocity_PreviousFrame *= 0.f;
    Velocity *= 0.f;
    Acceleration_PreviousFrame *= 0.f;
    Acceleration *= 0.f;

    /* Reset Debug Variables */
    FrameCount = 0;
}

void OctahedronBall::MoveTo(const QVector3D &Location)
{
    Position_PreviousFrame = Position;

    QVector3D Move = Location - mMatrix.column(3).toVector3D();
    mMatrix.translate(Move);
    Position = mMatrix.column(3).toVector3D();
}

//void OctahedronBall::CalculatePhysics(float DeltaTime)
//{
//    GetSurfaceInfo();
//    CalculateAcceleration(QVector3D SurfaceNormal);
//    UpdateVelocity(QVector3D Acceleration, float DeltaTime);
//    UpdatePosition();
//}


void OctahedronBall::CalculatePhysics(Bakke* bakken, float DeltaTime)
{
    FrameCount++;
    mLogger->logText("Frame: " + std::to_string(FrameCount));

    /* Object Position Variables */
    QVector3D ObjectPosition{ Position };
    QVector3D TranslationAdjustment{};

    /* Surface Variables */
    QVector3D SurfacePosition;
    QVector3D SurfaceNormal{0,0,1};
    QVector3D Baryc;
    unsigned int FinalTriangleIndex{};
    unsigned int tmpIndex{};
    float HeightDifferenceObjectSurface;
    float FrictionConstant{};
    QVector3D NormalForce{};
    float FrictionForce{};


    /* 1: Posisjonen og om den er på en overflate */
    {
        /* Få info om overflaten ballen er over */
        for (unsigned int i{1}; i <= bakken->mTriangleIndex; i++)
        {
            tmpIndex = bakken->BarySentricCoordinate( ObjectPosition, Baryc, SurfacePosition, SurfaceNormal, i );

            if (tmpIndex){ break; }
        }

        /* Hard koda friksjon konstant per triangel */
        switch(tmpIndex)
        {
        case 1:
            FrictionConstant = 0.2;
            break;
        case 2:
            FrictionConstant = 0.2;
            break;
        case 3:
            FrictionConstant = 0.4;
            break;
        case 4:
            FrictionConstant = 0.2;
            break;
        case 0:
            FrictionConstant = 0;
            break;
        default:
            break;
        }

        /* Normal force */
        NormalForce = SurfaceNormal * Gravity * SurfaceNormal.z();
        LogVector("NormalForce: ", NormalForce);

        /* Friction Force */
        FrictionForce = FrictionConstant * NormalForce.length();


        /* Sjekk om ballen er oppå flata */
        HeightDifferenceObjectSurface = SurfacePosition.z() - ObjectPosition.z();
        HeightDifferenceObjectSurface += radius;
        mLogger->logText("HeightDifference: " + std::to_string(HeightDifferenceObjectSurface));

        /* Hvis ballen er under flata så korrigerer vi høyden i z-aksen til å matche surface */
        if (HeightDifferenceObjectSurface >= 0.f)
        {
            FinalTriangleIndex = tmpIndex;
        }
        /* Hvis ballen er over flaten så skal den falle fritt nedover */
        else
        {
            FinalTriangleIndex = 0;
        }
    }

    /* 2: Akselerasjonen */
    {
        CalculateAcceleration(SurfaceNormal, FrictionForce, FinalTriangleIndex);
        LogVector("Acceleration", Acceleration);
    }

    /* 3: Hastigheten */
    {
        UpdateVelocity(DeltaTime);
        LogVector("Velocity", Velocity);
        mLogger->logText("Velocity Size: " + std::to_string(Velocity.length()));
    }

    QVector3D SurfaceNormal2{0,0,1};    // Hvis den kommer innenfor en ny trekant
    QVector3D SurfacePosition2{};

    /* 4: Den nye posisjonen */
    {
        /* Juster posisjonen etter hastigheten */
        ObjectPosition += Velocity;

        /* Sjekk de barysentriske koordinatene etter den nye posisjonen */
        for (unsigned int i{1}; i <= bakken->mTriangleIndex; i++)
        {
            tmpIndex = bakken->BarySentricCoordinate( ObjectPosition, Baryc, SurfacePosition2, SurfaceNormal2, i );

            if (tmpIndex){ break; }
        }
    }

    /* 5: Er den nye posisjonen gyldig? Nei?: Gjør justeringer */
    {
        /* Treffer kula en ny trekant? */
        if (tmpIndex != mTriangleIndex && mTriangleIndex != 0 && tmpIndex != 0)
        {
            /* Oppdater velocity */
            mLogger->logText("CAME OVER TO A NEW TRIANGLE!");
        }

//        QVector3D Height;
//        /* Setter kula oppå flata */
//        if (tmpIndex)
//        {
//            float theta = (M_PI/2) - (asinf(SurfaceNormal.z()));

//            Height = { 0, 0, radius / sinf(theta) };
//        }
        /* Sjekk om ballen er oppå flata */
        HeightDifferenceObjectSurface = SurfacePosition2.z() - ObjectPosition.z();
        HeightDifferenceObjectSurface += radius;
        mLogger->logText("HeightDifference: " + std::to_string(HeightDifferenceObjectSurface));

        /* Hvis ballen er under flata så korrigerer vi høyden i z-aksen til å matche surface */
        if (HeightDifferenceObjectSurface >= 0.f)
        {
            if (mTriangleIndex == 0)
            {
                mLogger->logText("LANDED ON TRIANGLE: " + std::to_string(tmpIndex));
                QVector3D n2{ QVector3D::crossProduct(QVector3D(0,0,1), QVector3D::crossProduct(SurfaceNormal, QVector3D(0,0,1))) }; n2.normalize();

                QVector3D n{ SurfaceNormal + n2 }; n.normalize();

                QVector3D Vetter{ Velocity - (2 * (Velocity*n)*n) };
                Velocity = Vetter;
                ObjectPosition += Vetter;
                LogVector("Velocity", Velocity);
                LogVector("n", n);
                LogVector("Vetter", Vetter);
            }

            HeightDifferenceObjectSurface = SurfacePosition2.z() - ObjectPosition.z();
            HeightDifferenceObjectSurface += radius;
            TranslationAdjustment += QVector3D(0, 0, HeightDifferenceObjectSurface);
            ObjectPosition += TranslationAdjustment;

            LogVector("TranslationAdjustment", TranslationAdjustment);



            FinalTriangleIndex = tmpIndex;
        }
    }

    /* 6: Sett den endelige posisjonen */
    {
        MoveTo(ObjectPosition);
        mTriangleIndex = FinalTriangleIndex;
    }

    mLogger->logText("Index : " + std::to_string(tmpIndex));
    mLogger->logText("TIndex: " + std::to_string(mTriangleIndex));

    mLogger->logText("");

    /*  ISSUE
     * "Mister" triangelet når den skal regne ut akselerasjonen for en frame ved kollisjonen mellom to planes
     */
}


void OctahedronBall::CalculateAcceleration(QVector3D SurfaceNormal, float FrictionForce, const int SurfaceIndex)
{
    Acceleration_PreviousFrame = Acceleration;

    /* If ball is free falling */
    if (SurfaceIndex == 0)
    {
        Acceleration = {0, 0, -Gravity};

        return;
    }
    else
    {
        /* If ball is on a surface */
        float nx{ SurfaceNormal.x() };
        float ny{ SurfaceNormal.y() };
        float nz{ SurfaceNormal.z() };
        Acceleration = QVector3D{nx * nz * Gravity, ny * nz * Gravity, (nz * nz * Gravity) - Gravity};

        QVector3D FrictionForceVector{ Acceleration * -1.f };
        FrictionForceVector.normalize();
        FrictionForceVector *= FrictionForce;
        Acceleration += FrictionForceVector;

        return;
    }
}

void OctahedronBall::UpdateVelocity(float DeltaTime)
{
    Velocity_PreviousFrame = Velocity;
    Velocity += Acceleration * DeltaTime;
}

void OctahedronBall::UpdatePosition(const QVector3D& Adjustment)
{
    Position_PreviousFrame = Position;

    mMatrix.translate(Velocity);
    mMatrix.translate( Adjustment );
//    mMatrix.translate( mMatrix.column(3).toVector3D() - SurfacePosition );
    Position = mMatrix.column(3).toVector3D();
}

void OctahedronBall::subDivide(const QVector3D &a, const QVector3D &b, const QVector3D &c, int n)
{
    if (n>0)
    {
        QVector3D v1 = a+b; v1.normalize(); v1 *= radius;
        QVector3D v2 = a+c; v2.normalize(); v2 *= radius;
        QVector3D v3 = c+b; v3.normalize(); v3 *= radius;
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

void OctahedronBall::LogVector(const std::string Name, const QVector3D Vector)
{
    std::string sVector = "( " + std::to_string(Vector.x()) + ", " +std::to_string(Vector.y()) + ", " + std::to_string(Vector.z()) + " )";
    mLogger->logText(Name + ": " + sVector);
}
