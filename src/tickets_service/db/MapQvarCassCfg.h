#ifdef QVARAINT_MAP_TYPE_TO_CASS
#define MAP_QVARAINT_TO_CASS_TYPE(QVAR_T, CASS_T) {QVAR_T, CASS_T},
#undef QVARAINT_MAP_TYPE_TO_CASS
#else
#define MAP_QVARAINT_TO_CASS_TYPE(QVAR_T, CASS_T)
#endif

MAP_QVARAINT_TO_CASS_TYPE(QVariant::Uuid,     "UUID")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::Int,      "INT")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::LongLong, "BIGINT")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::Bool,     "BOOLEAN")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::String,   "TEXT")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::Double,   "DOUBLE")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::DateTime, "TIMESTAMP")
MAP_QVARAINT_TO_CASS_TYPE(QVariant::List,     "SET")
