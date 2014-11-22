#ifndef MISSIONQUEUE_H
#define MISSIONQUEUE_H

#include <unordered_map>

#include "database.h"
#include "jobqueue.h"

class MissionQueue {
public:
    MissionQueue(Database & db, JobQueue * jq);

    void start(int api_deck_id, int api_mission_id);

private:
    qint64 _doStart(int api_deck_id, int api_mission_id);
    void _queueNext(qint64 msCompleteTime, int api_deck_id, int api_mission_id);
    void _onDone(const QtCoroutine::Yield & yield, int api_deck_id, int api_mission_id);

    std::unordered_map<int, qint64> _decks;
    Database & _db;
    JobQueue * _jq;
};

#endif
