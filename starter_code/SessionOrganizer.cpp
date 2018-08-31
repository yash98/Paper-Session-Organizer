/* 
 * File:   SessionOrganizer.cpp
 * Author: Kapil Thakkar
 * 
 */

#include <vector>
#include <stdlib.h>
#include <time.h>

#include "SessionOrganizer.h"
#include "Util.h"

SessionOrganizer::SessionOrganizer ( )
{
    parallelTracks = 0;
    papersInSession = 0;
    sessionsInTrack = 0;
    processingTimeInMinutes = 0;
    tradeoffCoefficient = 1.0;
}

SessionOrganizer::SessionOrganizer ( string filename )
{
    readInInputFile ( filename );
    conference = new Conference ( parallelTracks, sessionsInTrack, papersInSession );
}

void SessionOrganizer::organizePapers (clock_t startTime)
{
    double timeAvailable = (60*processingTimeInMinutes - 1)*CLOCKS_PER_SEC;
    randomStartStateGenerator();
    clock_t current = clock();
    int n = conference->getTotalPapers();
    int p = conference->getParallelTracks();
    int t = conference->getSessionsInTrack();
    int k = conference->getPapersInSession();
    while(timeAvailable > current-startTime) {
        randomGreedyWalk(n, p, t, k);
        // cout <<  << endl;
        current = clock();
        // cout << (timeAvailable - current + startTime)/CLOCKS_PER_SEC << endl;
    }
}

void SessionOrganizer::randomStartStateGenerator() 
{
    int paperCounter = 0;
    int paperId;
    int vectorIndex;
    vector<int> randomPageList;
    for (int i=0; i < conference->getTotalPapers(); i++) {
        randomPageList.push_back(paperCounter);
        paperCounter++;
    }
    for ( int i = 0; i < conference->getSessionsInTrack ( ); i++ )
    {
        for ( int j = 0; j < conference->getParallelTracks ( ); j++ )
        {
            for ( int k = 0; k < conference->getPapersInSession ( ); k++ )
            {
                vectorIndex = rand() % paperCounter;
                paperId = randomPageList.at(vectorIndex);
                randomPageList.erase(randomPageList.begin() + vectorIndex);
                conference->setPaper (j, i, k, paperId);
                paperCounter--;
            }
        }
    }
}

bool SessionOrganizer::randomGreedyWalk(int n, int p, int t, int k)
{
    int n1 = rand() % n;
    int n2 = rand() % (n-k);
    int n1Floor = n1 - (n1%k);
    if (n2>=n1Floor) {
        n2 += k;
    }
    vector<double> retScoreAndValues = swappedScore(n1, n2);
    // cout << retScoreAndValues[0] << " , " << n1 << " , " << n2 << endl;
    if (retScoreAndValues[0]>=0) {
        swapPaper(n1, n2, retScoreAndValues);
        return true;
    }
    return false;
}

void SessionOrganizer::swapPaper(int firstIndex, int secondIndex, vector<double> values)
{
    conference->setPaper(firstIndex, (int) values[2]);
    conference->setPaper(secondIndex, (int) values[1]);
}

vector<double> SessionOrganizer::swappedScore(int first, int second)
{
    int firstValue = conference->getPaperId(first);
    int secondValue = conference->getPaperId(second);
    vector<int> firstVtr = conference->cumulativeIndexToCordinate(first);
    vector<int> secondVtr = conference->cumulativeIndexToCordinate(second);
    int p1 = firstVtr[2];
    int t1 = firstVtr[1];
    int k1 = firstVtr[0];
    int p2 = secondVtr[2];
    int t2 = secondVtr[1];
    int k2 = secondVtr[0];

    // new similar cost
    // differences new-old same session similarity
    double score1 = 0.0;

    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {   
        if (i != p1) {
            Track tmpTrack1 = conference->getTrack ( i );
            Session tmpSession1 = tmpTrack1.getSession ( t1 );
            int indexOld = firstValue;
            int indexNew = secondValue;
            for ( int k = 0; k < tmpSession1.getNumberOfPapers ( ); k++ )
            {
                int index2 = tmpSession1.getPaper ( k );
                score2 += distanceMatrix[indexNew][index2] - distanceMatrix[indexOld][index2];
            }
        } else {
            Track tmpTrack = conference->getTrack ( p1 );
            Session tmpSession = tmpTrack.getSession ( t1 );
            int indexOld = firstValue;
            int indexNew = secondValue;
            for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
            {   
                if (k != k1) {
                    int index2 = tmpSession.getPaper(i);
                    score1 += distanceMatrix[indexOld][index2] - distanceMatrix[indexNew][index2] ;
                }
            }
        }
    }

    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {   
        if (i != p2) {
            Track tmpTrack1 = conference->getTrack ( i );
            Session tmpSession1 = tmpTrack1.getSession ( t2 );
            int indexOld = firstValue;
            int indexNew = secondValue;
            for ( int k = 0; k < tmpSession1.getNumberOfPapers ( ); k++ )
            {
                int index2 = tmpSession1.getPaper ( k );
                score2 += distanceMatrix[indexNew][index2] - distanceMatrix[indexOld][index2];
            }
        } else {
            Track tmpTrack = conference->getTrack ( p2 );
            Session tmpSession = tmpTrack.getSession ( t2 );
            int indexOld = firstValue;
            int indexNew = secondValue;
            for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
            {   
                if (k != k2) {
                    int index2 = tmpSession.getPaper(k);
                    score1 += distanceMatrix[indexOld][index2] - distanceMatrix[indexNew][index2] ;
                }
            }
        }
    }
    double diffInScore = score1 + tradeoffCoefficient*score2;
    vector<double> arr;
    arr.push_back(diffInScore);
    arr.push_back(firstValue);
    arr.push_back(secondValue);
    return arr;
}

void SessionOrganizer::readInInputFile ( string filename )
{
    vector<string> lines;
    string line;
    ifstream myfile ( filename.c_str () );
    if ( myfile.is_open ( ) )
    {
        while ( getline ( myfile, line ) )
        {
            //cout<<"Line read:"<<line<<endl;
            lines.push_back ( line );
        }
        myfile.close ( );
    }
    else
    {
        cout << "Unable to open input file";
        exit ( 0 );
    }

    if ( 6 > lines.size ( ) )
    {
        cout << "Not enough information given, check format of input file";
        exit ( 0 );
    }

    processingTimeInMinutes = atof ( lines[0].c_str () );
    papersInSession = atoi ( lines[1].c_str () );
    parallelTracks = atoi ( lines[2].c_str () );
    sessionsInTrack = atoi ( lines[3].c_str () );
    tradeoffCoefficient = atof ( lines[4].c_str () );

    int n = lines.size ( ) - 5;
    double ** tempDistanceMatrix = new double*[n];
    for ( int i = 0; i < n; ++i )
    {
        tempDistanceMatrix[i] = new double[n];
    }


    for ( int i = 0; i < n; i++ )
    {
        string tempLine = lines[ i + 5 ];
        string elements[n];
        splitString ( tempLine, " ", elements, n );

        for ( int j = 0; j < n; j++ )
        {
            tempDistanceMatrix[i][j] = atof ( elements[j].c_str () );
        }
    }
    distanceMatrix = tempDistanceMatrix;

    int numberOfPapers = n;
    int slots = parallelTracks * papersInSession*sessionsInTrack;
    if ( slots != numberOfPapers )
    {
        cout << "More papers than slots available! slots:" << slots << " num papers:" << numberOfPapers << endl;
        exit ( 0 );
    }
}

double** SessionOrganizer::getDistanceMatrix ( )
{
    return distanceMatrix;
}

void SessionOrganizer::printSessionOrganiser ( char * filename)
{
    conference->printConference ( filename);
}

double SessionOrganizer::scoreOrganization ( )
{
    // Sum of pairwise similarities per session.
    double score1 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession = tmpTrack.getSession ( j );
            for ( int k = 0; k < tmpSession.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession.getPaper ( k );
                for ( int l = k + 1; l < tmpSession.getNumberOfPapers ( ); l++ )
                {
                    int index2 = tmpSession.getPaper ( l );
                    score1 += 1 - distanceMatrix[index1][index2];
                }
            }
        }
    }

    // Sum of distances for competing papers.
    double score2 = 0.0;
    for ( int i = 0; i < conference->getParallelTracks ( ); i++ )
    {
        Track tmpTrack1 = conference->getTrack ( i );
        for ( int j = 0; j < tmpTrack1.getNumberOfSessions ( ); j++ )
        {
            Session tmpSession1 = tmpTrack1.getSession ( j );
            for ( int k = 0; k < tmpSession1.getNumberOfPapers ( ); k++ )
            {
                int index1 = tmpSession1.getPaper ( k );

                // Get competing papers.
                for ( int l = i + 1; l < conference->getParallelTracks ( ); l++ )
                {
                    Track tmpTrack2 = conference->getTrack ( l );
                    Session tmpSession2 = tmpTrack2.getSession ( j );
                    for ( int m = 0; m < tmpSession2.getNumberOfPapers ( ); m++ )
                    {
                        int index2 = tmpSession2.getPaper ( m );
                        score2 += distanceMatrix[index1][index2];
                    }
                }
            }
        }
    }
    double score = score1 + tradeoffCoefficient*score2;
    return score;
}
