/* 
 * File:   Conference.cpp
 * Author: Kapil Thakkar
 * 
 */

#include "Conference.h"

Conference::Conference ( )
{
    this->parallelTracks = 0;
    this->sessionsInTrack = 0;
    this->papersInSession = 0;
    this->totalPapers = 0;
}

Conference::Conference ( int parallelTracks, int sessionsInTrack, int papersInSession )
{
    this->parallelTracks = parallelTracks;
    this->sessionsInTrack = sessionsInTrack;
    this->papersInSession = papersInSession;
    this->totalPapers = parallelTracks*sessionsInTrack*papersInSession;
    initTracks ( parallelTracks, sessionsInTrack, papersInSession );
}

void Conference::initTracks ( int parallelTracks, int sessionsInTrack, int papersInSession )
{
    tracks = ( Track * ) malloc ( sizeof (Track ) * parallelTracks );
    for ( int i = 0; i < parallelTracks; i++ )
    {
        Track tempTrack ( sessionsInTrack );
        for ( int j = 0; j < sessionsInTrack; j++ )
        {
            Session tempSession ( papersInSession );
            tempTrack.setSession ( j, tempSession );
        }
        tracks[i] = tempTrack;
    }
}

int Conference::getParallelTracks ( )
{
    return parallelTracks;
}

int Conference::getSessionsInTrack ( )
{
    return sessionsInTrack;
}

int Conference::getPapersInSession ( )
{
    return papersInSession;
}

int Conference::getTotalPapers ( )
{
    return totalPapers;
}

Track Conference::getTrack ( int index )
{
    if ( index < parallelTracks )
    {
        return tracks[index];
    }
    else
    {
        cout << "Index out of bound - Conference::getTrack" << endl;
        exit ( 0 );
    }
}


int Conference::getPaperId (int trackIndex, int sessionIndex, int paperIndex)
{
    if ( this->parallelTracks > trackIndex )
    {
        Track curTrack = tracks[trackIndex];
        curTrack.getSession(sessionIndex).getPaper(paperIndex);
    }
    else
    {
        cout << "Index out of bound - Conference::setPaper" << endl;
        exit ( 0 );
    }
}

int Conference::getPaperId(int paperNum)
{
    int t = sessionsInTrack;
    int k = papersInSession;
    int k1 = paperNum%k;
    int t1 = (paperNum%(k*t)) - k1;
    int p1 = paperNum/(k*t);
    getPaperId(p1, t1, k1);
}

void Conference::setPaper ( int trackIndex, int sessionIndex, int paperIndex, int paperId )
{
    if ( this->parallelTracks > trackIndex )
    {
        Track curTrack = tracks[trackIndex];
        curTrack.setPaper ( sessionIndex, paperIndex, paperId );
    }
    else
    {
        cout << "Index out of bound - Conference::setPaper" << endl;
        exit ( 0 );
    }
}

void Conference::setPaper(int paperNum, int paperId)
{
    int t = sessionsInTrack;
    int k = papersInSession;
    int k1 = paperNum%k;
    int t1 = (paperNum%(k*t)) - k1;
    int p1 = paperNum/(k*t);
    setPaper(p1, t1, k1, paperId);
}

int* Conference::cumulativeIndexToCordinate(int cI)
{
    int arr[3];
    int t = sessionsInTrack;
    int k = papersInSession;
    arr[2] = cI%k;
    arr[1] = (cI%(k*t)) - arr[2];
    arr[0] = cI/(k*t);
    return arr;
}

void Conference::printConference (char * filename )
{
    ofstream ofile(filename);

    for ( int i = 0; i < sessionsInTrack; i++ )
    {
        for ( int j = 0; j < parallelTracks; j++ )
        {
            for ( int k = 0; k < papersInSession; k++ )
            {
                ofile<< tracks[j].getSession ( i ).getPaper ( k ) << " ";
            }
            if ( j != parallelTracks - 1 )
            {
                ofile<<"| ";
            }
        }
        ofile<<"\n";
    }
    ofile.close();
    cout<<"Organization written to ";
    printf("%s :)\n",filename);

}
