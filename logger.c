/*
 Copyright 2014-2017 Thumbtack, Inc.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

         http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/


#define _DEFAULT_SOURCE /* for vsyslog */

#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include "logger.h"


#define MK_LOGGER_LIST(f) \
    f(LOCAL0) f(LOCAL1) f(LOCAL2) f(LOCAL3) f(LOCAL4) \
    f(LOCAL5) f(LOCAL6) f(LOCAL7)
#define MK_LOGGER_PAIR(X) {LOG_##X, #X},


static int get_facility(config_t config)
{
    int i;
    /* map config_t strings to syslog constants */
    struct {
        int facility_const;
        char * facility_str;
    } facility[] = { MK_LOGGER_LIST(MK_LOGGER_PAIR) };

    if (! CFG_SYSLOG_FACILITY(config)) {
        return -1;
    }

    for (i = 0; i < 8; i++) {
        if (strcasecmp(CFG_SYSLOG_FACILITY(config), facility[i].facility_str) == 0) {
            return facility[i].facility_const;
        }
    }

    /* the facility is not local0..local7 */
    return -1;
}

extern void logger_open(config_t config)
{
    int facility = get_facility(config);

    if (facility == -1) {
        fprintf(stderr, "Invalid syslog facility: %s\n", CFG_SYSLOG_FACILITY(config));
        return;
    }

    openlog(NULL, LOG_CONS, facility);
}

extern void logger_write(int priority, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsyslog(priority, fmt, ap);
    va_end(ap);
}

extern void logger_close()
{
    closelog();
}
