/*
** MP3ID3.cpp 
**
** Author: Yubo Dong (yubodong@gmail.com
**
** Copyright (C) 2006 Yubo Dong
** 
** This is a wrapper class for libid3tag-0.15.1b.
*/
#include <string.h>
//#include <io.h>
#include <fcntl.h>
//#include <share.h>
#include "MP3ID3.h"

CMP3ID3::CMP3ID3(void)
{
	m_pID3File =  NULL;
	m_pID3Tag  =  NULL;
}

CMP3ID3::CMP3ID3(string strFile, bool bReadOnly)
{
	m_pID3File =  NULL;
	m_pID3Tag  =  NULL;
	prepareMP3(strFile,bReadOnly);
}

CMP3ID3::~CMP3ID3(void)
{
	if ( m_pID3File != NULL )
		id3_file_close(m_pID3File);
}
/*
** bool prepareMP3(string strFile, bool bReadOnly)
**
** Prepare a mp3 file to retreive it's tag information if it has
**
** Parameter:
** strFile:		mp3 file with full pathname
** bReadOnly:	Indicate how you are going to open the file. 
*/
bool CMP3ID3::prepareMP3(string strFile, bool bReadOnly){
	if ( m_pID3File != NULL )
		id3_file_close(m_pID3File);

	int fileID = -1;
	if ( bReadOnly )
		fileID = open(strFile.c_str(),O_RDONLY);
	else
		fileID = open(strFile.c_str(),O_RDWR);

	if ( fileID == -1 ) return false;

	if ( bReadOnly )
		m_pID3File = id3_file_fdopen(fileID,ID3_FILE_MODE_READONLY);
	else
		m_pID3File = id3_file_fdopen(fileID,ID3_FILE_MODE_READWRITE);

	if ( m_pID3File == NULL ) return false;

	m_pID3Tag = id3_file_tag(m_pID3File);

	return true;
}
/*
** ustring getFrame(const char *frameID)
** 
** Return frame text by frameID. Frame ID is a four character string defined to uniquely
** identify a frame. Details see http://www.id3.org
** 
*/
ustring CMP3ID3::getFrame(const char *frameID){
	if ( m_pID3Tag == NULL || m_pID3File == NULL ) return ustring((unsigned char *)"");

	ustring str = ustring((unsigned char *)"");
	
	//Search for given frame by frame id
	struct id3_frame *pFrame = id3_tag_findframe(m_pID3Tag,frameID,0);
	if ( pFrame == NULL )  return ustring((unsigned char *)"");

	union id3_field field = pFrame->fields[1];
	id3_ucs4_t const *pTemp = id3_field_getstrings(&field,0);
	if ( !strcmp(frameID,"TCON") ){
		//If the frameID is TCON, we then retreive genre name using id3_genre_name
		id3_ucs4_t const *pGenre = id3_genre_name(pTemp);
		pTemp = pGenre;
	}

	id3_latin1_t *pStrLatinl;
	if ( pTemp != NULL ){
	   pStrLatinl = id3_ucs4_latin1duplicate(pTemp);
	   str = pStrLatinl;
	   delete pStrLatinl;
	}
	return str;
}
ustring CMP3ID3::getTitle(){
	return getFrame(ID3_FRAME_TITLE);
}
ustring CMP3ID3::getArtist(){
	return getFrame(ID3_FRAME_ARTIST);
}
ustring CMP3ID3::getAlbum(){
	return getFrame(ID3_FRAME_ALBUM);
}
ustring CMP3ID3::getYear(){
	return getFrame(ID3_FRAME_YEAR);
}
ustring CMP3ID3::getTrack(){
	return getFrame(ID3_FRAME_TRACK);
}
ustring CMP3ID3::getGenre(){
	return getFrame(ID3_FRAME_GENRE);
}
ustring CMP3ID3::getComment(){
	return getFrame(ID3_FRAME_COMMENT);
}
CMP3Info CMP3ID3::getMP3Info(){
	CMP3Info mp3;
	mp3.setTitle(getTitle());
	mp3.setArtist(getArtist());
	mp3.setAlbum(getAlbum());
	mp3.setYear(getYear());
	mp3.setTrack(getTrack());
	mp3.setGenre(getGenre());
	mp3.setComment(getComment());
	return mp3;
}
CMP3Info CMP3ID3::getMP3Info(string strFile,bool bReadOnly){
	prepareMP3(strFile,bReadOnly);
	CMP3Info mp3 = getMP3Info();
	mp3.setFile(ustring((unsigned char *)strFile.c_str()));
	return mp3;
}

/*
** long getTagLength(string strFile)
**
** This is a static method so that you can easily get the length of id3 tag if there is
** an id3 tag residing in the beginning of the MP3 file. This is useful when you try
** to decode a mp3 file. You can simply skip the id3 tag to the real audio data. However
** some exception exists such as a mp3 file with XING header or Lame information
*/
long CMP3ID3::getTagLength(string strFile){
	int fileID = -1;
	fileID = open(strFile.c_str(),O_RDONLY);
	if ( fileID != -1 ){
		unsigned char buffer[ID3_TAG_QUERYSIZE];
		read(fileID,(unsigned char *)buffer,ID3_TAG_QUERYSIZE);
		long lRet = id3_tag_query(buffer, ID3_TAG_QUERYSIZE);
		if ( lRet < 0 ) lRet = 0;
		close(fileID);
		return lRet;
	}
	return 0;
}