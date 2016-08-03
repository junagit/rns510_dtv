#include "AS_language.h"
#include "app_settings.h"

//#define Debugowanie 1
using namespace std;
//#define Debugowanie

const QString AS_Language::DEFAULT_LANGUAGE = QString("POLSKI");
const QString AS_Language::DEFAULT_LANGUAGE_EXTENSION = QString(".lang");
const QString AS_Language::DEFAULT_PATH_TO_LANG = QString("/usr/juna/bin");

QString AS_Language::getDefaultLanguage()
{
    return DEFAULT_LANGUAGE;
}

QString AS_Language::getDefaultLangExt()
{
    return DEFAULT_LANGUAGE_EXTENSION;
}

int AS_Language::getLanguageId(QString of)
{
    return languages.indexOf(of);
}

void AS_Language::changeFirstLangToDefaultLang()
{
    if (!languages.isEmpty())
    {
        const int FIRST_INDEX = 0;
        if (!(getLanguageId(DEFAULT_LANGUAGE) == FIRST_INDEX))
        {
            QString temp = languages.at(FIRST_INDEX);
            languages.removeFirst();
            QString defaultLang = DEFAULT_LANGUAGE + DEFAULT_LANGUAGE_EXTENSION;
            languages.removeAt(getLanguageId(defaultLang.toLower()));
            languages.insert(FIRST_INDEX, defaultLang.toLower());
            languages.append(temp);
        }
    }
}

void AS_Language::fillLanguagesByFilesNames()
{
    QStringList nameFilter("*" + DEFAULT_LANGUAGE_EXTENSION);
    QDir directory(DEFAULT_PATH_TO_LANG);
    languages = directory.entryList(nameFilter);
    changeFirstLangToDefaultLang();
}

QString AS_Language::getLanguageFileWithoutExt(int index)
{
    if (index < languages.size())
    {
        QString result = languages.at(index);
        result.resize(result.length() - DEFAULT_LANGUAGE_EXTENSION.length());
        return result;
    }
}

QStringList AS_Language::getLanguages() 
{
    return languages;
}

void AS_Language::ReadWhatLanguage(void)
{
    app_settings settings;
    #ifdef Debugowanie
    cout<<"!!!!! START !!!!! AS_Language::ReadWhatLanguage(void)"<<endl;
    #endif
//    ifstream FileReadLanguage;
//    FileReadLanguage.open("/usr/local/share/settings/language.juna");
//    if(FileReadLanguage.fail()){
//           system("cp /usr/bin/juna/settings/* /usr/local/share/settings/");
//           system("sync");
//        }
//    FileReadLanguage>>LanguageName;
//    FileReadLanguage>>LanguageFile;
//    FileReadLanguage.close();
    int lang = settings.getSystemLanguage();
    fillLanguagesByFilesNames();
    
    if(lang<0)
    {
        strcpy(LanguageName,"polski");
        strcpy(LanguageFile,"polski.lang");
        lang = 0;
        settings.saveSystemLanguage(lang);
    }
    else
    {
        if (lang < languages.size())
        {
            QString tempLang = languages.at(lang);
            strcpy(LanguageName, getLanguageFileWithoutExt(lang).toStdString().c_str());
            strcpy(LanguageFile,tempLang.toStdString().c_str());
        }
    }
    

    #ifdef Debugowanie
    cout<<"#####  AS_Language::ReadWhatLanguage(void)===>LanguageNameL:= "<<LanguageName<<endl;
    cout<<"#####  AS_Language::ReadWhatLanguage(void)===>LanguageFile:= "<<LanguageFile<<endl;
    cout<<"@@@@@ END @@@@@ AS_Language::ReadWhatLanguage(void)"<<endl;
    #endif
}


AS_Language::AS_Language(char *ProgramName,int HowMuch)
{
    app_settings settings;
    int lang;
    
    #ifdef Debugowanie
    cout<<"!!!!! START !!!!! AS_Language::AS_Language(char *ProgramName:="<<ProgramName<<",char **Context,int *HowMuch:="<<HowMuch<<")"<<endl;
    #endif
    HowMuchElement=0;
    ifstream FileReadLanguage;
    ReadWhatLanguage();
    char FlienChar[165]="/usr/juna/bin/";
    strcat(FlienChar,LanguageFile);
    #ifdef Debugowanie
    cout<<"##### AS_Language::AS_Language(char *ProgramName,char **Context,int *HowMuch)====>FlienChar:= "<<FlienChar<<endl;
    #endif
    char HelpContext[500];
    Context= new char *[HowMuch];

    FileReadLanguage.open(FlienChar);

    //co jesli nie znajdziemy pliku z tłumaczeniem?
    if(FileReadLanguage.fail())
    {            
        memset(FlienChar,0,165);
        
        strcpy(LanguageName,"polski");
        strcpy(LanguageFile,"polski.lang");
        lang = 0;
        settings.saveSystemLanguage(lang);
//        system("cp /usr/juna/bin/*.lang /media/localdata");
//        system("sync");
        strcpy(FlienChar,"/usr/juna/bin/");
        strcat(FlienChar,LanguageFile);
        FileReadLanguage.open(FlienChar);
     }
    
    #ifdef Debugowanie
    cout<<"##### AS_Language::AS_Language(char *ProgramName,char **Context,int *HowMuch) FILE WAS OPEN:= "<<FlienChar<<endl;
    #endif
    do
    {
      FileReadLanguage.getline(HelpContext,500);
    } while(strcmp(ProgramName,HelpContext) != 0 && !FileReadLanguage.eof());
    
    for(int i=0;i<HowMuch &&!FileReadLanguage.eof();i++)
    {
        Context[i] = NULL;
        FileReadLanguage.getline(HelpContext,500);
        Context[i]= new char[strlen(HelpContext)+1];
        strcpy(Context[i],HelpContext);
        HowMuchElement++;
        #ifdef Debugowanie
        cout<<"##### AS_Language::AS_Language(char *ProgramName,char **Context,int *HowMuch)====>Context["<<i<<"]:= "<<Context[i]<<"SIZE:= "<<strlen(Context[i])<<endl;
        #endif
    }
    FileReadLanguage.close();
    #ifdef Debugowanie
    cout<<"@@@@@ END @@@@@ AS_Language::AS_Language(char *ProgramName,char **Context,int *HowMuch)"<<endl;
    #endif
}


 AS_Language::~AS_Language()
 {
     if(Context!=NULL)
     {
        for(int i=0;i<HowMuchElement;i++)
        {
            delete []Context[i];
        }
        delete []Context;
     }
 }
