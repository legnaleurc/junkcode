#include "database.h"

#include <QtCore/QtDebug>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

Database::Database():
	_db(QSqlDatabase::addDatabase("QSQLITE"))
{
	this->_db.setDatabaseName(":memory:");
//	this->_db.setDatabaseName("/tmp/b.sqlite");
	bool ok = this->_db.open();
	qDebug() << "db open" << ok;

	this->_init();
	this->_initShipType();
	this->_initDeck();
	this->_initShip();
	this->_initNDock();
}

void Database::createShipType(const QJsonValue &data) {
	auto ships = data.toArray();
	for (auto ship_ : ships) {
		auto ship = ship_.toObject();
		QVariantMap args;
		args.insert("api_id", ship.value("api_id").toInt());
		args.insert("api_sortno", ship.value("api_sortno").toInt());
		args.insert("api_name", ship.value("api_name").toString());
		args.insert("api_yomi", ship.value("api_yomi").toString());
		args.insert("api_stype", ship.value("api_stype").toInt());
		args.insert("api_ctype", ship.value("api_ctype").toInt());
		args.insert("api_cnum", ship.value("api_cnum").toInt());
		args.insert("api_enqflg", ship.value("api_enqflg").toString());
		args.insert("api_afterlv", ship.value("api_afterlv").toInt());
		args.insert("api_aftershipid", ship.value("api_aftershipid").toString());
		args.insert("api_fuel_max", ship.value("api_fuel_max").toInt());
		args.insert("api_bull_max", ship.value("api_bull_max").toInt());
		this->_insertInto("ship_type", args);
	}
}

void Database::createShip(const QJsonValue &data) {
	auto ships = data.toArray();
	for (auto ship_ : ships) {
		auto ship = ship_.toObject();
		QVariantMap args;
		args.insert("api_id", ship.value("api_id").toInt());
		args.insert("api_sortno", ship.value("api_sortno").toInt());
		args.insert("api_ship_id", ship.value("api_ship_id").toInt());
		args.insert("api_lv", ship.value("api_lv").toInt());
		args.insert("api_nowhp", ship.value("api_nowhp").toInt());
		args.insert("api_maxhp", ship.value("api_maxhp").toInt());
		args.insert("api_ndock_time", ship.value("api_ndock_time").toInt());
		args.insert("api_fuel", ship.value("api_fuel").toInt());
		args.insert("api_bull", ship.value("api_bull").toInt());
		this->_insertInto("ship", args);
	}
}

void Database::createDeck(const QJsonValue &data) {
	auto ships = data.toArray();
	for (auto ship_ : ships) {
		auto ship = ship_.toObject();
		QVariantMap args;
		args.insert("api_id", ship.value("api_id").toInt());
		args.insert("api_name", ship.value("api_name").toString());
		args.insert("mission_status", ship.value("mission_status").toInt());
		args.insert("mission_id", ship.value("mission_id").toInt());
		args.insert("mission_time", ship.value("mission_time").toInt());
		this->_insertInto("deck", args);
	}
}

void Database::createNDock(const QJsonValue &data) {
	auto ships = data.toArray();
	for (auto ship_ : ships) {
		auto ship = ship_.toObject();
		if (ship.value("api_ship_id").toInt() == 0) {
			continue;
		}
		QVariantMap args;
		args.insert("api_id", ship.value("api_id").toInt());
		args.insert("api_state", ship.value("api_state").toInt());
		args.insert("api_ship_id", ship.value("api_ship_id").toInt());
		args.insert("api_complete_time", ship.value("api_complete_time").toInt());
		this->_insertInto("ndock", args);
	}
}

void Database::_init() {
	QSqlQuery query(this->_db);
	query.prepare("PRAGMA foreign_keys = ON;");
	bool ok = query.exec();
	qDebug() << "db prepare" << ok;
}

void Database::_initShipType() {
	QSqlQuery query(this->_db);
	query.prepare("CREATE TABLE ship_type ("
				  "api_id INTEGER PRIMARY KEY, "
				  "api_sortno INTEGER, "
				  "api_name TEXT, "
				  "api_yomi TEXT, "
				  "api_stype INTEGER, "
				  "api_ctype INTEGER, "
				  "api_cnum INTEGER, "
				  "api_enqflg TEXT, "
				  "api_afterlv INTEGER, "
				  "api_aftershipid TEXT, "
				  "api_fuel_max INTEGER, "
				  "api_bull_max INTEGER);");
	bool ok = query.exec();
	qDebug() << "db create table" << ok;
}

void Database::_initDeck() {
	QSqlQuery query(this->_db);
	query.prepare("CREATE TABLE deck ("
				  "api_id INTEGER PRIMARY KEY, "
				  "api_name TEXT, "

				  // 0: no mission, 1: in a mission, 2: mission completed
				  "mission_status INTEGER, "
				  "mission_id INTEGER, "
				  "mission_time INTEGER);");
	bool ok = query.exec();
	qDebug() << "db create table" << ok;
}

void Database::_initShip() {
	QSqlQuery query(this->_db);
	query.prepare("CREATE TABLE ship ("
				  "api_id INTEGER PRIMARY KEY, "
				  "api_sortno INTEGER, "
				  "api_ship_id INTEGER, "
				  "api_lv INTEGER, "
				  "api_nowhp INTEGER, "
				  "api_maxhp INTEGER, "
				  "api_ndock_time INTEGER, "
				  "api_fuel INTEGER, "
				  "api_bull INTEGER, "
				  "deck_id INTEGER, "
				  "FOREIGN KEY (api_ship_id) REFERENCES ship_type (api_id), "
				  "FOREIGN KEY (deck_id) REFERENCES deck (api_id));");
	bool ok = query.exec();
	qDebug() << "db create table" << ok;
}

void Database::_initNDock() {
	QSqlQuery query(this->_db);
	query.prepare("CREATE TABLE ndock ("
				  "api_id INTEGER PRIMARY KEY, "
				  "api_state INTEGER, "
				  "api_ship_id INTEGER, "
				  "api_complete_time INTEGER, "
				  "FOREIGN KEY (api_ship_id) REFERENCES ship (api_id));");
	bool ok = query.exec();
	qDebug() << "db create table" << ok;
}

void Database::_insertInto(const QString &table, const QVariantMap &args) {
	auto columns = QStringList(args.keys());
	auto names = columns.join(", ");
	std::transform(columns.begin(), columns.end(), columns.begin(), [](const QString & column)->QString {
		return ":" + column;
	});
	auto values = columns.join(", ");
	auto statement = QString("INSERT INTO %1 (%2) VALUES (%3);").arg(table).arg(names).arg(values);
	QSqlQuery query(this->_db);
	bool ok = query.prepare(statement);
	if (!ok) {
		qDebug() << "db insert into" << query.lastError().text();
	}
	for (auto it = args.begin(); it != args.end(); ++it) {
		query.bindValue(":" + it.key(), it.value());
	}
	ok = query.exec();
	if (!ok) {
		qDebug() << "db insert into" << query.lastError().text();
	}
}
