#ifndef APICLIENT_H
#define APICLIENT_H

#include <vector>

#include <QtCore/QVariantMap>

class ApiClient {
public:
    QVariantMap charge(const std::vector<int> & api_id_items);
    QVariantMap mission(int api_deck_id, int api_mission_id);
};

#endif
