#ifndef FPSUBMIT_CONSTANTS_H_
#define FPSUBMIT_CONSTANTS_H_

static const char *API_KEY_URL = "http://acousticbrainz.org";
static const char *SUBMIT_URL = "http://acousticbrainz.org/%1/low-level";
//static const char *SUBMIT_URL = "http://127.0.0.1:8080/%1/low-level";
static const int AUDIO_LENGTH = 120;
static const int MAX_ACTIVE_FILES = 3;

static const int MAX_BATCH_SIZE = 100;
static const int MIN_BATCH_SIZE = 2;

#endif
