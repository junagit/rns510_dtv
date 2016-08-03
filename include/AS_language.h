#ifndef AS_Language_H
#define AS_Language_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <QString>
#include <QStringList>
#include <string>
//To make program without Debug Information delete this definition 
//#define Debugowanie


using namespace std;

class AS_Language
{
    char LanguageName[150];     //Name of Language now in use
    char LanguageFile[150];     //File name with word definition
    char **Context;
    int HowMuchElement;
    

    void ReadWhatLanguage(void);
    static const QString DEFAULT_LANGUAGE;
    static const QString DEFAULT_LANGUAGE_EXTENSION;
    static const QString DEFAULT_PATH_TO_LANG;
    QStringList languages;

//####################THIS FUNCTION YOU CAN USE#################################
public:
    // ProgramName =  array with your program name
    //                #MoviePlayer
    //                #ServiceManagment
    //                #DisplaySettings
    // Context = All Word needed for program, please put this not declared array
    // function make array (PUT ONLY POINTER NOT ARRAY )
    // HowMuch = How much Word have your Program
    AS_Language(char *ProgramName,int HowMuch);
    ~AS_Language();
    inline char **GetContext(void){return Context;}
    //Function for Batterfly to delete Array Context
    QString getDefaultLanguage();
    QString getDefaultLangExt();
    int getLanguageId(QString of);
    void changeFirstLangToDefaultLang();
    void fillLanguagesByFilesNames();
    QString getLanguageFileWithoutExt(int index);
    QStringList getLanguages();
};

#endif

