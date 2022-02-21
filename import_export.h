#ifndef IMPORT_EXPORT_H
#define IMPORT_EXPORT_H

#if defined MAKE_TEST_LIB
    #define TEST_LIB_EXPORT Q_DECL_EXPORT
#else
    #define TEST_LIB_EXPORT Q_DECL_IMPORT
#endif

#endif // IMPORT_EXPORT_H
