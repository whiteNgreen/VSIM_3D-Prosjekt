#include "hoydekart.h"
#include <iostream>
#include <ostream>


HoydeKart::HoydeKart(Shader* shader, float scale)
{
    setShader(shader);

    /* Max & Min verdier for hoydedataen */
    float Xmax{}, Ymax{}, Zmax{};
    float Xmin{}, Ymin{}, Zmin{};
    int count{};    // Sjekker antall linjer/punkter det er i hoydedata fila

    /* Leser hoydedata fila */
    std::string filnavn = "../VSIM_3D-Prosjekt/HoydeKart/SmallArea.txt";
    std::ifstream inn;
    inn.open(filnavn.c_str());
    if (inn.is_open())
    {
        float X{}, Y{}, Z{};
        int c{};
        int i{};
        while (true)
        {
            if (i == 100)
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


                    mPunktdata.push_back(QVector3D{X, Y, Z});
                    /* Creating vertex */
    //                mVertices.push_back(Vertex{ X, Y, Z });
                    count++;

                    c = 0;
                }
                i = 0;
            }
            i++;

            if (inn.eof()){ break; }
        }

        inn.close();
    }
    else
    {
        LogError("Could not open file");
    }

//    for (unsigned int i{}; i < mVertices.size(); i++)
//    {
//        mVertices[i].SubtractPosition(Xmin, Ymin, Zmin);
//        mVertices[i].ScalePosition(scale);
//        mIndices.push_back(i);  // Funker ikke, vil bare kunne tegne meshen nå for å sjekke om lesing av fila funka
//    }
    /* Trekker fra minimums verdiene for xyz slik at terrenget blir tegnet i origo */
    for (unsigned int i{}; i < mPunktdata.size(); i++)
    {
        mPunktdata[i] -= QVector3D{Xmin, Ymin, Zmin};
        LogVector("Punkt: " + std::to_string(i), mPunktdata[i]);
    }

    /* Line count and Max & Min values of the area */
    Xmax -= Xmin;
    Ymax -= Ymin;
    Zmax -= Zmin;

    LogValue("Line count", count);
    LogValue("Xmax", Xmax);
//    LogValue("Xmin", Xmin);
    LogValue("Ymax", Ymax);
//    LogValue("Ymin", Ymin);
    LogValue("Zmax", Zmax);
//    LogValue("Zmin", Zmin);


    /* Regulær Triangulering */
    float resolutionX{20};   // Bredde, brukt for å finne spesifike ruter via indekseringen
    float resolutionY{20};

    float StepLengthX = Xmax / resolutionX;
    float StepLengthY = Ymax / resolutionY;

    int index{};

    /* Lager alle rutene vi skal ha basert på resolution/oppløsningen som er ønskelig */
    for (int x{1}; x <= resolutionX; x++)
    {
        for (int y{1}; y <= resolutionY; y++)
        {
            index++;
            Rute rute(index, x, y);    // Kan omtenke hvordan indekseringen gjøres

            /* Setter grensene til ruten */
            rute.Xmin = StepLengthX * (x - 1);
            rute.Xmax = StepLengthX * x;

            rute.Ymin = StepLengthY * (y - 1);
            rute.Ymax = StepLengthY * y;

            mRuter.push_back(rute);

            Log("Lagde Rute: " + std::to_string(x) + ", " + std::to_string(y));
        }
    }

    /* Går igjennom alle punktene for å finne ut av hvilke rute de tilhører */
    for (unsigned int i{}; i < mPunktdata.size(); i++)
    {
        /* Sjekker hvilken rute punktet er innenfor */
        for (unsigned int k{}; k < mRuter.size(); k++)
        {
            if (
                mPunktdata[i].x() >= mRuter[k].Xmin &&
                mPunktdata[i].x() <= mRuter[k].Xmax &&
                mPunktdata[i].y() >= mRuter[k].Ymin &&
                mPunktdata[i].y() <= mRuter[k].Ymax
                )
            {
                mRuter[k].mPunkter.push_back(mPunktdata[i]);
            }
        }
    }

    /* Regner ut gjennomsnitts høyden til hver rute */
    for (unsigned int i{}; i < mRuter.size(); i++)
    {
        float z{};
        for (unsigned int p{}; p < mRuter[i].mPunkter.size(); p++)
        {
            z += mRuter[i].mPunkter[p].z();
        }
        z /= mRuter[i].mPunkter.size();
        mRuter[i].averageHeight = z;

        mRuter[i].MakeCenter();
//        LogVector("Center", mRuter[i].Center);

//        LogValue("Average height", mRuter[i].averageHeight);
//        Log("Rute Points: " + std::to_string(mRuter[i].mPunkter.size()));
    }

    /* Lag vertexer med rutene */
    // Nested For loop for(x for(y)) størrelsen basert på oppløsning
    // Må kunne finne en rute basert på denne indeksen  // Har det i notatene
    for (unsigned int i{}; i < mRuter.size(); i++)
    {
        mVertices.push_back(Vertex{mRuter[i].Center});
        LogVector("Center " + std::to_string(i), mRuter[i].Center);
    }
//    count = 0;
    for (int x{}; x < resolutionX - 1; x++)
    {
        for (int y{}; y < resolutionY - 1; y++)
        {
            /* Første triangel (b, a, c) */
            mIndices.push_back((x * resolutionY) + y + 1);      // b
            mIndices.push_back((x * resolutionY) + y);          // a
            mIndices.push_back(((x+1) * resolutionY) + y);      // c

            /* Andre triangel (b, c, d) */
            mIndices.push_back((x * resolutionY) + y + 1);      // b
            mIndices.push_back(((x+1) * resolutionY) + y);      // c
            mIndices.push_back(((x+1) * resolutionY) + y + 1);  // d
        }
    }

//    mIndices = {
//        1, 0, 2,
//        1, 2, 3,
//        3, 2, 4,
//        3, 4, 5
//    };

    Log("Vertex count: " + std::to_string(mVertices.size()));
    LogVector("Vertex 0", mVertices[0].getPosition());
}
