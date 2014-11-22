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
//    this->_db.setDatabaseName("/tmp/a.sqlite");
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
    auto decks = data.toArray();
    for (auto deck_ : decks) {
        auto deck = deck_.toObject();
        QVariantMap args;

        // identity
        auto deck_id = deck.value("api_id").toInt();
        args.insert("api_id", deck_id);
        args.insert("api_name", deck.value("api_name").toString());

        // mission information
        auto api_mission = deck.value("api_mission").toArray();
        args.insert("mission_status", api_mission.at(0).toInt());
        args.insert("mission_id", api_mission.at(1).toInt());
        args.insert("mission_time", api_mission.at(2).toString().toInt());
        this->_insertInto("deck", args);

        // update ship's deck
        auto api_ship = deck.value("api_ship").toArray();
        for (auto ship : api_ship) {
            auto ship_id = ship.toInt();
            if (ship_id == 0) {
                continue;
            }
            QString statement = "UPDATE ship SET deck_id = :deck_id WHERE api_id = :ship_id;";
            QSqlQuery query(this->_db);
            bool ok = query.prepare(statement);
            if (!ok) {
                qDebug() << "db update" << query.lastError().text();
            }
            query.bindValue(":deck_id", deck_id);
            query.bindValue(":ship_id", ship_id);
            ok = query.exec();
            if (!ok) {
                qDebug() << "db update" << query.lastError().text();
            }
        }
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
                  "api_afterlv INTEGER, "
                  "api_aftershipid TEXT, "
                  "api_fuel_max INTEGER, "
                  "api_bull_max INTEGER);");
    bool ok = query.exec();
    qDebug() << "db create table" << ok;
}

void Database::clearCache() {
    QString statement = "DELETE FROM ndock;";
    QSqlQuery query(this->_db);
    bool ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    ok = query.exec();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    query.finish();

    statement = "DELETE FROM deck;";
    ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    ok = query.exec();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    query.finish();

    statement = "DELETE FROM ship;";
    ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    ok = query.exec();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    query.finish();
}

bool Database::needCharge(int api_deck_id) const {
    QString statement = "SELECT ship.api_id "
                        "FROM ship "
                        "JOIN ship_type ON ship.api_ship_id = ship_type.api_id "
                        "WHERE (ship.deck_id = :deck_id AND (ship.api_fuel < ship_type.api_fuel_max OR ship.api_bull < ship_type.api_bull_max));";
    QSqlQuery query(this->_db);
    bool ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    query.bindValue(":deck_id", api_deck_id);
    ok = query.exec();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    ok = query.next();
    qDebug() << "need charge" << ok << api_deck_id;
    return ok;
}

QVariantMap Database::getDeck(int api_deck_id) const {
    QString statement = "SELECT `mission_status`,`mission_time` FROM `deck` WHERE `api_id`=:api_deck_id;";
    QSqlQuery query(this->_db);
    bool ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
        return QVariantMap();
    }
    query.bindValue(":api_deck_id", api_deck_id);
    ok = query.exec();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
        return QVariantMap();
    }
    ok = query.next();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
        return QVariantMap();
    }
    QVariantMap result;
    result.insert("mission_status", query.value(0));
    result.insert("mission_time", query.value(1));
    return result;
}

void Database::updateMission(int api_deck_id, int api_mission_id, int api_complatetime) {
    // UPDATE ship SET deck_id = :deck_id WHERE api_id = :ship_id;
    QString statement = "UPDATE deck SET "
                        "mission_id = :api_mission_id, "
                        "mission_time = :api_complatetime "
                        "WHERE api_id = :api_deck_id;";
    QSqlQuery query(this->_db);
    bool ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
    query.bindValue(":api_deck_id", api_deck_id);
    query.bindValue(":api_mission_id", api_mission_id);
    query.bindValue(":api_complatetime", api_complatetime);
    ok = query.exec();
    if (!ok) {
        qDebug() << "db query" << query.lastError().text();
    }
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

    // insert or ignore
    auto statement = QString("INSERT OR IGNORE INTO %1 (%2) VALUES (%3);").arg(table).arg(names).arg(values);
    QSqlQuery query(this->_db);
    bool ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db insert or ignore into" << query.lastError().text();
    }
    for (auto it = args.begin(); it != args.end(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }
    ok = query.exec();
    if (!ok) {
        qDebug() << "db insert or ignore into" << query.lastError().text();
    }

    // update
    // FIXME hidden rule: always has `api_id` as PK
    columns = args.keys();
    columns.removeAll("api_id");
    std::transform(columns.begin(), columns.end(), columns.begin(), [](const QString & column)->QString {
        return QString("%1=:%1").arg(column);
    });
    values = columns.join(", ");
    statement = QString("UPDATE %1 SET %2 WHERE api_id=:api_id;").arg(table).arg(values);
    ok = query.prepare(statement);
    if (!ok) {
        qDebug() << "db update" << query.lastError().text();
    }
    for (auto it = args.begin(); it != args.end(); ++it) {
        query.bindValue(":" + it.key(), it.value());
    }
    ok = query.exec();
    if (!ok) {
        qDebug() << "db update" << query.lastError().text();
    }
}
