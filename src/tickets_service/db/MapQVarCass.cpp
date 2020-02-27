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
        cass_int64_t e;
        ERR_CHECK(cass_value_get_int64(in, &e));
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
        cass_int32_t out;
        ERR_CHECK(cass_value_get_int32(in, &out));
        timestamp.setMSecsSinceEpoch(out);
        res = timestamp;
        ret = true;
        break;
    }
    case QVariant::List:  {
        QVariant list;
        if (cass_value_is_collection(in)) {

            res = list;
            ret = true;
            //////////////////////////////
            CassIterator* items_iterator = nullptr;
            cass_int32_t output;
            items_iterator = cass_iterator_from_collection(in);
            while (cass_iterator_next(items_iterator)) {
                const CassValue * val = cass_iterator_get_value(items_iterator);
                output = -1;

                if (cass_value_type(val) == CASS_VALUE_TYPE_INT) {
                    cass_value_get_int32(val, &output);
                }
                qDebug() << "item: " << output;
            }
            cass_iterator_free(items_iterator);


            /////////////////////////////////

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

    case QVariant::Uuid:
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
        res = in.toString();
        ret = true;
        break;
    }
    case QVariant::List:  {
#if 0
        QVariant list;
        if (cass_value_is_collection(in)) {

            res = list;
            ret = true;
            //////////////////////////////
            CassIterator* items_iterator = nullptr;
            cass_int32_t output;
            items_iterator = cass_iterator_from_collection(in);
            while (cass_iterator_next(items_iterator)) {
                const CassValue * val = cass_iterator_get_value(items_iterator);
                output = -1;

                if (cass_value_type(val) == CASS_VALUE_TYPE_INT) {
                    cass_value_get_int32(val, &output);
                }
                qDebug() << "item: " << output;
            }
            cass_iterator_free(items_iterator);


            /////////////////////////////////

        }
#endif
        break;
    }
    default: {
        Q_ASSERT_X(0, "convertQVariantToStrout", QString("Uncknown type: %1").arg(type).toUtf8().constData());
        break;
    }
    }
    return ret;
}
