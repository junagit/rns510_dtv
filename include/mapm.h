#ifndef _MAPM_SASI
#define _MAPM_SASI


#define  MAPM_ERR_MAP   ((void *)-1)
#define  MAPM_ERR_FILE  ((void *)-2)

void  *mapm(unsigned int addr, unsigned int len);
/**<
map physical address to uncached user virtual address using /dev/mem
@param[in]  addr    physical address
@param[in]  len     size of memory in bytes to map
@return     virtual address if successful or MAPM_ERR_xxx on failure
*/

void  *mapm_cc(unsigned int addr, unsigned int len);
/**<
map physical address to cached user virtual address using /dev/mem
@param[in]  addr    physical address
@param[in]  len     size of memory in bytes to map
@return     virtual address if successful or MAPM_ERR_xxx on failure
*/

void  *fmapm(int fd, unsigned int addr, unsigned int len);
/**<
map physical offset of file descriptor to user virtual address
@param[in]  fd      open file descriptor
@param[in]  addr    physical address
@param[in]  len     size of memory in bytes to map
@return     virtual address if successful or MAPM_ERR_xxx on failure
*/

int      unmapm(void *addr, unsigned int len);
/**<
unmap user virtual address
@param[in]  addr  virtual address
@param[in]  len   size of memory in bytes mapped
@return     0 on success, else error
*/


/* internal */
void  *mapm_do(unsigned int addr, unsigned int len, int sync);
#define mapm(a, l)      mapm_do(a, l, 1)
#define mapm_cc(a, l)   mapm_do(a, l, 0)


#endif
