/*
 * File:   newfile.h
 * Author: norb
 *
 * Created on 16 lipiec 2010, 07:51
 */

#ifndef _NEWFILE_H
#define	_NEWFILE_H


#ifdef __cplusplus
extern "C" {
#endif
#define FB0_ADDRESS 0x47c18000
#define FB1_ADDRESS 0x47830000
#define FB2_ADDRESS 0x47448000




int map_fw_regs(void);
void unmap_lcdc_reg(void);
void set_fw1_win_size(int w, int h);
void set_fw1_win_pos(int x, int y);
void set_fw1_win_alpha(unsigned char alpha);
void fw1_enable(void);
void fw1_disable(void);

void set_fw2_win_size(int w, int h);
void set_fw2_win_pos(int x, int y);
void set_fw2_win_alpha(unsigned char alpha);
void fw2_enable(void);
void fw2_disable(void);

#ifdef __cplusplus
}
#endif
#endif	/* _NEWFILE_H */

