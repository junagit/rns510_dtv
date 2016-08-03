/* 
 * File:   pmt.h
 * Author: michalm
 *
 * Created on 6 maj 2011, 12:31
 */

#ifndef PMT_H
#define	PMT_H

#include "multiplex.h"

/**
 * @brief Klasa dla użytku wewnętrznego(prywatnego) biblioteki podczas skanowania.Pobiera informacje PMT.
 */
class dvb_pmt_data {
public:
    dvb_pmt_data();
    ~dvb_pmt_data();
    uint8_t find_pmt_for_services(dvb_multiplex *p);
private:
    section_usr sbuf;
    struct section *sect;
};

#endif	/* PMT_H */

