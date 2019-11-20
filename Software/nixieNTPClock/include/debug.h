#ifndef H_DEBUG
#define H_DEBUG

// Comment this line if you don't want debug fonctionalities
#define DEBUG

#ifdef DEBUG
#define debug_print(arg) Serial.print(arg)
#else
#define debug_print(arg)
#endif


#endif