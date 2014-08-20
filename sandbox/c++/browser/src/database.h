#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql/QSqlDatabase>
#include <QtCore/QJsonValue>
#include <QtCore/QVariantMap>

class Database
{
public:
	Database();

	void createShipType(const QJsonValue & data);
	void createShip(const QJsonValue & data);
	void createDeck(const QJsonValue & data);
	void createNDock(const QJsonValue & data);

    void clearCache();

	bool needCharge(int api_deck_id) const;

    void updateMission(int api_deck_id, int api_mission_id, int api_complatetime);

private:
	void _init();
	void _initShipType();
	void _initDeck();
	void _initShip();
	void _initNDock();

	void _insertInto(const QString & table, const QVariantMap & args);

	QSqlDatabase _db;
};

#endif // DATABASE_H
