#include "MapQVarCass.h"
#include <QDateTime>
#include <QMap>
#include <QUuid>
#include <QDebug>

const static QMap<QVariant::Type, const char*> s_qvarTypeCass = {
    #define QVARAINT_MAP_TYPE_TO_CASS
    #include "MapQvarCassCfg.h"
};

const char *MapQVarCass::qvarTypeCassName(QVariant::Type type)
{
    return s_qvarTypeCass.value(type, nullptr);
}

#define ERR_CHECK(x)  do {\
    if((x) != CASS_OK) {\
    break;\
    }\
    } while(0)



bool MapQVarCass::convertCassToQVariant(const CassValue *in, QVariant::Type type, QVariant &res)
{
    bool ret = false;

    switch (type) {

    case QVariant::Uuid: {
        CassUuid uuid;
        char uuid_str[CASS_UUID_STRING_LENGTH];

        ERR_CHECK(cass_value_get_uuid(in, &uuid));
        cass_uuid_string(uuid, uuid_str);
        res = QUuid(uuid_str);
        ret = true;
        break;
    }
    case QVariant::Int: {
        cass_int32_t e;
        ERR_CHECK(cass_value_get_int32(in, &e));
        res = int(e);
        ret = true;
        break;
    }
    case QVariant::Bool:  {
        cass_bool_t e;
        ERR_CHECK(cass_value_get_bool(in, &e));
        res = bool(e);
        ret = true;
        break;
    }
    case QVariant::String:  {
        const char* output;
        size_t output_length;

        ERR_CHECK(cass_value_get_string(in, &output, &output_length));
        res = output;
        ret = true;
        break;
    }
    case QVariant::Double:  {
        cass_double_t out;

        ERR_CHECK(cass_value_get_double(in, &out));
        res = out;
        ret = true;
        break;
    }
    case QVariant::DateTime:  {
        QDateTime timestamp;
        cass_int64_t time;

        if( !cass_value_is_null(in) ) {
            ERR_CHECK(cass_value_get_int64(in, &time));
            timestamp.setMSecsSinceEpoch(time);
        }
        res = timestamp;
        ret = true;
        break;
    }
    case QVariant::List:  {
        QVariantList list;
        if (cass_value_is_collection(in)) {


            //////////////////////////////
            CassIterator* items_iterator = nullptr;

            items_iterator = cass_iterator_from_collection(in);
            while (cass_iterator_next(items_iterator)) {
                const CassValue * val = cass_iterator_get_value(items_iterator);

                switch (cass_value_type(val)) {

                case  CASS_VALUE_TYPE_INT: {
                    cass_int32_t output = -1;
                    cass_value_get_int32(val, &output);
                    list.append(output);
                    qDebug() << "item: " << output;
                    break;
                }
                default:
                    qCritical() <<__func__ << ":" << __LINE__
                            << " Undefined type: " << cass_value_type(val);
                    break;
                }

            }
            cass_iterator_free(items_iterator);
            /////////////////////////////////
            res = list;
            ret = true;

        }
        break;
    }
    default: {
        Q_ASSERT_X(0, "convertCassToQVariant", QString("Uncknown type: %1").arg(type).toUtf8().constData());
        break;
    }
    }
    return ret;
}

bool MapQVarCass::convertQVariantToStrout(const QVariant &in, QVariant::Type type, QString &res)
{
    bool ret = false;

    switch (type) {

    case QVariant::String: {
        res = QString("'%1'").arg(in.toString());
        ret = true;
        break;
    }
    case QVariant::Int: {
        res = in.toString();
        ret = true;
        break;
    }
    case QVariant::Bool:  {
        res = in.toString();
        ret = true;
        break;
    }
    case QVariant::Double:  {
        res = in.toString();
        ret = true;
        break;
    }
    case QVariant::DateTime:  {
        if (in.canConvert(QVariant::DateTime)) {
            QDateTime time = in.toDateTime();
            res = QString("'%1'").arg(in.toString());
        } else {
            res = QString("%1").arg("null");
        }
        ret = true;
        break;
    }
    case QVariant::Uuid:  {
        res = in.toString();
        ret = true;
        break;
    }
    case QVariant::List:  {

        if (in.canConvert<QVariantList>()) {

            QSequentialIterable iterable = in.value<QSequentialIterable>();
            QSequentialIterable::const_iterator it = iterable.begin();
            const QSequentialIterable::const_iterator end = iterable.end();

            ret = true;
            res = "{";

            for ( ; it != end;) {
                switch ((*it).type()) {
                case QVariant::Int:
                    res.append(QString("%1").arg((*it).toInt()));
                    break;
                case QVariant::String:
                    res.append(QString("'%1'").arg((*it).toString()));
                    break;
                default:
                    qCritical() << __func__<< ":" << __LINE__<< "Error: Not defined for type " << (*it).type();
                    ret = false;
                    break;
                }

                ++it;
                if (it != end) {
                    res.append(",");
                }
            }
            res.append("}");
        } else {
            qCritical() << __func__ << "Error in Line " <<__LINE__ << "Can't get variant list";
        }

        break;
    }
    default: {
        Q_ASSERT_X(0, "convertQVariantToStrout", QString("Uncknown type: %1").arg(type).toUtf8().constData());
        break;
    }
    }
    return ret;
}
