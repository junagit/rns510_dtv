/*
** macro.h
**
** Author: Yubo Dong (yubodong@gmail.com)
**
** Copyright (C) 2006 Yubo Dong
** 
*/
#ifndef DYB_MACRO_HEADER_FILE
#define DYB_MACRO_HEADER_FILE

//
// Coding style:
//        Always staring a macro with 'MACRO_'
//

//---------------------------------------------------------------------------------------------
//                          MACRO_SET_GET(OBJECT,DATA_TYPE,MEM_VAR_NAME) 
//---------------------------------------------------------------------------------------------
//  Parameters:
//                     OBJECT:  Used to follow set/get to construct the method name
//                  DATA_TYPE:  Any data type that this set/get will face to
//               MEM_VAR_NAME:  Member variable defined in the class which need to be set/get.
//
// Description:  Declare set/get method pair in a class.
//
//      Sample: 
//               MACRO_SET_GET(Color, LONG, m_Color) is actually equal to:
//
//               void setColor(LONG var)
//               {
//                    m_Color = var;
//               }
//               LONG getColor() const
//               {
//                    return m_Color;
//               }
//
//---------------------------------------------------------------------------------------------
//                                     Dong Yubo (Mar 21, 2003)
//---------------------------------------------------------------------------------------------

#define MACRO_SET_GET(OBJECT,DATA_TYPE,MEM_VAR_NAME) \
          void set##OBJECT(DATA_TYPE var) \
          {                               \
              MEM_VAR_NAME = var;         \
          }                               \
          DATA_TYPE get##OBJECT() const   \
          {                               \
              return (MEM_VAR_NAME);      \
          }                               


#define MACRO_SET(OBJECT,DATA_TYPE,MEM_VAR_NAME) \
          void set##OBJECT(DATA_TYPE var) \
          {                               \
              MEM_VAR_NAME = var;         \
          }                               


#define MACRO_GET(OBJECT,DATA_TYPE,MEM_VAR_NAME) \
          DATA_TYPE get##OBJECT() const   \
          {                               \
              return (MEM_VAR_NAME);      \
          }                               

#endif
