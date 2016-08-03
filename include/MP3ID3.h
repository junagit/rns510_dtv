/*
** MP3ID3.h 
**
** Author: Yubo Dong (yubodong@gmail.com
**
** Copyright (C) 2006 Yubo Dong
** 
** This is a wrapper class for libid3tag-0.15.1b.
*/
#pragma once
#include "id3tag.h"
#include "MP3Info.h"

class CMP3ID3
{
private:
	struct id3_file*	m_pID3File;
	struct id3_tag*		m_pID3Tag;
public:
	CMP3ID3(void);
	CMP3ID3(string strgile, bool bReadOnly=true);
	~CMP3ID3(void);
public:
	bool		prepareMP3(string strFile, bool bReadOnly=true);
	ustring		getTitle();		//Return title
	ustring		getArtist();	//Return artist
	ustring		getAlbum();		//Return album
	ustring		getYear();		//Return recording time
	ustring		getTrack();		//Return original CD track 
	ustring		getGenre();		//Return genre
	ustring		getComment();	//Return comment
	CMP3Info	getMP3Info();	//Return all ID3 tag information in a class CMP3Info
	CMP3Info	getMP3Info(string strFile, bool bReadOnly=true);
	static		long	getTagLength(string strFile);	//get length of id3 tag
private:
	ustring		getFrame(const char *frameID);
};

