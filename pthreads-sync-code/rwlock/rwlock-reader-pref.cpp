#include "rwlock.h"

void InitializeReadWriteLock(struct read_write_lock* rw){
    pthread_mutex_init(&rw->mutex, NULL);
    pthread_cond_init(&rw->readers_ok, NULL);
    pthread_cond_init(&rw->writers_ok, NULL);
    rw->readers = 0;
    rw->writers = 0;
    rw->waiting_writers = 0;
}

void ReaderLock(struct read_write_lock* rw) {
    pthread_mutex_lock(&rw->mutex);
    while (rw->writers > 0 || rw->waiting_writers > 0) {
        pthread_cond_wait(&rw->readers_ok, &rw->mutex);
    }
    rw->readers++;
    pthread_mutex_unlock(&rw->mutex);
}

void ReaderUnlock(struct read_write_lock * rw)
{
    pthread_mutex_lock(&rw->mutex);
    rw->readers--;
    if (rw->readers == 0) {
        pthread_cond_signal(&rw->writers_ok);
    }
    pthread_mutex_unlock(&rw->mutex);}

void WriterLock(struct read_write_lock * rw)
{
    pthread_mutex_lock(&rw->mutex);
    rw->waiting_writers++;
    while (rw->readers > 0 || rw->writers > 0) {
        pthread_cond_wait(&rw->writers_ok, &rw->mutex);
    }
    rw->waiting_writers--;
    rw->writers++;
    pthread_mutex_unlock(&rw->mutex);}

void WriterUnlock(struct read_write_lock * rw)
{
    pthread_mutex_lock(&rw->mutex);
    rw->writers--;
    if (rw->waiting_writers > 0) {
        pthread_cond_signal(&rw->writers_ok);
    } else {
        pthread_cond_broadcast(&rw->readers_ok);
    }
    pthread_mutex_unlock(&rw->mutex);}
