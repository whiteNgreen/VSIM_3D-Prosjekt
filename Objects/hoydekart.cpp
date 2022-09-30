#include "hoydekart.h"
#include <iostream>
#include <ostream>

#include <chrono>


HoydeKart::HoydeKart(Shader* shader, float scale)
{
    setShader(shader);

//    float Xmax{}, Ymax{}, Zmax{};
//    float Xmin{}, Ymin{}, Zmin{};

//    int Pointresolution{ 100 };
    std::string filnavn = "../VSIM_3D-Prosjekt/HoydeKart/SmallArea.txt";
    /* Leser hele punktskyen med en viss oppløsning
     * Velger hvert hundrede punkt (PointResolution) */
//    ReadComplexPointCloud(filnavn, 100);
    /* Skriver den nye simplifiserte punktskyen til en ny fil */
//    WriteSimplePointCloud();

    /* Leser hoydedata fra den simplifiserte punktskyen for å spare tid */
    ReadSimplePointCloud();

//    /* Leser hoydedata fila */
    {
//    int count{};    // Sjekker antall linjer/punkter det er i hoydedata fila
////    int Pointresolution{ 100 };

//    /* Setter Timer */
//    auto readfile_start = std::chrono::system_clock::now();

//    std::string filnavn = "../VSIM_3D-Prosjekt/HoydeKart/SmallArea.txt";
//    std::ifstream inn;
//    inn.open(filnavn.c_str());
//    if (inn.is_open())
//    {
//        float X{}, Y{}, Z{};
//        int c{};
//        int PointRead{};
//        while (true)
//        {
////            if (PointRead%10 == 0)
//            {
//                /* Reading inn points from file */
//                c++;
//                if (c == 1)
//                {
//                    inn >> X;
//                }
//                else if (c == 2)
//                {
//                    inn >> Y;
//                }
//                else if (c == 3)
//                {
//                    inn >> Z;

////                    X*=scale, Y*=scale, Z*=scale;

//                    /* Initializing the first round */
//                    if (count == 0)
//                    {
//                        Xmax = X; Xmin = X;
//                        Ymax = Y; Ymin = Y;
//                        Zmax = Z; Zmin = Z;
//                    }

//                    /* Checking if there is a new maximum or minimum value for each coordinate */
//                    /* X */
//                    if (X > Xmax){ Xmax = X; }
//                    if (X < Xmin){ Xmin = X; }
//                    /* Y */
//                    if (Y > Ymax){ Ymax = Y; }
//                    if (Y < Ymin){ Ymin = Y; }
//                    /* Z */
//                    if (Z > Zmax){ Zmax = Z; }
//                    if (Z < Zmin){ Zmin = Z; }

////                    if (PointRead % 10 == 0)
//                    if (PointRead == Pointresolution)
//                    {
//                        mPunktdata.push_back(QVector3D{X, Y, Z});
//                        PointRead = 0;
//                    }
//                    PointRead++;
//                    /* Creating vertex */
////                    mVertices.push_back(Vertex{ X, Y, Z });
//                    count++;

//                    c = 0;
//                }
//            }

//            if (inn.eof()){ break; }
//        }

//        inn.close();
//    }
//    else
//    {
//        LogError("Could not open file");
//    }

//    auto readfile_end = std::chrono::system_clock::now();
//    auto readfile_time = std::chrono::duration<double>(readfile_end - readfile_start);
//    Log("Readfile Timer = " + std::to_string(readfile_time.count()) + "s");

////    LogValue("Line count", count);
//    Log("Points : " + std::to_string(mPunktdata.size()));
////    return;
    }

    /* Kun for testing for å kunne se hvordan meshen ish skal kunne se ut */
//    for (unsigned int i{}; i < mPunktdata.size(); i++)
//    {
//        mVertices.push_back(Vertex{mPunktdata[i]});
//    }
    /* Setter vertices til nullpunktet i scenen */
//    for (unsigned int i{}; i < mVertices.size(); i++)
//    {
//        mVertices[i].SubtractPosition(Xmin, Ymin, Zmin);
//        mVertices[i].ScalePosition(scale);
//        mIndices.push_back(i);  // Funker ikke, vil bare kunne tegne meshen nå for å sjekke om lesing av fila funka
//    }

    /* Trekker fra minimums verdiene for xyz slik at terrenget blir tegnet i origo */
    for (unsigned int i{}; i < mPunktdata.size(); i++)
    {
        /* Setter punktene i Origo */
        mPunktdata[i] -= QVector3D{Xmin, Ymin, Zmin};
        /* Skalerer størrelsen på terrenget */
        mPunktdata[i] *= scale;

//        mVertices.push_back(Vertex{mPunktdata[i]}); // For test visualisering
    }
//    return;

    /* Line count and Max & Min values of the area */
    Xmax -= Xmin;
    Ymax -= Ymin;
//    Zmax -= Zmin;

//    Xmin *= scale;
//    Ymin *= scale;
//    Zmin *= scale;

    Xmax *= scale;
    Ymax *= scale;


//    LogValue("Line count", count);

    /* --- Timer for rute laging --- */
    auto makerute_start = std::chrono::system_clock::now();

    /* Regulær Triangulering */
    unsigned int resolutionX{3};   // Bredde, brukt for å finne spesifike ruter via indekseringen
    unsigned int resolutionY{3};

    float StepLengthX = Xmax / resolutionX;
    float StepLengthY = Ymax / resolutionY;

    int index{};

    /* Lager alle rutene vi skal ha basert på resolution/oppløsningen som er ønskelig */
    for (unsigned int x{1}; x <= resolutionX; x++)
    {
        for (unsigned int y{1}; y <= resolutionY; y++)
        {
            index++;
            Rute* rute = new Rute(m_shader, index, x, y);    // Kan omtenke hvordan indekseringen gjøres

            /* Setter grensene til ruten */
            rute->Xmin = StepLengthX * (x - 1);
            rute->Xmax = StepLengthX * x;

            rute->Ymin = StepLengthY * (y - 1);
            rute->Ymax = StepLengthY * y;

            rute->MakeRute();

            mRuter.push_back(rute);
        }
    }
    auto makerute_end = std::chrono::system_clock::now();
    auto makerute_time = std::chrono::duration<double>(makerute_end - makerute_start);
    Log("MakeRute Timer = " + std::to_string(makerute_time.count()) + "s");
//    return;


    /* --- Timer for rute laging --- */
    auto sjekkrute_start = std::chrono::system_clock::now();

    /* Går igjennom alle punktene for å finne ut av hvilke rute de tilhører */
    for (unsigned int i{}; i < mPunktdata.size(); i++)
    {
        /* Sjekker hvilken rute punktet er innenfor */
        for (unsigned int k{}; k < mRuter.size(); k++)
        {
            if (
                mPunktdata[i].x() >= mRuter[k]->Xmin &&
                mPunktdata[i].x() <= mRuter[k]->Xmax &&
                mPunktdata[i].y() >= mRuter[k]->Ymin &&
                mPunktdata[i].y() <= mRuter[k]->Ymax
                )
            {
                mRuter[k]->mPunkter.push_back(mPunktdata[i]);
            }
        }
    }

    auto sjekkrute_end = std::chrono::system_clock::now();
    auto sjekkrute_time = std::chrono::duration<double>(sjekkrute_end - sjekkrute_start);
    Log("SjekkRute Timer = " + std::to_string(sjekkrute_time.count()) + "s");
//    return;


    /* --- Timer for utregning av rute hoyde --- */
    auto rutehoyde_start = std::chrono::system_clock::now();

    /* Regner ut gjennomsnitts høyden til hver rute */
    for (unsigned int i{}; i < mRuter.size(); i++)
    {
        float z{};
        for (unsigned int p{}; p < mRuter[i]->mPunkter.size(); p++)
        {
            z += mRuter[i]->mPunkter[p].z();
        }
        z /= mRuter[i]->mPunkter.size();
        mRuter[i]->averageHeight = z;

        mRuter[i]->MakeCenter();
    }

    auto rutehoyde_end = std::chrono::system_clock::now();
    auto rutehoyde_time = std::chrono::duration<double>(rutehoyde_end - rutehoyde_start);
    Log("RuteHoyde Timer = " + std::to_string(rutehoyde_time.count()) + "s");
//    return;


    /* --- Timer for laging av meshen --- */
    auto lagmesh_start = std::chrono::system_clock::now();

    /* Lag vertexer med rutene */
    for (unsigned int i{}; i < mRuter.size(); i++)
    {
        mVertices.push_back(Vertex{mRuter[i]->Center});
    }
    for (unsigned int x{0}; x < resolutionX - 1; x++)
    {
        for (unsigned int y{0}; y < resolutionY - 1; y++)
        {
            unsigned int a = (x * resolutionY) + y;
            unsigned int b = (x * resolutionY) + y + 1;
            unsigned int c = ((x+1) * resolutionY) + y;
            unsigned int d = ((x+1) * resolutionY) + y + 1;

            /* Første triangel (b, a, c) */
            mIndices.push_back(b);
            mIndices.push_back(a);
            mIndices.push_back(c);

            CalculateNormalofTriangle(
                        mVertices[a],
                        mVertices[b],
                        mVertices[c]
                        );

            /* Andre triangel (b, c, d) */
            mIndices.push_back(b);
            mIndices.push_back(c);
            mIndices.push_back(d);

            CalculateNormalofTriangle(
                        mVertices[d],
                        mVertices[c],
                        mVertices[b]
                        );
        }
    }

    auto lagmesh_end = std::chrono::system_clock::now();
    auto lagmesh_time = std::chrono::duration<double>(lagmesh_end - lagmesh_start);
    Log("Lagmesh Timer = " + std::to_string(lagmesh_time.count()) + "s");

    WriteMeshToFile();
    return;


//    unsigned int i = 0;
//    mIndices = {
////        1, 0, 4
//        (i + 1), i, (i+1) * resolutionY,    // First Triangle
////        1, 4, 5
//        (i+1), (i+1) * resolutionY, (i+1) * resolutionY + 1 // Second Triangle
//    };

//    Log("Vertex count: " + std::to_string(mVertices.size()));
    //    LogVector("Vertex 0", mVertices[0].getPosition());
}

void HoydeKart::ReadComplexPointCloud(std::string file, int PointResolution)
{
    int count{};    // Sjekker antall linjer/punkter det er i hoydedata fila
//    int Pointresolution{ 100 };

    /* Setter Timer */
    auto readfile_start = std::chrono::system_clock::now();

    /* Leser hoydedata fila */
    std::ifstream inn;
    inn.open(file.c_str());
    if (inn.is_open())
    {
        float X{}, Y{}, Z{};
        int c{};
        int PointRead{};
        while (true)
        {
            {
                /* Reading inn points from file */
                c++;
                if (c == 1)
                {
                    inn >> X;
                }
                else if (c == 2)
                {
                    inn >> Y;
                }
                else if (c == 3)
                {
                    inn >> Z;

                    /* Initializing the first round */
                    if (count == 0)
                    {
                        Xmax = X; Xmin = X;
                        Ymax = Y; Ymin = Y;
                        Zmax = Z; Zmin = Z;
                    }

                    /* Checking if there is a new maximum or minimum value for each coordinate */
                    /* X */
                    if (X > Xmax){ Xmax = X; }
                    if (X < Xmin){ Xmin = X; }
                    /* Y */
                    if (Y > Ymax){ Ymax = Y; }
                    if (Y < Ymin){ Ymin = Y; }
                    /* Z */
                    if (Z > Zmax){ Zmax = Z; }
                    if (Z < Zmin){ Zmin = Z; }

//                    if (PointRead % 10 == 0)
                    if (PointRead == PointResolution)
                    {
                        mPunktdata.push_back(QVector3D{X, Y, Z});
                        PointRead = 0;
                    }
                    PointRead++;
                    /* Creating vertex */
//                    mVertices.push_back(Vertex{ X, Y, Z });
                    count++;

                    c = 0;
                }
            }

            if (inn.eof()){ break; }
        }

        inn.close();
    }
    else
    {
        LogError("Could not open file");
    }

    auto readfile_end = std::chrono::system_clock::now();
    auto readfile_time = std::chrono::duration<double>(readfile_end - readfile_start);
    Log("Readfile Timer = " + std::to_string(readfile_time.count()) + "s");

    LogValue("Line count", count);
    Log("Points : " + std::to_string(mPunktdata.size()));
}

void HoydeKart::WriteSimplePointCloud()
{
    std::ofstream ut;
    ut.open("../VSIM_3D-Prosjekt/HoydeKart/SimplifiedPointCloud.txt");

    if (ut.is_open())
    {
        for (unsigned int i{}; i < mPunktdata.size(); i++)
        {
            ut << mPunktdata[i].x() << " " << mPunktdata[i].y() << " " << mPunktdata[i].z() << std::endl;
        }
        ut.close();
    }
    else
    {
        LogError("Could not write Simple Point Cloud");
    }
}

void HoydeKart::ReadSimplePointCloud()
{
    int count{};    // Sjekker antall linjer/punkter det er i hoydedata fila

    /* Setter Timer */
    auto readfile_start = std::chrono::system_clock::now();

    /* Leser hoydedata fila */
    std::ifstream inn;
    inn.open("../VSIM_3D-Prosjekt/HoydeKart/SimplifiedPointCloud.txt");
    if (inn.is_open())
    {
        float X{}, Y{}, Z{};
        int c{};
        int PointRead{};
        while (true)
        {
            {
                /* Reading inn points from file */
                c++;
                if (c == 1)
                {
                    inn >> X;
                }
                else if (c == 2)
                {
                    inn >> Y;
                }
                else if (c == 3)
                {
                    inn >> Z;

                    /* Initializing the first round */
                    if (count == 0)
                    {
                        Xmax = X; Xmin = X;
                        Ymax = Y; Ymin = Y;
                        Zmax = Z; Zmin = Z;
                    }

                    /* Checking if there is a new maximum or minimum value for each coordinate */
                    /* X */
                    if (X > Xmax){ Xmax = X; }
                    if (X < Xmin){ Xmin = X; }
                    /* Y */
                    if (Y > Ymax){ Ymax = Y; }
                    if (Y < Ymin){ Ymin = Y; }
                    /* Z */
                    if (Z > Zmax){ Zmax = Z; }
                    if (Z < Zmin){ Zmin = Z; }

//                    if (PointRead % 10 == 0)
//                    if (PointRead == PointResolution)
                    {
                        mPunktdata.push_back(QVector3D{X, Y, Z});
                        PointRead = 0;
                    }
                    PointRead++;
                    /* Creating vertex */
//                    mVertices.push_back(Vertex{ X, Y, Z });
                    count++;

                    c = 0;
                }
            }

            if (inn.eof()){ break; }
        }

        inn.close();
    }
    else
    {
        LogError("Could not open file");
    }

    auto readfile_end = std::chrono::system_clock::now();
    auto readfile_time = std::chrono::duration<double>(readfile_end - readfile_start);
    Log("Readfile Timer = " + std::to_string(readfile_time.count()) + "s");

    LogValue("Line count", count);
    Log("Points : " + std::to_string(mPunktdata.size()));
}

void HoydeKart::WriteMeshToFile()
{
    std::ofstream ut;
    ut.open("../VSIM_3D-Prosjekt/HoydeKart/HoydeKartMesh.txt");

    if (ut.is_open())
    {
        for (unsigned int i{}; i < mVertices.size(); i++)
        {

            ut << mVertices[i] << std::endl;
        }
        ut.close();
    }
    else
    {
        LogError("Could not write mesh to file");
    }
}

void HoydeKart::GetMeshFromFile()
{
    std::ifstream inn;
    inn.open("../VSIM_3D-Prosjekt/HoydeKart/HoydeKartMesh.txt");

    if (inn.is_open())
    {
        Vertex vertex;
        while(true)
        {
            inn >> vertex;
            if (inn.eof()) break;
            mVertices.push_back(vertex);
        }
        inn.close();
    }
    else
    {
        LogError("Could not open file to Get Mesh");
    }
}

void HoydeKart::draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix)
{
    glUseProgram(m_shader->getProgram());
    m_shader->setUniformMatrix("mMatrix", mMatrix);
    m_shader->setUniformMatrix("pMatrix", projectionMatrix);
    m_shader->setUniformMatrix("vMatrix", viewMatrix);

    glBindVertexArray( mVAO );
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);

    glDrawArrays(GL_POINTS, 0, mVertices.size());
    glPointSize(2.5f);
    glBindVertexArray(0);

    for (const auto& it : mRuter)
    {
        it->draw(projectionMatrix, viewMatrix);
    }
}

Rute::Rute(Shader* shader, int index, int X, int Y)
{
    m_shader = shader;

    Xindex = X;
    Yindex = Y;
    TriangleIndex = index;
}
/* Rute rendering */
void Rute::draw(QMatrix4x4 &projectionMatrix, QMatrix4x4 &viewMatrix)
{
    glUseProgram(m_shader->getProgram());
    m_shader->setUniformMatrix("mMatrix", mMatrix);
    m_shader->setUniformMatrix("pMatrix", projectionMatrix);
    m_shader->setUniformMatrix("vMatrix", viewMatrix);

    glBindVertexArray( mVAO );
    glDrawElements(GL_LINE_LOOP, mIndices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
