/** 
 * @file   MemTrack.h
 * @author michalm
 *
 * @date   17 kwiecień 2012, 14:44
 */

#ifndef MEMTRACK_H
#define	MEMTRACK_H

#include <typeinfo>

namespace MemTrack
{
    /* ---------------------------------------- class MemStamp */

    class MemStamp
    {
        public:        // member variables
            char const * const filename;
            int const lineNum;
        public:        // construction/destruction
            MemStamp(char const *filename, int lineNum)
                : filename(filename), lineNum(lineNum) { }
            ~MemStamp() { }
    };

    /* ---------------------------------------- memory allocation and stamping prototypes */

    void *TrackMalloc(size_t size);
    void TrackFree(void *p);
    void TrackStamp(void *p, const MemStamp &stamp, char const *typeName);
    void TrackDumpBlocks();
    void TrackListMemoryUsage();

    /* ---------------------------------------- operator * (MemStamp, ptr) */

    template <class T> inline T *operator*(const MemStamp &stamp, T *p)
    {
        TrackStamp(p, stamp, typeid(T).name());
        return p;
    }

}    // namespace MemTrack

/* ---------------------------------------- new macro */

#define MEMTRACK_NEW MemTrack::MemStamp(__FILE__, __LINE__) * new
#define new MEMTRACK_NEW


#endif	/* MEMTRACK_H */

