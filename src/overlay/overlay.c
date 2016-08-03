/** 
 * @file   overlay.cpp
 * @author Michał Motyl
 *
 * @date   1 sierpień 2012, 14:44
 */


#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>


volatile void *virt_addr_lcdc = NULL;
volatile void *map_base = NULL;

#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)

int current_w = 0;
int current_h = 0;
int current_x = 0;
int current_y = 0;


int map_fw_regs(void)
{
    int fd;
    off_t target = 0;


    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) 
            {
                printf("/dev/mem could not be opened.\n");
                perror("open");
                virt_addr_lcdc = NULL;
                exit(1);
            } 
            
            map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x04105000 & ~MAP_MASK);
            
            if(map_base == (void *) -1) {
                printf("Memory map failed.\n");
                perror("mmap");
                close(fd);
                virt_addr_lcdc = NULL;
                return -1;
            }
             
           virt_addr_lcdc = (map_base + (target & MAP_MASK));
          // printf("lcdc id :0x%x\n",*(unsigned long *)virt_addr_lcdc);
           close(fd);
          
           return 0;
}

void unmap_lcdc_reg(void)
{
    
    if(munmap((void*)map_base, MAP_SIZE) == -1) 
    {
        printf("Memory unmap failed.\n");	
    }
    virt_addr_lcdc = NULL;
    
}


void set_fw1_win_size(int w, int h)
{

    if(virt_addr_lcdc==NULL) return;
    if(w>=720) w = 719;
    
    if(h>=574) w = 573;
    unsigned long temp = (h<<16) | w;
    
    *(unsigned long *)((virt_addr_lcdc+0x8c)) = temp;

    
}


void set_fw1_win_pos(int x, int y)
{
    if(virt_addr_lcdc==NULL) return;
    unsigned long temp = (y<<16) | x;
    *(unsigned long *)(virt_addr_lcdc+0x88) = temp; 
    current_x = x;
    current_y = y;
    
}

void set_fw1_win_alpha(unsigned char alpha)
{
    unsigned long temp;
    if(virt_addr_lcdc==NULL) return;
    temp = *(unsigned long *)(virt_addr_lcdc+0x54); 
    temp |= alpha<<16;
    *(unsigned long *)(virt_addr_lcdc+0x54) = temp; 
}

void fw1_enable(void)
{
    unsigned long temp;
    
    if(virt_addr_lcdc==NULL) return;
    
    temp = *(unsigned long *)(virt_addr_lcdc+0x54); 
    temp |= 1<<0;
    *(unsigned long *)(virt_addr_lcdc+0x54) = temp; 
    
}


void fw1_disable(void)
{
    unsigned long temp;
    
    if(virt_addr_lcdc==NULL) return;
    
    temp = *(unsigned long *)(virt_addr_lcdc+0x54); 
    temp &= ~(1 << 0);
    *(unsigned long *)(virt_addr_lcdc+0x54) = temp; 
    
}



void set_fw2_win_size(int w, int h)
{
   if(w>=720) w = 719;
    
    if(h>=574) w = 573;
    if(virt_addr_lcdc==NULL) return;    
    unsigned long temp = (h<<16) | w;
    *(unsigned long *)(virt_addr_lcdc+0xA8) = temp;
    
}


void set_fw2_win_pos(int x, int y)
{
    if(virt_addr_lcdc==NULL) return;
    unsigned long temp = (y<<16) | x;
    *(unsigned long *)(virt_addr_lcdc+0xA4) = temp; 
    
}

void set_fw2_win_alpha(unsigned char alpha)
{
    if(virt_addr_lcdc==NULL) return;
    unsigned long temp;
    temp = *(unsigned long *)(virt_addr_lcdc+0x54); 
    temp |= alpha<<24;
    *(unsigned long *)(virt_addr_lcdc+0x54) = temp; 
}

void fw2_enable(void)
{
    if(virt_addr_lcdc==NULL) return;
    unsigned long temp;
    temp = *(unsigned long *)(virt_addr_lcdc+0x54); 
    temp |= 1<<1;
    *(unsigned long *)(virt_addr_lcdc+0x54) = temp; 
    
}


void fw2_disable(void)
{
    if(virt_addr_lcdc==NULL) return;
    unsigned long temp;
    temp = *(unsigned long *)(virt_addr_lcdc+0x54); 
    temp &= ~(1 << 1);
    *(unsigned long *)(virt_addr_lcdc+0x54) = temp; 
    
}

//MagusPtr        pMagus = NULL;
////MagusOverlayPtr pOverlay_one = NULL;
//MagusOverlayPtr pOverlay_two = NULL;
//
//MagusOverlayPtr pOverlay = NULL;
//
//#endif
//
//void destroy_overlay()
//{
//#ifdef MAGUS_OVERLAY
//	if(pMagus)
//	{
//		free(pMagus);
//		pMagus = NULL;
//	}
//	if(pOverlay_two)
//	{
//		free(pOverlay_two);
//		pOverlay_two = NULL;
//	}
//	if(pOverlay)
//	{
//		free(pOverlay);
//		pOverlay = NULL;
//	}
//
//#endif
//}
//
//void init_overlay()
//{
//#ifdef MAGUS_OVERLAY
//	pMagus = (MagusPtr) malloc (sizeof (Magus));
//    if (!pMagus)
//    {
//		printf ("Cannot allocate memory for Magus\n");
//		exit (1);
//    }
//    printf("dbg:%s %d\n",__FUNCTION__,__LINE__);
//    pOverlay = (MagusOverlayPtr) malloc (sizeof (MagusOverlay));
//    if (!pOverlay)
//    {
//		printf ("Cannot allocate memory for MagusOverlay\n");
//		free (pMagus);
//		exit (1);
//    }
//
//    if (!magusDrawInit (pMagus, pOverlay))
//    {
//		printf ("Cannot initialize magus drawable\n");
//		free (pMagus);
//		free (pOverlay);
//		exit (1);
//    }
//#endif
//	//initial_FW2();
//
//	initial_FW1();
//
//}
//
//void initial_FW1()
//{
//#ifdef MAGUS_OVERLAY
//    pOverlay->index = FLOAT_WIN_ONE;
//    pOverlay->alphaValue = 0xff;
//    pOverlay->alphaFormat = DISABLE_ALPHA_BLENDING;
//#if defined(TRUECOLOR)
//    pOverlay->bitsPerPixel = 32;
//#elif defined(PSEUDOCOLOR)
//	pOverlay->bitsPerPixel = 16;
//#endif
//    pOverlay->width = 720;
//    pOverlay->height = 576;
//    pOverlay->stride = pOverlay->width * (pOverlay->bitsPerPixel >> 3);
//    pOverlay->xoff = 0;
//    pOverlay->yoff = 0;
//
//    magusInitDrawable (pOverlay);
//    magusDrawEnable (pOverlay);
//#endif // <WE3HK-8623-029>
//
//}
//
//void initial_FW2()
//{
//#ifdef MAGUS_OVERLAY
//	/* initialize floating window 2 parameters */
//	pOverlay_two->index = FLOAT_WIN_TWO;
//	pOverlay_two->alphaValue = 0xff;
//	pOverlay_two->alphaFormat = ENABLE_GLOBAL_ALPHA;
//	pOverlay_two->bitsPerPixel = MAGUS_OVERLAY_BPP;
//	pOverlay_two->width = FW2_WIDTH;
//	pOverlay_two->height = FW2_HEIGHT;
//	pOverlay_two->stride = pOverlay_two->width * (pOverlay_two->bitsPerPixel >> 3);
//	pOverlay_two->xoff = FW2_OFFSET_X;
//	pOverlay_two->yoff = FW2_OFFSET_Y;
//	magusInitDrawable (pOverlay_two);
//	magusDrawDisable(pOverlay_two);  //2008-12-16 disable fw2 at beginning. (fix the under run problem )
///*	magusDrawImage(pOverlay_two, FW1_OFFSET_X, FW1_OFFSET_Y, MOVIE_OVERLAY_IMAGE);
//	magusDrawEnable(pOverlay_two);
//	deactivate();
// */
//#endif
//}





