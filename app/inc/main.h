/*
 * main.h
 *
 *  Created on: 23 ao�t 2020
 *      Author: Arnaud
 */

#ifndef APP_INC_MAIN_H_
#define APP_INC_MAIN_H_

/*
 * printf() and sprintf() from printf-stdarg.c
 */

int my_printf	(const char *format, ...);
int my_sprintf	(char *out, const char *format, ...);
void AnalyseTramesRecues(void);
void ConfigurationCapteurs(char* p_donnee);
void LectureCapteur(char* p_donnee);

#endif /* APP_INC_MAIN_H_ */
