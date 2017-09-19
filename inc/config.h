#ifndef CONFIG_H_
#define CONFIG_H_

/* listen port on the server side */
#define PORTNUM    13000 

/* heart beat interval with unit us. */
#define HEARTINTERVAL (2000 * 1000)

/* It is recommended to leave below parameters unchanged. */
/* on server side, login info timeout of unit second
 * count down from TICKS to 0, then timeout */
#define TICKS_INI   9   

/* define buffer size, not originally BUFSIZ */
#define IBUFSIZ    1024

#endif

