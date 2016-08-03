/*
** MP3Info.h
**
** Author: Yubo Dong (yubodong@gmail.com)
**
** Copyright (C) 2006 Yubo Dong
** 
*/
#pragma once
#include <string>
#include "macro.h"
using namespace std;

typedef basic_string <unsigned char> ustring;

class CMP3Info
{
private:
	ustring		m_strFile;
	ustring		m_strTitle;
	ustring		m_strArtist;
	ustring		m_strAlbum;
	ustring		m_strGenre;
	ustring		m_strYear;
	ustring		m_strTrack;
	ustring		m_strComment;
public:
	MACRO_SET_GET(File,   ustring, m_strFile)
	MACRO_SET_GET(Title,  ustring, m_strTitle)
	MACRO_SET_GET(Artist, ustring, m_strArtist)
	MACRO_SET_GET(Album,  ustring, m_strAlbum)
	MACRO_SET_GET(Genre,  ustring, m_strGenre)
	MACRO_SET_GET(Year,   ustring, m_strYear)
	MACRO_SET_GET(Track,  ustring, m_strTrack)
	MACRO_SET_GET(Comment,ustring, m_strComment)
public:
	CMP3Info(void)
	{
		m_strFile    = ustring((unsigned char*)"");
		m_strTitle   = ustring((unsigned char*)"");
		m_strArtist  = ustring((unsigned char*)"");
		m_strAlbum   = ustring((unsigned char*)"");
		m_strGenre   = ustring((unsigned char*)"");
		m_strYear    = ustring((unsigned char*)"");
		m_strTrack   = ustring((unsigned char*)"");
		m_strComment = ustring((unsigned char*)"");
	}
	CMP3Info(const CMP3Info& info){
		*this = info;
	}

	~CMP3Info(void)
	{
	}

	CMP3Info& operator=(const CMP3Info& info){
		this->m_strFile    = info.m_strFile; 
		this->m_strTitle   = info.m_strTitle;
		this->m_strArtist  = info.m_strArtist;
		this->m_strAlbum   = info.m_strAlbum;
		this->m_strGenre   = info.m_strGenre;
		this->m_strYear    = info.m_strYear;
		this->m_strTrack   = info.m_strTrack;
		this->m_strComment = info.m_strComment;

		return *this;
	}
	ustring toString(){
		ustring strRet = (unsigned char*)"";
		strRet += ustring((unsigned char*)"File\t:\t") + m_strFile + ustring((unsigned char*)"\n");
		
		if ( m_strTitle != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Title\t:\t") + m_strTitle + ustring((unsigned char*)"\n");
		
		if ( m_strArtist != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Artist\t:\t") + m_strArtist + ustring((unsigned char*)"\n");

		if ( m_strAlbum != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Album\t:\t") + m_strAlbum + ustring((unsigned char*)"\n");

		if ( m_strGenre != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Genre\t:\t") + m_strGenre + ustring((unsigned char*)"\n");

		if ( m_strYear != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Year\t:\t") + m_strYear + ustring((unsigned char*)"\n");

		if ( m_strTrack != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Track\t:\t") + m_strTrack + ustring((unsigned char*)"\n");

		if ( m_strComment != (unsigned char *)"" )
			strRet += ustring((unsigned char*)"Comment\t:\t") + m_strComment + ustring((unsigned char*)"\n");
		return strRet;
	}
};
