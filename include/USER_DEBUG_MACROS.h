/*** 
 * @Author: nikejasonlyz
 * @Date: 2023-03-04 12:02:00
 * @LastEditors: nikejasonlyz
 * @LastEditTime: 2023-03-04 12:15:17
 * @FilePath: \DISCO_F746_testTFT\include\USER_DEBUG_MACROS.h
 * @Description: 
 * @
 * @Copyright (c) 2023 by nikejasonlyz, All Rights Reserved. 
 */
#if 1

#ifdef DEBUG
    #define DBG_BEGIN(baud)  { Serial.begin(baud); while(!Serial); }

    /*assert */
    #define DBG_ASSERT(A) if( !(A) ) { Serial.print("ERR:F=");Serial.print(__FILE__);Serial.print(" L=");Serial.println(__LINE__); delay(1000); while(1);}
    #define DBG_WARNING(A) if( !(A) ) { Serial.print("WARNING:F=");Serial.print(__FILE__);Serial.print(" L=");Serial.println(__LINE__); delay(500);}

    /*print information*/
    #define DBG_PRINT_FUNNAME  { Serial.print("INFO:Fun=");Serial.println(__func__);}  //put it into the begin of function, you can trace the flow of program
    #define DBG_PRINT(info)  { Serial.print(info);}
    #define DBG_PRINTLN(info)  { Serial.println(info);}
    #define DBG_PRINTLN_VAR(var, type)  { Serial.print(#var);  Serial.print("=");Serial.print(var, type); Serial.print(" @"); Serial.println(type);}


#else
    #define DBG_BEGIN

    #define DBG_ASSERT(A)
    #define DBG_WARNING(A)
    #define DBG_PRINT_FUNNAME
    #define DBG_PRINT(info) 
    #define DBG_PRINTLN(info)
    #define DBG_PRINTLN_VAR(val, type)
#endif

#endif