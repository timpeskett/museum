#include <stdio.h>
#include <windows.h>

typedef unsigned int uint;
typedef unsigned short ushort;

#define BMP_MAGIC_NUMBER 0x4d42
#define BMP_HEADER_LEN 0x36

struct BMP_HEADER
{
    ushort magic_number;
    uint BMP_size;
    ushort reserved1;
    ushort reserved2;
    uint data_offset;

    uint header_size;
    int bitmap_w;
    int bitmap_h;
    ushort col_planes;
    ushort bpp;
    uint compression;
    uint data_size;
    int horiz_res;
    int vert_res;
    uint col_palette;
    uint important;
};


struct BMP_UNIT
{
    unsigned char Blue;
    unsigned char Green;
    unsigned char Red;
    unsigned char Reserved;
};


int ReadHeader( FILE *infile, struct BMP_HEADER *hdr )
{
    char data[BMP_HEADER_LEN];

    if( fread( (void*)data, 1, BMP_HEADER_LEN, infile ) != BMP_HEADER_LEN )
    {
        return -1;
    }

    hdr->magic_number = *((ushort*)data);
    hdr->BMP_size = *((uint*)(data + 2));
    hdr->reserved1 = hdr->reserved2 = 0;
    hdr->data_offset = *((uint*)(data + 10));
    hdr->header_size = *((uint*)(data + 14));
    hdr->bitmap_w = *((int*)(data + 18));
    hdr->bitmap_h = *((int*)(data + 22));
    hdr->col_planes = *((ushort*)(data + 26));
    hdr->bpp = *((ushort*)(data + 28));
    hdr->compression = *((uint*)(data + 30));
    hdr->data_size = *((uint*)(data + 34));
    hdr->horiz_res = *((int*)(data + 38));
    hdr->vert_res = *((int*)(data + 42));
    hdr->col_palette = *((uint*)(data + 46));
    hdr->important = *((uint*)(data + 50));

    if( hdr->bitmap_h < 0 )
    {
        hdr->bitmap_h *= -1;
    }

    if( hdr->bitmap_w < 0 )
    {
        hdr->bitmap_w *= -1;
    }

    if( hdr->magic_number != BMP_MAGIC_NUMBER )
    {
        return -2;
    }

    return 0;
}


int ApplyFilter( struct BMP_UNIT *unit, char filter )
{
    unsigned char temp;

    switch( filter )
    {
        case 'i':
            unit->Blue = 255 - unit->Blue;
            unit->Green = 255 - unit->Green;
            unit->Red = 255 - unit->Red;
            break;
        case 'l':
            unit->Blue <<= 1;
            unit->Green <<= 1;
            unit->Red <<= 1;
            break;
        case 'r':
            unit->Blue >>=1;
            unit->Green >>= 1;
            unit->Red >>= 1;
            break;
        case 'h':
            unit->Blue = (unit->Blue <= 127) ? (127 - unit->Blue) : (127 + (255 - unit->Blue));
            unit->Green = (unit->Green <= 127) ? (127 - unit->Green) : (127 + (255 - unit->Green));
            unit->Red = (unit->Red <= 127) ? (127 - unit->Red) : (127 + (255 - unit->Red));
            break;
        case 'c':
            temp = unit->Red;
            unit->Red = unit->Blue;
            unit->Blue = unit->Green;
            unit->Green = temp;
            break;
        case 'g':
            unit->Red /= 3;
            unit->Green /= 2;
            unit->Blue /= 10;
            unit->Red = unit->Blue = unit->Green = unit->Red + unit->Blue + unit->Green;
            break;
        case 'B':
            unit->Red = unit->Green = 0;
            break;
        case 'G':
            unit->Red = unit->Blue = 0;
            break;
        case 'R':
            unit->Green = unit->Blue = 0;
            break;
        default:
            return -1;
            break;
    }

    return 0;
}


int main( int argc, char **argv )
{
    struct BMP_HEADER hdr;
    struct BMP_UNIT unit;
    FILE *bmpfile, *newfile;
    char filename[MAX_PATH + 3];
    int i, z, errorflag = 0, recvchar, numpad;

    if( argc < 3 )
    {
        printf("Usage:bitmapinv <filename> -[filter]\n[filter] -Letter indicating filter\nFilters:\n\ti - Inverse Filter\n\tl - Left Shift\
        \n\tr - Right Shift\n\th - Halfway Filter\n\tc - cycles RGB values for pixels\n\tg - Greyscale\n\tR/G/B - All Red/Green/Blue\n");
        return 0;
    }

    bmpfile = fopen( *(argv + 1), "rb" );

    if( bmpfile == NULL )
    {
        printf("Could not open file\n");
        return 0;
    }

    switch( ReadHeader( bmpfile, &hdr ) )
    {
        case -2:
            printf("Not a valid Bitmap file");
            fclose( bmpfile );
            return 0;
        case -1:
            printf("Could not read header\n");
            fclose( bmpfile );
            return 0;
        case 0:
            printf("Header read!\n");
            break;
        default:
            printf("Something screwed up, really badly\n");
            fclose( bmpfile );
            return 0;
    }


    printf("\nHeader Info:\n\tFile Size = %d\n\tBitmap Width = %d\n\tBitmap Height = %d\n\tBits Per Pixel = %d\n\tBMP Data Offset = %#x\n\n\
    ", hdr.BMP_size, hdr.bitmap_w, hdr.bitmap_h, hdr.bpp, hdr.data_offset );

    fclose(bmpfile );

    bmpfile = fopen( *(argv + 1), "rb" );

    if( bmpfile == NULL )
    {
        printf("Could not reopen file\n");
        return 0;
    }

    sprintf( filename, "new%s", *(argv+1) );

    newfile = fopen( filename, "wb" );

    if( newfile == NULL )
    {
        printf("Could not open new file\n");
        return 0;
    }

    for( i = 0; i < BMP_HEADER_LEN; i++ )
    {
        recvchar = getc( bmpfile );
        putc( recvchar, newfile );
    }

    switch(hdr.bpp)
    {
        case 1:
            errorflag = -4;
            break;
        case 4:
            errorflag = -4;
            break;
        case 8:
            for( i = 0; i < 256; i++ )
            {
                recvchar = fread( (void*)&unit, 1, 4, bmpfile );
                if( recvchar < 4 )
                {
                    errorflag = -1;
                    break;
                }
                if( ApplyFilter( &unit, *(*(argv + 2) + 1) ) )
                {
                    errorflag = -2;
                    break;
                }

                recvchar = fwrite( (void*)&unit, 1, 4, newfile );
                if( recvchar < 4 )
                {
                    errorflag = -3;
                    break;
                }
            }

            if( errorflag < 0 )
            {
                break;
            }

            for( i = 0; (unsigned int)i < hdr.data_size; i++ )
            {
                recvchar = getc( bmpfile );
                putc( recvchar, newfile );
            }

            printf("Done!\n");
            break;
        case 24:
            numpad = 4 - hdr.bitmap_w * 3 % 4;
            if( numpad == 4 ) numpad = 0;
            for( i = 0; i < hdr.bitmap_h; i++ )
            {
                for( z = 0; z < hdr.bitmap_w; z++ )
                {
                    recvchar = fread( (void*)&unit, 1, 3, bmpfile );
                    if( recvchar < 3 )
                    {
                        errorflag = -1;
                        break;
                    }
                    if( ApplyFilter( &unit, *(*(argv + 2) + 1) ) )
                    {
                        errorflag = -2;
                        break;
                    }

                    recvchar = fwrite( (void*)&unit, 1, 3, newfile );
                    if( recvchar < 3 )
                    {
                        errorflag = -3;
                        break;
                    }
                }
                if( errorflag < 0 )
                {
                    break;
                }
                for( z = 0; z < numpad; z++ ) ////////////For the padding at end of line
                {
                    recvchar = getc( bmpfile );
                    putc( recvchar, newfile );
                }
            }
            if( errorflag == 0 )
            {
                printf("Done!\n");
            }

            break;
        default:
            printf("Invalid BPP\n");
            break;
    }

    switch( errorflag )
    {
        case 0:
            break;
        case -1:
            printf("Read Error\n");
            break;
        case -2:
            printf("Invalid Filter\n");
            break;
        case -3:
            printf("Write Error\n");
            break;
        case -4:
            printf("Bpp not implemented yet\n");
        default:
            break;
    }

    fclose( bmpfile );
    fclose( newfile );

    if( errorflag < 0 )
    {
        remove( filename );
    }

    return 0;
}
