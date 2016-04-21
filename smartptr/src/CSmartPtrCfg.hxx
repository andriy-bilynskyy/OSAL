#ifndef CSMARTPTRCFG_H_
#define CSMARTPTRCFG_H_

#include <pthread.h>

#define lockObjectType          pthread_mutex_t
#define lockObjectINIT(obj)     (void)pthread_mutex_init(&obj, NULL)
#define lockObjectDESTROY(obj)  (void)pthread_mutex_destroy(&obj)
#define lockObjectLOCK(obj)     (void)pthread_mutex_lock(&obj)
#define lockObjectUNLOCK(obj)   (void)pthread_mutex_unlock(&obj)

#endif
